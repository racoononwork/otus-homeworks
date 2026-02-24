cmake_minimum_required(VERSION 3.14)

project(matrix
        VERSION 0.0.1
        LANGUAGES CXX)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH ${PATCH_VERSION})

option(WITH_UNIT_TESTS "Build unit tests (uses GoogleTest)" ON)

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

endfunction()

function(add_custom_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})

    target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME})

    target_include_directories(${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )


    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endfunction()

add_custom_library(${PROJECT_NAME}
        include/bulk.hpp
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
