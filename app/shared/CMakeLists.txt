find_library(PNG_LIB png)

if(NOT PNG_LIB)
  message(FATAL_ERROR "libpng not found")
endif()

include(CheckIncludeFileCXX)
check_include_file_cxx("png++/png.hpp" PNGPP_H)
if(NOT PNGPP_H)
  message(FATAL_ERROR "png++ not found")
endif()
