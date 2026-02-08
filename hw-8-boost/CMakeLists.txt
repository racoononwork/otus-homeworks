cmake_minimum_required(VERSION 3.20)
project(bayan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS 
    filesystem 
    program_options 
    regex
)
include_directories(./)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

function(add_custom_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    
    # Линкуем Boost и системные библиотеки
    target_link_libraries(${TARGET_NAME} PRIVATE 
        Boost::filesystem
        Boost::program_options
        Boost::regex
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
    endif()

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
endfunction()

add_custom_executable(${PROJECT_NAME} src/main.cpp)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "boost-duplicate-finder")
set(CPACK_PACKAGE_CONTACT "kiselev.sd@racoonatwork.ru")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-filesystem-dev, libboost-program-options-dev, libboost-regex-dev")
include(CPack)
