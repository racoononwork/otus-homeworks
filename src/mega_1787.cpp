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

    // 1. Ввод k и n
    if (!(std::cin >> k >> n)) return;

    // 2. Ввод и запись всех a_i в MyList
    for (int i = 0; i < n; ++i) {
        int a_i;
        if (!(std::cin >> a_i)) return;
        arrivals_list.push_back(a_i);
    }

    int current_queue = 0;

    // 3. Итерация по MyList и вычисление
    for (int a_i : arrivals_list) {
        // Машины подъезжают
        current_queue += a_i;

        // Машины уезжают: current_queue -= k, но не меньше 0
        current_queue = std::max(0, current_queue - k);
    }

    std::cout << current_queue << std::endl;
}

int main() { solve_1787_with_list(); return 0; }