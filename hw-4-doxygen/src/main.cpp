#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <ranges>
#include <type_traits>
#include <concepts>
#include <string>
#include <algorithm>
#include <utility>

/**
 * @brief Проверяет, поддерживает ли тип `T` размер кортежа.
 *
 * @tparam T Тип, который будет проверяться.
 */
template<typename T>
concept HasTupleSize = requires {
    std::tuple_size<std::remove_cvref_t<T>>::value;
};

/**
 * @brief Проверяет однородность типов в кортеже.
 *
 * @tparam Tuple Кортеж, для которого будет проверяться однородность типов.
 * @tparam Is Индексы, используемые для проверки однородности.
 * @return true, если все типы одинаковые; иначе false.
 */
template<typename Tuple, std::size_t... Is>
consteval bool check_homogeneity(std::index_sequence<Is...>) {
    using T0 = std::tuple_element_t<0, Tuple>;

    return ((std::same_as<T0, std::tuple_element_t<Is, Tuple>>) && ...);
}

/**
 * @brief Проверяет, является ли кортеж однородным.
 *
 * @tparam T Тип, который будет проверяться.
 * @return true, если кортеж однородный; иначе false.
 */
template<typename T>
consteval bool is_homogeneous_tuple() {
    using RawT = std::remove_cvref_t<T>;
    constexpr auto size = std::tuple_size_v<RawT>;

    if constexpr (size == 0) {
        return true;
    } else {
        return check_homogeneity<RawT>(std::make_index_sequence<size>{});
    }
}

/**
 * @brief Проверяет, является ли тип строкоподобным (например, std::string).
 *
 * @tparam T Проверяемый тип.
 * @return true, если тип строкоподобный; иначе false.
 */
template<typename T>
concept StringLike = std::same_as<std::remove_cvref_t<T>, std::string>;

/**
 * @brief Проверяет, является ли тип контейнером для IP-адресов.
 *
 * @tparam T Тип, который будет проверяться.
 * @return true, если тип контейнер; иначе false.
 */
template <typename T>
concept IpContainer =
    std::ranges::range<T> &&
    !StringLike<T>;

/**
 * @brief Проверяет, является ли тип однородным кортежем.
 *
 * @tparam T Тип, который будет проверяться.
 * @return true, если тип однородный кортеж; иначе false.
 */
template <typename T>
concept HomogeneousTuple =
    !IpContainer<T> &&
    !StringLike<T> &&
    HasTupleSize<T> &&
    (is_homogeneous_tuple<T>());

/**
 * @brief Печатает IP-адрес для целочисленного типа.
 *
 * @tparam T Целочисленный тип.
 * @param value Значение IP-адреса.
 */
template<std::integral T>
void print_ip(T value) {
    using U = std::make_unsigned_t<T>;

    U unsigned_val = static_cast<U>(value);
    constexpr std::size_t bytes_count = sizeof(T);

    for (std::size_t i = 0; i < bytes_count; ++i) {
        int shift = (bytes_count - 1 - i) * 8; // 8 бит в байте
        unsigned int byte = (unsigned_val >> shift) & 0xFF;
        std::cout << byte << (i == bytes_count - 1 ? "" : ".");
    }

    std::cout << "\n";
}

/**
 * @brief Печатает IP-адрес, если значение является строкой.
 *
 * @tparam T Строкоподобный тип.
 * @param str Строковое представление IP-адреса.
 */
template<StringLike T>
void print_ip(T&& str) {
    std::cout << str << "\n";
}

/**
 * @brief Печатает IP-адрес из контейнера (например, vector, list).
 *
 * @tparam T Контейнер для IP-адреса.
 * @param container Контейнер, содержащий значения.
 */
template<IpContainer T>
void print_ip(T&& container) {
    bool first = true;

    for (const auto& element : container) {
        if (!first) std::cout << ".";
        std::cout << element;
        first = false;

    }

    std::cout << "\n";
}

/**
 * @brief Печатает IP-адрес из однородного кортежа.
 *
 * @tparam T Однородный кортеж.
 * @param tpl Кортеж, содержащий значения.
 */
template<HomogeneousTuple T>
void print_ip(T&& tpl) {
    std::apply([](const auto&... args) {
        std::size_t n = 0;
        ((std::cout << (n++ == 0 ? "" : ".") << args), ...);
    }, tpl);

    std::cout << "\n";
}

/**
 * @brief Главная функция программы.
 *
 * Выполняет тестовые вызовы функций печати IP-адресов для различных типов данных.
 *
 * @return 0 при успешном завершении.
 */
int main() {
    // Тестовые примеры
    print_ip(int8_t{-1});           // Ожидается: 255
    print_ip(int16_t{0});           // Ожидается: 0.0
    print_ip(int32_t{2130706433});  // Ожидается: 127.0.0.1
    print_ip(int64_t{-1});          // Ожидается: 255.255.255.255.255.255.255.255

    print_ip(std::string{"Hello, world!"}); // Ожидается: Hello, world!
    print_ip(std::vector<int>{100, 200, 300, 400}); // Ожидается: 100.200.300.400
    print_ip(std::list<int>{400, 300, 200, 100});   // Ожидается: 400.300.200.100
    print_ip(std::make_tuple(123, 456, 789)); // Ожидается: 123.456.789

    return 0;
}
