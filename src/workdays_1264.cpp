//
// Created by sdkiselev on 06/10/2025.
//

// output N * (M + 1)
#include <iostream>
#include "include/custom_allocator.hxx"
#include "include/my_list.hxx"

void solve_1264_with_list() {
    using Allocator = ExpandablePoolAllocator<long long>;
    MyList<long long, Allocator> params_list;
    long long N, M;

    // 1. Ввод и запись в MyList
    if (!(std::cin >> N >> M)) return;

    params_list.push_back(N);
    params_list.push_back(M);

    // 2. Получение данных из MyList
    auto it = params_list.begin();
    N = *it; ++it;
    M = *it;

    // 3. Вычисление: N * (M + 1)
    long long total_seconds = N * (M + 1);

    std::cout << total_seconds << std::endl;
}

int main() { solve_1264_with_list(); return 0; }