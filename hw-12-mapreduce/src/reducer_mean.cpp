#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, double> sums;
    std::unordered_map<std::string, long long> counts;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string key, value_str;
        iss >> key >> value_str;
        
        double value = std::stod(value_str);
        sums[key] += value;
        counts[key]++;
    }
    
    double total_sum = sums["mean"];
    long long total_count = counts["count"];
    double mean = total_sum / total_count;
    
    std::cout << "Average price: " << mean << std::endl;
    return 0;
}
