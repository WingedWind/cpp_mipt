cmake_minimum_required(VERSION 3.28)

project(rational_06_02 LANGUAGES CXX)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "Modules require Ninja generator")
endif()

add_executable(rational_06_02
    main.cpp
    rational.cpp
)

target_sources(rational_06_02
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            rational.cppm
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -stdlib=libc++")

target_compile_features(rational_06_02 PUBLIC cxx_std_23)

target_compile_options(rational_06_02 PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fmodules
    -stdlib=libc++
)