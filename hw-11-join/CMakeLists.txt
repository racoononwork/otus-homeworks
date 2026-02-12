cmake_minimum_required(VERSION 3.20)
project(join_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS  filesystem program_options regex)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        spdlog::spdlog
        Threads::Threads
        Boost::filesystem
        Boost::regex
)
target_include_directories(${PROJECT_NAME} PRIVATE include)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "sdkiselev@racoonatwork.ru")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libspdlog-dev, libboost-all-dev")
include(CPack)
