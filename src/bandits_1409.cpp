//
// Created by sdkiselev on 06/10/2025.
//

///  L−1 и H−1
///
#include <iostream>
#include <numeric>
#include "include/custom_allocator.hxx"
#include "include/my_list.hxx"

void solve_1409_with_list() {
    using Allocator = ExpandablePoolAllocator<int>;
    MyList<int, Allocator> data_list;
    int input_a, input_b;

    if (!(std::cin >> input_a >> input_b)) return;

    data_list.push_back(input_a);
    data_list.push_back(input_b);

    auto it = data_list.begin();
    int garry_shot = *it;

    ++it;
    int larry_shot = *it;

    int garry_missed = larry_shot - 1;
    int larry_missed = garry_shot - 1;

    std::cout << garry_missed << " " << larry_missed << std::endl;
}

int main() { solve_1409_with_list(); return 0; }
