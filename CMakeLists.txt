INCLUDE(CheckIncludeFiles)

# define project
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_CONTACT "Damian R (damian@motoi.pro)")
set(PROJECT_WEBSITE "https://github.com/drojaazu")
set(PROJECT_COPYRIGHT "©2017 Motoi Productions / Released under MIT License")

set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# chrgfx library
project(chrgfx VERSION 2.1.0 LANGUAGES CXX)
add_subdirectory(chrgfx)

# conversion executables
project(chr2png VERSION 1.3.0 LANGUAGES CXX)
add_subdirectory(chr2png)
project(png2chr VERSION 1.3.0 LANGUAGES CXX)
add_subdirectory(png2chr)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

install(TARGETS chr2png png2chr
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
