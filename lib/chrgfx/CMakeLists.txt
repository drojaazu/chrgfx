project(chrgfx VERSION 3.0.4 LANGUAGES CXX)
add_library(chrgfx SHARED)

target_sources(chrgfx
PRIVATE
  app.hpp
  builtin_defs.cpp
  chrconv.cpp
  chrdef.cpp
  colconv.cpp
  coldef.cpp
  custom.cpp
  gfxdef.cpp
  imageformat_png.cpp
  palconv.cpp
  paldef.cpp
  rgb_layout.cpp
  imaging.cpp
  utils.cpp
PUBLIC
  FILE_SET headers
  TYPE HEADERS
  FILES
    builtin_defs.hpp
    chrconv.hpp
    chrdef.hpp
    chrgfx.hpp
    colconv.hpp
    coldef.hpp
    custom.hpp
    gfxdef.hpp
    image.hpp
    image_types.hpp
    imageformat_png.hpp
    imaging.hpp
    palconv.hpp
    paldef.hpp
    rgb_layout.hpp
    strutil.hpp
    types.hpp
    utils.hpp
)

set_target_properties(chrgfx PROPERTIES SOVERSION 3)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp")

set_target_properties(chrgfx PROPERTIES PUBLIC_HEADER "${HEADERS}")

target_compile_features(chrgfx PUBLIC cxx_std_17)
target_link_libraries(chrgfx png)

install(TARGETS chrgfx
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET headers  
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
