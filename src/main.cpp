#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "include/custom_allocator.hxx"
#include "include/data_structs.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Подключаем наш контейнер
#include "include/my_list.hxx"

#include <cstddef>
#include <new>
#include <utility>
#include <algorithm>

// --- Вспомогательная функция для расчета факториала ---
long long factorial(int n) {
    if (n < 0) return 0; // Факториал отрицательных чисел не определен
    if (n == 0) return 1;
    long long res = 1;
    for (int i = 1; i <= n; ++i) {
        res *= i;
    }
    return res;
}


int main() {
    // 1. Создание std::map с обычным аллокатором
    std::cout << "--- 1. std::map with std::allocator ---" << std::endl;
    std::map<int, int> map_std_alloc;
    for (int i = 0; i < 10; ++i) {
        map_std_alloc[i] = factorial(i);
    }
    std::cout << "Filled std::map with 10 factorials." << std::endl << std::endl;


    // 2. Создание std::map с новым аллокатором
    std::cout << "--- 2. std::map with custom allocator ---" << std::endl;
    // Тип аллокатора для узлов map (пары ключ-значение)
    using MapAllocator = ExpandablePoolAllocator<std::pair<const int, int>>;
    std::map<int, int, std::less<int>, MapAllocator> map_custom_alloc;
    for (int i = 0; i < 10; ++i) {
        map_custom_alloc[i] = factorial(i);
    }
    std::cout << "Filled std::map with custom allocator with 10 factorials." << std::endl;


    // 3. Вывод на экран всех значений из map с кастомным аллокатором
    std::cout << "Contents of map with custom allocator:" << std::endl;
    for (const auto& pair : map_custom_alloc) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    std::cout << std::endl;


    // 4. Создание своего контейнера с обычным аллокатором
    std::cout << "--- 3. Custom container with std::allocator ---" << std::endl;
    MyList<int> list_std_alloc;
    for (int i = 0; i < 10; ++i) {
        list_std_alloc.push_back(i);
    }
    std::cout << "Filled custom container with numbers 0-9." << std::endl << std::endl;


    // 5. Создание своего контейнера с новым аллокатором
    std::cout << "--- 4. Custom container with custom allocator ---" << std::endl;
    // Аллокатор для int
    using ListAllocator = ExpandablePoolAllocator<int>;
    MyList<int, ListAllocator> list_custom_alloc;
    for (int i = 0; i < 10; ++i) {
        list_custom_alloc.push_back(i);
    }
    std::cout << "Filled custom container with custom allocator with numbers 0-9." << std::endl;


    // 6. Вывод на экран всех значений из своего контейнера
    std::cout << "Contents of custom container with custom allocator:" << std::endl;
    for (int value : list_custom_alloc) {
        std::cout << value << std::endl;
    }

    return 0;
}
