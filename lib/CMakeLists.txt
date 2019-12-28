cmake_minimum_required (VERSION 3.8)

project(glad)
set(GLAD_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/glad/include/KHR/khrplatform.h"
	)

add_library(glad STATIC ${GLAD_SOURCES})
target_compile_options(glad PUBLIC -w)
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad" "${CMAKE_CURRENT_SOURCE_DIR}/glad")

add_subdirectory(sdl)
add_subdirectory(sol2)

set(MUNCHKIN_LINK_LIBRARIES
    ${MUNCHKIN_LINK_LIBRARIES}
    sdl
    glad
    PARENT_SCOPE
)

set(MUNCHKIN_INCLUDE_DIRECTORIES
    ${MUNCHKIN_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/sol2/include"
	PARENT_SCOPE
)