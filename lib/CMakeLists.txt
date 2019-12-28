cmake_minimum_required (VERSION 3.8)

project(glad)
set(GLAD_SOURCES
	"glad/include/glad/glad.h"
	"glad/include/glad/src/glad.c"
	"glad/include/KHR/khrplatform.h"
	)

add_library(glad STATIC GLAD_SOURCES)

add_subdirectory(glfw)