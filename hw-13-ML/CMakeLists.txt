cmake_minimum_required(VERSION 3.20)
project(fashion_mnist VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(spdlog REQUIRED)

file(MAKE_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/models
)

add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native -DNDEBUG)

add_executable(${PROJECT_NAME} src/main.cpp include/infra_ml/model.hpp include/infra_ml/models.hpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Eigen3::Eigen
        spdlog::spdlog
        Boost::program_options
        Boost::filesystem
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
