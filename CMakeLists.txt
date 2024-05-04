cmake_minimum_required(VERSION 3.23)

set(PROJECT_CONTACT "Damian R (damian@motoi.pro)")
set(PROJECT_WEBSITE "https://github.com/drojaazu")
set(PROJECT_COPYRIGHT "©2017 Motoi Productions / Released under MIT License")

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fstandalone-debug -DDEBUG" CACHE STRING "" FORCE)

# Include CCache for compilation caching if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message("using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
else()
    message("ccache not available")
endif()

# Interprocedural Optimisation if available
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    message("using interprocedural optimisation")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(SEND_ERROR "IPO not available: ${output}")
endif()

include(GNUInstallDirs)
include(CheckIncludeFiles)

project(chrgfx VERSION 2.1.1 LANGUAGES CXX)
add_subdirectory(src/chrgfx)

project(chr2png
  DESCRIPTION "Convert encoded graphics to indexed PNG"
  VERSION 1.3.0
  LANGUAGES CXX)
add_subdirectory(src/chr2png)

project(png2chr
  DESCRIPTION "Convert indexed PNG input to encoded graphics"
  VERSION 1.3.0
  LANGUAGES CXX)
add_subdirectory(src/png2chr)


if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

install(FILES src/gfxdef/gfxdefs
  DESTINATION ${CMAKE_INSTALL_DATADIR}/chrgfx)

#set(CMAKE_VERBOSE_MAKEFILE true)
