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

    if (!(std::cin >> N >> M)) return;

    params_list.push_back(N);
    params_list.push_back(M);

    auto it = params_list.begin();
    N = *it; ++it;
    M = *it;

    long long total_seconds = N * (M + 1);

    std::cout << total_seconds << std::endl;
}

int main() { solve_1264_with_list(); return 0; }