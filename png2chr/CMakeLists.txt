aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCFILES)
add_executable(png2chr ${SRCFILES})

target_include_directories(chr2png PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_include_directories(chr2png PUBLIC "${PROJECT_SOURCE_DIR}/shared/inc")

target_link_libraries(png2chr LINK_PUBLIC libchrgfx)
