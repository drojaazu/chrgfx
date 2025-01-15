project(chr2png
  DESCRIPTION "Convert encoded tile graphics to PNG image"
  VERSION 2.0.0
  LANGUAGES CXX)
	
add_executable(chr2png)

target_include_directories(chr2png
	PRIVATE "${PROJECT_SOURCE_DIR}/../shared"
	PRIVATE "${PROJECT_SOURCE_DIR}/../../lib"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp" ESCAPE_QUOTES)

target_sources(chr2png
PRIVATE
	main.cpp
	${PROJECT_SOURCE_DIR}/../shared/cfgload.cpp
	${PROJECT_SOURCE_DIR}/../shared/shared.cpp
	${PROJECT_SOURCE_DIR}/../shared/usage.cpp
	${PROJECT_SOURCE_DIR}/../shared/xdgdirs.cpp
)

target_link_libraries(chr2png PRIVATE chrgfx)



install(TARGETS chr2png RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
