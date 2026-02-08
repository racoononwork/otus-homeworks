cmake_minimum_required(VERSION 3.20)
project(bulk_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS  filesystem program_options regex)
find_package(Threads REQUIRED)

# Библиотека async
add_library(async src/async/async.cpp src/async/global_state.cpp)
target_link_libraries(async PUBLIC spdlog::spdlog Threads::Threads)
target_include_directories(async PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Сервер bulk_server
add_executable(bulk_server src/bulk_server.cpp src/async/session.cpp)
target_link_libraries(bulk_server PRIVATE
        async
        spdlog::spdlog
        Threads::Threads
        Boost::filesystem
        Boost::regex
)
target_include_directories(bulk_server PRIVATE include)

# Тестовый cli (опционально)
add_executable(async_cli src/main.cpp)
target_link_libraries(async_cli PRIVATE async spdlog::spdlog Threads::Threads Boost::filesystem)

install(TARGETS async bulk_server RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(TARGETS async_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "bulk_server")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libspdlog-dev, libboost-all-dev")
include(CPack)
