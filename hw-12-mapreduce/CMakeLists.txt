cmake_minimum_required(VERSION 3.20)
project(mapreduce VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${SRC_DIR} ${TEST_DIR} ${BIN_DIR})

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_MEAN "Build mean calculation binaries" ON)
option(BUILD_VARIANCE "Build variance calculation binaries" ON)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -march=native)

set(MAPREDUCE_BINARIES)

if(BUILD_MEAN)
    add_executable(mapper_mean
            ${SRC_DIR}/mapper_mean.cpp
    )
    target_compile_options(mapper_mean PRIVATE -DMEAN_CALCULATION)
    list(APPEND MAPREDUCE_BINARIES mapper_mean)

    add_executable(reducer_mean
            ${SRC_DIR}/reducer_mean.cpp
    )
    target_compile_options(reducer_mean PRIVATE -DMEAN_CALCULATION)
    list(APPEND MAPREDUCE_BINARIES reducer_mean)
endif()

if(BUILD_VARIANCE)
    add_executable(mapper_variance
            ${SRC_DIR}/mapper_variance.cpp
    )
    target_compile_options(mapper_variance PRIVATE -DVARIANCE_CALCULATION)
    list(APPEND MAPREDUCE_BINARIES mapper_variance)

    add_executable(reducer_variance
            ${SRC_DIR}/reducer_variance.cpp
    )
    target_compile_options(reducer_variance PRIVATE -DVARIANCE_CALCULATION)
    list(APPEND MAPREDUCE_BINARIES reducer_variance)
endif()

foreach(binary ${MAPREDUCE_BINARIES})
    install(TARGETS ${binary} RUNTIME DESTINATION bin)
endforeach()

# if(BUILD_TESTS)
#     find_package(Catch2 3 QUIET)
#     if(Catch2_FOUND)
#         enable_testing()

#         add_executable(test_mapreduce ${TEST_DIR}/test_mapreduce.cpp)
#         target_link_libraries(test_mapreduce Catch2::Catch2WithMain)
#         include(CTest)
#         include(Catch)
#         catch_discover_tests(test_mapreduce)

#         install(TARGETS test_mapreduce RUNTIME DESTINATION bin)
#     endif()
# endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_locally.sh.in
        ${BIN_DIR}/run_locally.sh @ONLY)
install(PROGRAMS ${BIN_DIR}/run_locally.sh DESTINATION bin)

add_custom_target(run_mean
        COMMAND ${BIN_DIR}/run_locally.sh mean
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running mean calculation"
)

add_custom_target(run_variance
        COMMAND ${BIN_DIR}/run_locally.sh variance
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running variance calculation"
)

add_custom_target(test_all
        COMMAND ctest --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests"
)

# Псевдоцель для всех MapReduce бинарников
add_custom_target(mapreduce ALL DEPENDS ${MAPREDUCE_BINARIES})

# CPack для упаковки
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "sdkiselev@racoonatwork.ru")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
include(CPack)
