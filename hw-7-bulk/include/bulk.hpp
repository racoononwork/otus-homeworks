#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <memory>
#include <list>
#include <optional>


template<typename... Args>
void print(std::string_view fmt, Args&&... args) {
std::cout << "LOG: " << fmt << " (upgrade compiler for std::print)\n";
}

struct Bulk {
    std::vector<std::string> commands;
    std::time_t created_at;

    Bulk() : created_at(std::time(nullptr)) {}

    void add(std::string&& cmd) {
        if (commands.empty()) {
            created_at = std::time(nullptr); 
        }
        commands.emplace_back(std::move(cmd));
    }

    bool empty() const { return commands.empty(); }
    size_t size() const { return commands.size(); }
    void clear() { commands.clear(); }
};

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const Bulk& bulk) = 0;
};

class ConsoleLogger : public IObserver {
public:
    void update(const Bulk& bulk) override {
        if (bulk.empty()) return;

        std::cout << "bulk: ";
        for (size_t i = 0; i < bulk.commands.size(); ++i) {
            std::cout << bulk.commands[i] << (i == bulk.commands.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    }
};

class FileLogger : public IObserver {
public:
    void update(const Bulk& bulk) override {
        if (bulk.empty()) return;

        std::string filename = "bulk" + std::to_string(bulk.created_at) + ".log";
    
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "bulk: ";
            for (size_t i = 0; i < bulk.commands.size(); ++i) {
                file << bulk.commands[i] << (i == bulk.commands.size() - 1 ? "" : ", ");
            }
            file << "\n";
        }
    }
};


class CommandParser {
private:
    size_t static_block_size_;
    std::vector<std::shared_ptr<IObserver>> subs_;
    
    Bulk current_block_;
    /// 0 = static, >=1 = dynamic
    int nesting_level_ = 0; 

public:
    CommandParser(size_t n) : static_block_size_(n) {}

    void subscribe(std::shared_ptr<IObserver> obs) {
        subs_.push_back(obs);
    }

    void process_line(std::string line) {
        if (line == "{") {
            if (nesting_level_ == 0) {
                flush_block();
                nesting_level_ = 1;
            } else {
                nesting_level_++;
            }
        } 
        else if (line == "}") {
            if (nesting_level_) {
                nesting_level_--;
                if (!nesting_level_) {
                    flush_block();
                }
            }
        } 
        else {
            current_block_.add(std::move(line));

            if (nesting_level_ == 0 && current_block_.size() >= static_block_size_) {
                flush_block();
            }
        }
    }

    // Вызывается при EOF
    void finish() {
        if (nesting_level_ == 0) {
            flush_block();
        } else {
            current_block_.clear();
        }
    }

private:
    void flush_block() {
        if (!current_block_.empty()) {
            for (auto& sub : subs_) {
                sub->update(current_block_);
            }
            current_block_.clear();
        }
    }
};
