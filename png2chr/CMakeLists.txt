aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" SRCFILES)
aux_source_directory("${PROJECT_SOURCE_DIR}/shared/src" SRCFILES)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/src/app.hpp")

add_executable(png2chr ${SRCFILES})

target_include_directories(png2chr
	PUBLIC "${PROJECT_SOURCE_DIR}/shared/src/"
	PUBLIC "${PROJECT_SOURCE_DIR}/chrgfx/inc/")

	target_link_libraries(png2chr LINK_PUBLIC chrgfx)
