#include <include/duplicates_with_boost.hpp>

int main(int argc, char** argv) {
    try {
        Settings settings;
        std::vector<std::string> include_raw, exclude_raw;

        po::options_description desc("Bayan File Duplicate Finder");
        desc.add_options()
            ("help,h", "Show help")
            ("include,i", po::value<std::vector<std::string>>(&include_raw), "Directories to scan")
            ("exclude,e", po::value<std::vector<std::string>>(&exclude_raw), "Directories to exclude")
            ("level,l", po::value<int>(&settings.max_depth)->default_value(0), "Scan depth (0 = only current dir)")
            ("min-size,m", po::value<uintmax_t>(&settings.min_size)->default_value(1), "Min file size (bytes)")
            ("block-size,s", po::value<size_t>(&settings.block_size)->default_value(1024), "Block size for hashing")
            ("mask", po::value<std::vector<std::string>>(&settings.masks), "Filename regex masks");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") || include_raw.empty()) {
            std::cout << desc << "\nExample:\n  bayan -i . -e ./build -s 1024\n";
            return 1;
        }

        for (const auto& s : include_raw) settings.include_dirs.emplace_back(s);
        for (const auto& s : exclude_raw) settings.exclude_dirs.emplace_back(s);

        DuplicateFinder finder(std::move(settings));
        finder.run();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}