//
// Created by sdkiselev on 06/10/2025.
//


// A % 2 == 0 || B % 2 == 1
#include <iostream>
#include <string>
#include "include/custom_allocator.hxx"
#include "include/my_list.hxx"

void solve_1877_with_list() {
    // Аллокатор для строк
    using Allocator = ExpandablePoolAllocator<std::string>;
    MyList<std::string, Allocator> codes_list;
    std::string code1_str, code2_str;

    if (!(std::cin >> code1_str >> code2_str)) return;

    codes_list.push_back(code1_str);
    codes_list.push_back(code2_str);

    auto it = codes_list.begin();

    const std::string& C1_str = *it;
    ++it;
    const std::string& C2_str = *it;

    int C1_int = std::stoi(C1_str);
    int C2_int = std::stoi(C2_str);

    if ((C1_int % 2 == 0) || (C2_int % 2 != 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
}

int main() { solve_1877_with_list(); return 0; }