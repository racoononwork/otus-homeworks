#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <ranges>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/crc.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;


struct Settings {
    std::vector<fs::path> include_dirs;
    std::vector<fs::path> exclude_dirs;
    std::vector<std::string> masks;
    size_t block_size = 1024;
    uintmax_t min_size = 1;
    int max_depth = 0;
};

std::string calculate_crc32(const std::vector<char>& buffer, size_t bytes_read) {
    boost::crc_32_type result;
    result.process_bytes(buffer.data(), bytes_read);
    
    if (bytes_read < buffer.size()) {
        for (size_t i = bytes_read; i < buffer.size(); ++i) {
            char zero = 0;
            result.process_byte(zero);
        }
    }
    return std::to_string(result.checksum());
}


class FileScanner {
private:
    const Settings& settings_;
    
    std::vector<boost::regex> mask_regexes_;
    [[ maybe_unused ]] std::vector<boost::regex> exclude_regexes_;

public:
    explicit FileScanner(const Settings& s) : settings_(s) {
        for (const auto& m : settings_.masks) {
            mask_regexes_.emplace_back(m, boost::regex::icase);
        }
    }


    auto scan() {
        std::map<uintmax_t, std::vector<fs::path>> size_groups;

        for (const auto& dir : settings_.include_dirs) {
            if (!fs::exists(dir)) continue;

            try {
                fs::recursive_directory_iterator it(dir);
                fs::recursive_directory_iterator end;

                while (it != end) {
                    const auto& entry = *it;
                    
                    if (settings_.max_depth >= 0 && it.depth() > settings_.max_depth) {
                        it.pop(); 
                        continue;
                    }

                    if (entry.is_directory() && is_path_excluded(entry.path())) {
                        it.disable_recursion_pending(); 
                        ++it;
                        continue;
                    }

                    if (entry.is_regular_file()) {
                        uintmax_t fsize = 0;
                        try { fsize = fs::file_size(entry); } catch(...) { ++it; continue; }

                        if (fsize >= settings_.min_size && 
                            is_filename_allowed(entry.path().filename().string())) {
                            size_groups[fsize].push_back(entry.path());
                        }
                        
                    }
                    ++it;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error scanning " << dir << ": " << e.what() << std::endl;
            }
        }
        return size_groups;
    }

private:
    bool is_filename_allowed(const std::string& filename) const {
        if (mask_regexes_.empty()) return true;
        return std::ranges::any_of(mask_regexes_, [&](const auto& re) {
            return boost::regex_match(filename, re);
        });
    }

    bool is_path_excluded(const fs::path& p) const {
        for (const auto& ex : settings_.exclude_dirs) {
            if (auto rel = fs::relative(p, ex); 
                (!rel.empty() && rel.native()[0] != '.') || p == ex) {
                return true;
            }
        }
        return false;
    }

};


class DuplicateFinder {
    Settings settings;

public:
    explicit DuplicateFinder(Settings s) : settings(std::move(s)) {}

    void run() {
        FileScanner scanner(settings);
        auto groups = scanner.scan();

        for (auto& [size, paths] : groups) {
            if (paths.size() < 2) continue;
            
            process_group(paths);
        }
    }

private:
    void process_group(const std::vector<fs::path>& paths) {
        std::vector<fs::path> current_paths = paths;
        check_blocks(current_paths, 0);
    }

    void check_blocks(std::vector<fs::path>& paths, size_t block_index) {
        if (paths.size() < 2) return;

        std::unordered_map<std::string, std::vector<fs::path>> hash_groups;
        
        std::vector<char> buffer(settings.block_size);
        bool any_file_ended = false;

        for (const auto& path : paths) {
            std::ifstream f(path, std::ios::binary);
            if (!f.is_open()) continue; 

            f.seekg(block_index * settings.block_size);
            
            if (f.fail()) { 
                // Proccessing empty block
                continue;
            }

            f.read(buffer.data(), settings.block_size);

            std::string hash = calculate_crc32(buffer, f.gcount());
            hash_groups[hash].emplace_back(path);

            const bool cond = (block_index + 1) * settings.block_size >= fs::file_size(path);
            any_file_ended = cond ? cond : any_file_ended; 
            
        }

        for (auto& [hash, sub_paths] : hash_groups) {
            if (sub_paths.size() < 2) continue;

            any_file_ended ? print_duplicates(sub_paths) : check_blocks(sub_paths, block_index + 1);
        }
    }

    void print_duplicates(const std::vector<fs::path>& paths) {
        for (const auto& p : paths) {
            std::cout << fs::absolute(p).string() << "\n";
        }
        std::cout << std::endl; 
    }
};
