#include <include/bulk.hpp>

int main(int argc, char* argv[]) {
    size_t n = 3; 
    if (argc > 1) {
        try {
            n = std::stoul(argv[1]);
        } catch (...) {
            std::cerr << "Invalid block size provided. Using default: 3\n";
        }
    }

    CommandParser parser(n);
    parser.subscribe(std::make_shared<ConsoleLogger>());
    parser.subscribe(std::make_shared<FileLogger>());

    std::string line;
    while (std::getline(std::cin, line)) {
        parser.process_line(std::move(line));
    }

    parser.finish();

    return 0;
}