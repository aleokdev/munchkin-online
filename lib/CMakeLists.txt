cmake_minimum_required(VERSION 3.8)

set(MUNCHKIN_LINK_LIBRARIES
        ${MUNCHKIN_LINK_LIBRARIES}
        freetype
        audeo)

if (WIN32)
    set(MUNCHKIN_LINK_LIBRARIES ${MUNCHKIN_LINK_LIBRARIES}
            "${CMAKE_CURRENT_SOURCE_DIR}/SDL2.lib"
            "${CMAKE_CURRENT_SOURCE_DIR}/lua53.lib"
            PARENT_SCOPE
            )
else ()
    message("Assuming linux build, using Lua package")
    find_package(Lua REQUIRED)
    message("Lua library path: ${LUA_LIBRARIES}")
    set(MUNCHKIN_LINK_LIBRARIES ${MUNCHKIN_LINK_LIBRARIES}
            ${LUA_LIBRARIES}
            dl
            glad
            GL
            SDL2
            PARENT_SCOPE
            )
endif ()

set(MUNCHKIN_INCLUDE_DIRECTORIES
        ${MUNCHKIN_INCLUDE_DIRECTORIES}
        "${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/doctest/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples"
        "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/freetype/"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

if (WIN32)
    set(MUNCHKIN_INCLUDE_DIRECTORIES ${MUNCHKIN_INCLUDE_DIRECTORIES}
            "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/"
            "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/sdl"
            "${CMAKE_CURRENT_SOURCE_DIR}/lua/include"
            PARENT_SCOPE
            )
else ()
    message("Lua include path: ${LUA_INCLUDE_DIR}")
    set(MUNCHKIN_INCLUDE_DIRECTORIES ${MUNCHKIN_INCLUDE_DIRECTORIES}
            SDL2
            ${LUA_INCLUDE_DIR}
            PARENT_SCOPE
            )
endif ()

set(MUNCHKIN_SOURCES
        ${MUNCHKIN_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_sdl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
        PARENT_SCOPE
        )

add_subdirectory("glad")
add_subdirectory("freetype")
set(AUDEO_BUILD_EXAMPLES OFF)
add_subdirectory("audeo")