cmake_minimum_required(VERSION 3.15)

add_subdirectory("freetype")
set(AUDEO_BUILD_EXAMPLES OFF)
add_subdirectory("audeo")

target_link_libraries(munchkin-online PRIVATE
        freetype
        audeo)

if (WIN32)
    target_link_libraries(munchkin-online PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/SDL2.lib"
            "${CMAKE_CURRENT_SOURCE_DIR}/lua53.lib"
            )
elseif (UNIX)
    message("Linux build, using Lua package")
    find_package(Lua REQUIRED)
    message("Lua library path: ${LUA_LIBRARIES}")
    target_link_libraries(munchkin-online PRIVATE
            ${LUA_LIBRARIES}
            dl
            glad
            GL
            SDL2
            )
else()
    message(FATAL_ERROR "Unknown system (Not WIN32 or UNIX), stopping generation")
endif()

target_include_directories(munchkin-online PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/doctest/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

if (WIN32)
    target_include_directories(munchkin-online PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/"
            "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/sdl"
            "${CMAKE_CURRENT_SOURCE_DIR}/lua/include"
            )
else ()
    message("Lua include path: ${LUA_INCLUDE_DIR}")
    target_include_directories(munchkin-online PRIVATE
            SDL2
            ${LUA_INCLUDE_DIR}
            PARENT_SCOPE
            )
endif ()

target_sources(munchkin-online PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_sdl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
        )

add_subdirectory("glad")