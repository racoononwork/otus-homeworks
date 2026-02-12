#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string field;
        std::vector<std::string> fields;

        // Парсим CSV строку
        while (std::getline(iss, field, ',')) {
            fields.push_back(field);
        }

        if (fields.size() < 8) continue; // Минимально нужные поля

        // price обычно в столбце ~8 (нужно проверить датасет)
        std::string price_str = fields[8]; // price column
        try {
            double price = std::stod(price_str);
            if (price > 0) {
                std::cout << "mean\t" << price << "\n";
                std::cout << "count\t1\n";
            }
        } catch (...) {
            continue;
        }
    }
    return 0;
}
