add_executable(png2chr)

target_include_directories(png2chr
	PUBLIC "${PROJECT_SOURCE_DIR}/src/shared"
)

target_sources(png2chr
PRIVATE
	app.hpp
	main.cpp
	${PROJECT_SOURCE_DIR}/src/shared/defblocks.cpp
	${PROJECT_SOURCE_DIR}/src/shared/filesys.hpp
	${PROJECT_SOURCE_DIR}/src/shared/fstreams.hpp
	${PROJECT_SOURCE_DIR}/src/shared/gfxdef_builder.hpp
	${PROJECT_SOURCE_DIR}/src/shared/gfxprofile.cpp
	${PROJECT_SOURCE_DIR}/src/shared/import_defs.cpp
	${PROJECT_SOURCE_DIR}/src/shared/shared.cpp
	${PROJECT_SOURCE_DIR}/src/shared/strutil.hpp
	${PROJECT_SOURCE_DIR}/src/shared/usage.cpp
	${PROJECT_SOURCE_DIR}/src/shared/xdgdirs.hpp
)

set(PROJECT_BRIEF "Convert indexed PNG input to encoded graphics")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp" ESCAPE_QUOTES)

target_link_libraries(png2chr PRIVATE chrgfx)
