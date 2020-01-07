cmake_minimum_required (VERSION 3.8.2)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "build/")
endforeach()

set(BUILD_SHARED_LIBS OFF CACHE STRING "" FORCE)

project("munchkin-online" VERSION 0.0.0
						  DESCRIPTION "Collab project that tries to bring the fun of table Munchkin to a worldwide experience")

set(MUNCHKIN_SOURCES 
    "src/api/game.cpp" "src/api/state.cpp" "src/api/card.cpp" "src/api/carddef.cpp"
    "src/api/card_loader.cpp" "src/api/gamerules.cpp" "src/api/battle.cpp" "src/renderer/state_debugger.cpp"
    "src/renderer/game_renderer.cpp" "src/renderer/render_target.cpp"
)
set(MUNCHKIN_INCLUDE_DIRECTORIES "include")
set(MUNCHKIN_LINK_LIBRARIES "")
set(MUNCHKIN_LINK_DIRECTORIES "")

add_subdirectory("lib")

add_executable (${PROJECT_NAME} "src/main.cpp" ${MUNCHKIN_SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

target_compile_features(${PROJECT_NAME} PUBLIC "cxx_std_17")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-deprecated-declarations")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MUNCHKIN_LINK_LIBRARIES})

# Tests

add_executable ("carddata-test" "tests/carddata.cpp" ${MUNCHKIN_SOURCES})
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
    target_compile_options("carddata-test" PUBLIC "-w")
endif()
target_compile_features("carddata-test" PUBLIC "cxx_std_17")
target_include_directories("carddata-test" PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries("carddata-test" PUBLIC ${MUNCHKIN_LINK_LIBRARIES})