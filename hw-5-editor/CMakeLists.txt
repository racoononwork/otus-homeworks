cmake_minimum_required(VERSION 3.20)

project(hw5_vector_redactor
    VERSION 0.0.1
    LANGUAGES CXX)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH ${PATCH_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(WITH_UNIT_TESTS "Build unit tests (uses GoogleTest)" ON)


function(add_custom_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})

    target_include_directories(${TARGET_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -std=c++20)
    endif()

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endfunction()

# --- Dependency Setup: Slint (Retained from original file) ---
find_package(Slint QUIET)

if (NOT Slint_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
    # Опционально: ускоряет сборку, пропуская тесты самого Slint
    CMAKE_ARGS -DSLINT_BUILD_TESTING=OFF -DSLINT_BUILD_EXAMPLES=OFF
  )
  FetchContent_MakeAvailable(Slint)
endif()

# Dependency Setup: Cap'n Proto (Retained from original file)
find_package(CapnProto CONFIG REQUIRED)
# Генерация кода из схемы .capnp
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS src/slint_vector_editor/serialization/vector_editor.capnp)

# --- Build Executable ---
add_custom_executable(VectorEditor src/main.cpp) # Используем кастомную функцию

# --- Project-Specific Dependency Linking (Required for Slint/CapnProto) ---

# Link libraries (Slint/CapnProto)
target_link_libraries(VectorEditor PRIVATE Slint::Slint CapnProto::capnp-rpc)

# Add generated CapnProto sources and headers
target_sources(VectorEditor PRIVATE ${CAPNP_SRCS} ${CAPNP_HDRS})

# Include directories for generated files
target_include_directories(VectorEditor PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Slint source generation
slint_target_sources(VectorEditor src/slint_vector_editor/view/editor.slint)

# CPack (DEB)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "vector_editor@example.com") # Использован заглушка контакта
include(CPack)