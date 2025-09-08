cmake_minimum_required(VERSION 3.14)

project(ip_filter
        VERSION 0.0.1
        LANGUAGES CXX)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH ${PATCH_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_UNIT_TESTS "Build unit tests (uses GoogleTest)" ON)

add_library(${PROJECT_NAME} STATIC src/ip_filter.cpp)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

add_executable(${PROJECT_NAME}_cli src/main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_features(${PROJECT_NAME}_cli PUBLIC cxx_std_23)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(${PROJECT_NAME}_cli
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_cli PRIVATE ${PROJECT_NAME} spdlog::spdlog)


if(MSVC)
    target_compile_options(${PROJECT_NAME}_cli PRIVATE /W4)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME}_cli PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

if(WITH_UNIT_TESTS)
    enable_testing()

    file(COPY tests/data DESTINATION ${CMAKE_BINARY_DIR})

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(test_version tests/test_version.cpp)
    target_link_libraries(test_version PRIVATE gtest_main ${PROJECT_NAME} spdlog::spdlog)
    target_include_directories(test_version PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    if(MSVC)
        target_compile_options(test_version PRIVATE /W4)
    else()
        target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()

    include(GoogleTest)
    gtest_discover_tests(test_version)
endif()

install(TARGETS ${PROJECT_NAME}_cli RUNTIME DESTINATION bin)

# CPack (DEB)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "kiselev.sd@racoonatwork.ru")
include(CPack)
