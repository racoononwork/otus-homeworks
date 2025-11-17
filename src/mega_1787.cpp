//
// Created by sdkiselev on 06/10/2025.
//

// k n
// 1..n

// ABS(Sum(1..n) - k * n)
#include <iostream>
#include <algorithm>
#include "include/custom_allocator.hxx"
#include "include/my_list.hxx"

void solve_1787_with_list() {
    using Allocator = ExpandablePoolAllocator<int>;
    MyList<int, Allocator> arrivals_list;
    int k, n;

    if (!(std::cin >> k >> n)) return;

    for (int i = 0; i < n; ++i) {
        int a_i;
        if (!(std::cin >> a_i)) return;
        arrivals_list.push_back(a_i);
    }

    int current_queue = 0;

    for (int a_i : arrivals_list) {
        current_queue += a_i;

        current_queue = std::max(0, current_queue - k);
    }

    std::cout << current_queue << std::endl;
}

int main() { solve_1787_with_list(); return 0; }