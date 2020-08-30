INCLUDE (CheckIncludeFiles)

# define project
cmake_minimum_required (VERSION 3.5)
project (chrgfx VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# chrgfx library
add_subdirectory(chrgfx)
# conversion executables
add_subdirectory(chr2png)
add_subdirectory(png2chr)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()


install(TARGETS chr2png
        RUNTIME DESTINATION bin)
