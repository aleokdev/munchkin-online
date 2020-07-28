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

set(MUNCHKIN_INCLUDE_DIRECTORIES "include" "lib/glm")
set(MUNCHKIN_LINK_LIBRARIES "")
set(MUNCHKIN_LINK_DIRECTORIES "")
set(MUNCHKIN_SOURCES "")
set(MUNCHKIN_VERSION 0.0.0)

project("munchkin-online" VERSION ${MUNCHKIN_VERSION}
        DESCRIPTION "Collab project that tries to bring the fun of table Munchkin to a worldwide experience")

add_subdirectory(src)

add_executable(munchkin-online ${MUNCHKIN_SOURCES})

add_subdirectory(lib)

set_property(TARGET munchkin-online PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

target_compile_features(munchkin-online PUBLIC "cxx_std_17")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(munchkin-online PUBLIC "-Wall" "-Wno-deprecated-declarations" "-Wno-unused-private-field"
            "-Wno-reorder" "-Wno-unknown-attributes")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

endif ()

target_include_directories(munchkin-online PUBLIC ${MUNCHKIN_INCLUDE_DIRECTORIES})
target_link_libraries(munchkin-online PUBLIC ${MUNCHKIN_LINK_LIBRARIES})
target_compile_definitions(munchkin-online PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD MUNCHKIN_VERSION=${MUNCHKIN_VERSION}, SOL_ALL_SAFETIES_ON=1)

# Data
if (NOT (${PROJECT_BINARY_DIR} STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/build/"))
    message("Copying data files to ${PROJECT_BINARY_DIR}...")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/build/data" DESTINATION "${PROJECT_BINARY_DIR}")
    message("Done!")
endif ()

# Tests

if (BUILD_CARDDATA_TEST)
    add_executable("carddata-test" "tests/carddata.cpp" ${MUNCHKIN_SOURCES})
    target_compile_features("carddata-test" PUBLIC "cxx_std_17")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options("carddata-test" PUBLIC "-w")
    endif ()
    add_subdirectory(lib)
endif ()