INCLUDE (CheckIncludeFiles)

# define project
cmake_minimum_required (VERSION 3.5)
project (chrgfx VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)

# define target
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(${PROJECT_SOURCE_DIR}/src CPPFILES)
add_executable(chrgfx ${CPPFILES}) 

find_library(PNG_LIB png)
if(NOT PNG_LIB)
  message(FATAL_ERROR "libpng not found")
endif()

check_include_files("png++/png.hpp" PNGPP_H)
if(NOT PNGPP_H)
  message(FATAL_ERROR "png++ not found")
endif()

target_include_directories(chrgfx PUBLIC "${PROJECT_SOURCE_DIR}/inc")
target_compile_features(chrgfx PUBLIC cxx_std_11)
target_link_libraries(chrgfx png)



install(TARGETS chrgfx
        RUNTIME DESTINATION bin)
