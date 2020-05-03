aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCFILES)
add_library(libchrgfx ${SRCFILES})

find_library(PNG_LIB png)
if(NOT PNG_LIB)
  message(FATAL_ERROR "libpng not found")
endif()

check_include_files("png++/png.hpp" PNGPP_H)
if(NOT PNGPP_H)
  message(FATAL_ERROR "png++ not found")
endif()

target_include_directories(libchrgfx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_compile_features(libchrgfx PUBLIC cxx_std_11)
target_link_libraries(libchrgfx png)

target_include_directories(libchrgfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
