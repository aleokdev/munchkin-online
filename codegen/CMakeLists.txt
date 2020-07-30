cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(munchkin-codegen src/main.cpp src/parser.cpp src/lua_bindings.cpp)
set_property(TARGET munchkin-codegen PROPERTY CXX_STANDARD 17)
target_link_libraries(munchkin-codegen PRIVATE extlibs)
target_include_directories(munchkin-codegen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CODEGEN_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/scripts/systems.lua)
file(GLOB CODEGEN_DEPENDENCIES ${PROJECT_SOURCE_DIR}/include/systems/*)
set(CODEGEN_DEPENDENCIES ${CODEGEN_DEPENDENCIES} ${CODEGEN_SCRIPT})
set(CODEGEN_GENERATED_FILES "${PROJECT_BINARY_DIR}/include/game_systems_cg.hpp"
        "${PROJECT_BINARY_DIR}/include/render_systems_cg.hpp"
        "${PROJECT_BINARY_DIR}/include/game_wrapper_defs_cg.hpp"
        "${PROJECT_BINARY_DIR}/include/render_wrapper_defs_cg.hpp")
add_custom_command(OUTPUT ${CODEGEN_GENERATED_FILES}
        COMMAND munchkin-codegen ${CODEGEN_SCRIPT} ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${CODEGEN_DEPENDENCIES}
        )

add_custom_target(run-munchkin-codegen
        DEPENDS ${CODEGEN_GENERATED_FILES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Updating codegen...")