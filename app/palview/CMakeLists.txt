project(palview
  DESCRIPTION "Visualize an encoded color palette"
  VERSION 1.0.0
  LANGUAGES CXX)
	
add_executable(palview)

target_include_directories(palview
	PRIVATE "${PROJECT_SOURCE_DIR}/../shared"
	PRIVATE "${PROJECT_SOURCE_DIR}/../../lib"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp" ESCAPE_QUOTES)

target_sources(palview
PRIVATE
	app.hpp
	main.cpp
	${PROJECT_SOURCE_DIR}/../shared/cfgload.cpp
	${PROJECT_SOURCE_DIR}/../shared/filesys.hpp
	${PROJECT_SOURCE_DIR}/../shared/fstreams.hpp
	${PROJECT_SOURCE_DIR}/../shared/gfxdef_builder.hpp
	${PROJECT_SOURCE_DIR}/../shared/gfxdefman.hpp
	${PROJECT_SOURCE_DIR}/../shared/shared.cpp
	${PROJECT_SOURCE_DIR}/../shared/strutil.hpp
	${PROJECT_SOURCE_DIR}/../shared/xdgdirs.cpp
	${PROJECT_SOURCE_DIR}/../shared/usage.cpp
)

target_link_libraries(palview PRIVATE chrgfx)

install(TARGETS palview RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
