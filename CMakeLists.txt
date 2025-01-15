cmake_minimum_required(VERSION 3.23)
project(chrgfx)

set(PROJECT_CONTACT "Damian R (damian@motoi.pro)")
set(PROJECT_WEBSITE "https://github.com/drojaazu")
set(PROJECT_COPYRIGHT "©2017 Motoi Productions / Released under MIT License")

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
endif()

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fstandalone-debug -DDEBUG" CACHE STRING "" FORCE)

include(GNUInstallDirs)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

add_subdirectory(lib/chrgfx)
add_subdirectory(app/chr2png)
add_subdirectory(app/png2chr)
add_subdirectory(app/palview)

install(FILES share/gfxdef/gfxdefs
  DESTINATION ${CMAKE_INSTALL_DATADIR}/chrgfx)

#set(CMAKE_VERBOSE_MAKEFILE true)
