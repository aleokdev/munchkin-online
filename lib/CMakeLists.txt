cmake_minimum_required(VERSION 3.15)

add_subdirectory(freetype)
set(AUDEO_BUILD_EXAMPLES OFF)
add_subdirectory(audeo)
add_subdirectory(glad)

add_library(sdl INTERFACE)
if (WIN32)
    target_include_directories(sdl INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include")
    target_include_directories(sdl INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/sdl")
    target_link_libraries(sdl INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/SDL2.lib"
            )
elseif (UNIX)
    target_link_libraries(sdl INTERFACE
            dl
            GL
            SDL2
            )
else()
    message(FATAL_ERROR "Unknown system (Not WIN32 or UNIX), stopping generation")
endif()

add_library(imgui STATIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_sdl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.cpp")
target_link_libraries(imgui PRIVATE sdl glad)
target_include_directories(imgui PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples"
        )
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_library(extlibs INTERFACE)
target_link_libraries(extlibs INTERFACE
        freetype
        audeo
        imgui
        sdl
        glad)

if (WIN32)
    target_link_libraries(extlibs INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/lua53.lib"
            )
elseif (UNIX)
    message(STATUS "Linux build, using Lua package")
    find_package(Lua REQUIRED)
    message(STATUS "Lua library path: ${LUA_LIBRARIES}")
    target_link_libraries(extlibs INTERFACE
            ${LUA_LIBRARIES}
            )
else()
    message(FATAL_ERROR "Unknown system (Not WIN32 or UNIX), stopping generation")
endif()

target_include_directories(extlibs INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/doctest/include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

if (WIN32)
    target_include_directories(extlibs INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/lua/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
            )
else ()
    message("Lua include path: ${LUA_INCLUDE_DIR}")
    target_include_directories(extlibs INTERFACE
            ${LUA_INCLUDE_DIR}
            )
endif ()