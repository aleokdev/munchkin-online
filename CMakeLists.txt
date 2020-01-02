cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "bin/")
endforeach()

set(BUILD_SHARED_LIBS OFF CACHE STRING "" FORCE)

project("munchkin-online" VERSION 0.0.0
						  DESCRIPTION "Collab project that tries to bring the fun of table Munchkin to a worldwide experience")

set(MUNCHKIN_SOURCES "src/main.cpp" "src/game/aigame.cpp")
set(MUNCHKIN_INCLUDE_DIRECTORIES "include")
set(MUNCHKIN_LINK_LIBRARIES "")
set(MUNCHKIN_LINK_DIRECTORIES "")

add_subdirectory("lib")

add_executable (${PROJECT_NAME} ${MUNCHKIN_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC "cxx_std_17")
target_include_directories(${PROJECT_NAME} PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MUNCHKIN_LINK_LIBRARIES})