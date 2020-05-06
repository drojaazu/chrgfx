aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCFILES)
add_library(chrgfx SHARED ${SRCFILES})

set_target_properties(chrgfx PROPERTIES SOVERSION 1)

set_target_properties(chrgfx PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/inc/chrgfx.hpp)

find_library(PNG_LIB png)
if(NOT PNG_LIB)
  message(FATAL_ERROR "libpng not found")
endif()

check_include_files("png++/png.hpp" PNGPP_H)
if(NOT PNGPP_H)
  message(FATAL_ERROR "png++ not found")
endif()

#target_include_directories(chrgfx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_compile_features(chrgfx PUBLIC cxx_std_11)
target_link_libraries(chrgfx png)

include(GNUInstallDirs)

install(TARGETS chrgfx
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

