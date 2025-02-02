project(png2chr
  DESCRIPTION "Convert PNG image input to encoded tile graphics"
  VERSION 2.0.0
  LANGUAGES CXX)

add_executable(png2chr)

target_include_directories(png2chr
	PRIVATE "${PROJECT_SOURCE_DIR}/../shared"
	PRIVATE "${PROJECT_SOURCE_DIR}/../../lib"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp" ESCAPE_QUOTES)

target_sources(png2chr
PRIVATE
	main.cpp
	${PROJECT_SOURCE_DIR}/../shared/cfgload.cpp
	${PROJECT_SOURCE_DIR}/../shared/shared.cpp
	${PROJECT_SOURCE_DIR}/../shared/usage.cpp
	${PROJECT_SOURCE_DIR}/../shared/xdgdirs.cpp
)

target_link_libraries(png2chr PRIVATE chrgfx)

install(TARGETS png2chr RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
