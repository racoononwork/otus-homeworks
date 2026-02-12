#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>

int main() {
    std::vector<double> prices;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string key, value_str;
        iss >> key >> value_str;
        
        if (key == "price") {
            try {
                double price = std::stod(value_str);
                prices.push_back(price);
            } catch (...) {}
        }
    }
    
    if (prices.empty()) {
        std::cout << "Variance: 0" << std::endl;
        return 0;
    }
    
    // Вычисляем среднее
    double mean = std::accumulate(prices.begin(), prices.end(), 0.0) / prices.size();
    
    // Вычисляем дисперсию
    double variance = 0;
    for (double price : prices) {
        variance += (price - mean) * (price - mean);
    }
    variance /= prices.size();
    
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Mean (for check): " << mean << std::endl;
    return 0;
}
