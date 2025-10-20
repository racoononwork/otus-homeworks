//
// Created by sdkiselev on 06/10/2025.
//

// a_1 = x+ a_3
// b_1 = y + b_2

// x + y = a_2 - x = b_3 -y
// x = a_1 - a_3
// y = b_1 - b_2
#include <iostream>
#include "include/custom_allocator.hxx"
#include "include/my_list.hxx"

void solve_2001_with_list() {
    using Allocator = ExpandablePoolAllocator<int>;
    MyList<int, Allocator> measurements;
    int val_a, val_b;

    // 1. Ввод и запись в MyList (a1, b1, a2, b2, a3, b3)
    for (int i = 0; i < 3; ++i) {
        if (!(std::cin >> val_a >> val_b)) return;
        measurements.push_back(val_a);
        measurements.push_back(val_b);
    }

    if (measurements.size() != 6) return;

    // 2. Получение данных из MyList по итератору
    auto it = measurements.begin();
    int a1 = *it; ++it; // 0
    int b1 = *it; ++it; // 1
    [[maybe_unused]] int a2 = *it; ++it; // 2
    int b2 = *it; ++it; // 3 - Корзина 2
    int a3 = *it; ++it; // 4 - Корзина 1
    [[maybe_unused]] int b3 = *it;        // 5

    // 3. Вычисление:
    // X = a1 - a3 (Масса с ягодами - Масса пустой корзины)
    int berries_first = a1 - a3;
    // Y = b1 - b2
    int berries_second = b1 - b2;

    std::cout << berries_first << " " << berries_second << std::endl;
}

int main() { solve_2001_with_list(); return 0; }