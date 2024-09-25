
add_library(chrgfx SHARED)

target_sources(chrgfx
PRIVATE
  app.hpp
  basic_gfx.cpp
  builtin_defs.cpp
  chrconv.cpp
  chrdef.cpp
  colconv.cpp
  coldef.cpp
  custom.cpp
  filesys.hpp
  gfxdef.cpp
  imgfmt_png.cpp
  palconv.cpp
  paldef.cpp
  rgb_layout.cpp
  strutil.hpp
  chrsetconv.cpp
  utils.cpp
PUBLIC
  FILE_SET headers
  TYPE HEADERS
  FILES
    basic_gfx.hpp
    builtin_defs.hpp
    chrconv.hpp
    chrdef.hpp
    chrgfx.hpp
    colconv.hpp
    coldef.hpp
    custom.hpp
    filesys.hpp
    gfxdef.hpp
    imgfmt_png.hpp
    palconv.hpp
    paldef.hpp
    rgb_layout.hpp  
    chrsetconv.hpp
    types.hpp
    utils.hpp
)

set_target_properties(chrgfx PROPERTIES SOVERSION 2)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp")

set_target_properties(chrgfx PROPERTIES PUBLIC_HEADER "${HEADERS}")

find_library(PNG_LIB png)

if(NOT PNG_LIB)
  message(FATAL_ERROR "libpng not found")
endif()

check_include_files("png++/png.hpp" PNGPP_H)

if(NOT PNGPP_H)
  message(FATAL_ERROR "png++ not found")
endif()

target_compile_features(chrgfx PUBLIC cxx_std_17)
target_link_libraries(chrgfx png)

install(TARGETS chrgfx
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET headers  
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
