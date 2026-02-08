cmake_minimum_required(VERSION 3.14)

project(matrix
        VERSION 0.0.1
        LANGUAGES CXX)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH ${PATCH_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(WITH_UNIT_TESTS "Build unit tests (uses GoogleTest)" ON)
cmake_minimum_required(VERSION 3.20)
project(CustomDataStructures VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


function(add_custom_library TARGET_NAME)
    add_library(${TARGET_NAME} ${ARGN})

    target_include_directories(${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} INTERFACE /W4)
    else()
        target_compile_options(${TARGET_NAME} INTERFACE -Wall -Wextra -pedantic -Werror)
    endif()
endfunction()

function(add_custom_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})

    target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME})

    target_include_directories(${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endfunction()

add_custom_library(${PROJECT_NAME}
        include/infinity_matrix.hpp
        src/main.cpp
)

add_custom_executable(${PROJECT_NAME}_cli src/main.cpp)



# CPack (DEB)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "kiselev.sd@racoonatwork.ru")
include(CPack)
