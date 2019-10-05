include_directories(${PROJECT_SOURCE_DIR}/inc)
aux_source_directory(${PROJECT_SOURCE_DIR}/src CPPFILES)
add_executable(chrgfx ${CPPFILES})
target_link_libraries(chrgfx png)
