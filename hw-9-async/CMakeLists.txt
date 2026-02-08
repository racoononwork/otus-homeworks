cmake_minimum_required(VERSION 3.20)
project(async VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem program_options regex)
find_package(Threads REQUIRED)  # Для jthread

include_directories(./include)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

function(add_custom_library TARGET_NAME)
    add_library(${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog Threads::Threads)
    target_include_directories(${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
            ./include
    )
    if(MSVC)
        target_compile_options(${TARGET_NAME} INTERFACE /W4)
    else()
        target_compile_options(${TARGET_NAME} INTERFACE -Wall -Wextra -pedantic -Werror)
    endif()
endfunction()

function(add_custom_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE
            Boost::filesystem Boost::program_options Boost::regex
            ${PROJECT_NAME}
            spdlog::spdlog Threads::Threads
    )
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
    endif()
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endfunction()

add_custom_library(${PROJECT_NAME} src/async/async.cpp src/async/global_state.cpp)

add_custom_executable(${PROJECT_NAME}_cli src/main.cpp)

# Установка библиотеки
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

# Пакет async
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "async")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libspdlog-dev, libboost-filesystem-dev, libboost-program-options-dev, libboost-regex-dev")
include(CPack)
