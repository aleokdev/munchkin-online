cmake_minimum_required (VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, "${CMAKE_BINARY_DIR}/bin")

project("munchkin-online" VERSION 0.0.0
						  DESCRIPTION "Collab project that tries to bring the fun of table Munchkin to a worldwide experience")

set(MUNCHKIN_SOURCES "src/main.cpp")
set(MUNCHKIN_INCLUDE_DIRECTORIES "")
set(MUNCHKIN_LINK_LIBRARIES "")

add_subdirectory("lib")

add_executable (${PROJECT_NAME} ${MUNCHKIN_SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC "-std=c++17" "-Wall")
target_include_directories(${PROJECT_NAME} PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MUNCHKIN_LINK_LIBRARIES})