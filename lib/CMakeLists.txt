cmake_minimum_required (VERSION 3.8)

set(MUNCHKIN_LINK_LIBRARIES
	"${CMAKE_CURRENT_SOURCE_DIR}/SDL2.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/lua53.lib"
	PARENT_SCOPE)

set(MUNCHKIN_INCLUDE_DIRECTORIES
    ${MUNCHKIN_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/sdl"
	"${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/lua/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/doctest/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples"
	"${CMAKE_CURRENT_SOURCE_DIR}/glad/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/freetype/include/"
	PARENT_SCOPE
)

set(MUNCHKIN_SOURCES
	${MUNCHKIN_SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_sdl.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
	PARENT_SCOPE
)