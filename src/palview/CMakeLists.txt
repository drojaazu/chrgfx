add_executable(palview)

target_include_directories(palview
	PRIVATE "${PROJECT_SOURCE_DIR}/src/shared"
	PRIVATE "${PROJECT_SOURCE_DIR}/src"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.hpp.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/app.hpp" ESCAPE_QUOTES)

target_sources(palview
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

target_link_libraries(palview PRIVATE chrgfx)

install(TARGETS palview RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
