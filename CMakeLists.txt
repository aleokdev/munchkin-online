cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32 AND NOT ${CMAKE_RC_COMPILER} STREQUAL rc)
    enable_language(RC)
    set(CMAKE_RC_FLAGS ${CMAKE_RC_FLAGS}
            "/i \"${WINDOWS_H_INCLUDE_DIR}\" /i \"${WINDOWS_SHARED_INCLUDE_DIR}\"")
    set(CMAKE_RC_COMPILER rc)
endif ()

foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "build/")
endforeach ()

set(BUILD_SHARED_LIBS OFF CACHE STRING "" FORCE)

project("munchkin-online" VERSION 0.0.0
        DESCRIPTION "Collab project that tries to bring the fun of table Munchkin to a worldwide experience")

set(MUNCHKIN_SOURCES
        "src/game.cpp" "src/api/state.cpp" "src/api/card.cpp" "src/api/carddef.cpp"
        "src/api/card_loader.cpp" "src/api/gamerules.cpp" "src/api/battle.cpp"
        "src/systems/game_renderer.cpp" "src/renderer/render_target.cpp" "src/renderer/background_renderer.cpp"
        "src/renderer/stb_image.cpp" "src/renderer/util.cpp" "src/renderer/uniform_buffer.cpp"
        "src/systems/title_screen_renderer.cpp"
        "src/systems/ai_manager.cpp"
        "src/renderer/sprite_renderer.cpp"
        "src/input/input.cpp" "src/renderer/card_sprite.cpp" "src/renderer/asset_loaders.cpp"
        "src/systems/input_binder.cpp"
        "src/systems/state_debugger.cpp"
        "src/renderer/font_renderer.cpp"
        "src/renderer/font.cpp" "src/game_wrapper.cpp"
        "src/systems/debug_terminal.cpp"
        "src/systems/game_logger.cpp"
        "src/api/player.cpp"
        "src/render_wrapper.cpp"
        )
set(MUNCHKIN_INCLUDE_DIRECTORIES "include" "lib" "lib/glm")
set(MUNCHKIN_LINK_LIBRARIES "")
set(MUNCHKIN_LINK_DIRECTORIES "")

add_subdirectory("lib")

add_executable(${PROJECT_NAME} "src/main.cpp" ${MUNCHKIN_SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

target_compile_features(${PROJECT_NAME} PUBLIC "cxx_std_17")
if (UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SOL_USING_CXX_LUA)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-deprecated-declarations" "-Wall" "-Wno-unused-private-field"
            "-Wno-reorder")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MUNCHKIN_LINK_LIBRARIES})

# Tests

if (BUILD_CARDDATA_TEST)
    add_executable("carddata-test" "tests/carddata.cpp" ${MUNCHKIN_SOURCES})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options("carddata-test" PUBLIC "-w")
    endif ()
    target_compile_features("carddata-test" PUBLIC "cxx_std_17")
    target_include_directories("carddata-test" PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
    target_link_libraries("carddata-test" PUBLIC ${MUNCHKIN_LINK_LIBRARIES})
    if (UNIX)
        target_compile_definitions("carddata-test" PRIVATE SOL_USING_CXX_LUA)
    endif ()
endif ()