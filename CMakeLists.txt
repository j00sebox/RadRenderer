cmake_minimum_required(VERSION 3.10)
project(radrenderer)

set(CMAKE_CXX_STANDARD 20)

# Set the source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)

# Collect all .cpp files in the source directory
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Download submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Grab submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT
                    )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
        endif()
    endif()
endif()

add_subdirectory(submodules/json)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0 # Use the latest stable version
)
FetchContent_MakeAvailable(Catch2)


FetchContent_Declare(
  CPM
  GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake
  GIT_TAG origin/master
)

FetchContent_MakeAvailable(CPM)

CPMAddPackage(
  NAME SFML
  GITHUB_REPOSITORY SFML/SFML
  GIT_TAG 3.0.0
  OPTIONS
    "SFML_BUILD_EXAMPLES OFF"
    "SFML_BUILD_DOC OFF"
    "SFML_BUILD_TEST_SUITE OFF"
)

# tinyfiledialogs for cross-platform file dialogs
FetchContent_Declare(
  tinyfiledialogs
  GIT_REPOSITORY https://github.com/native-toolkit/tinyfiledialogs.git
  GIT_TAG master
)
FetchContent_MakeAvailable(tinyfiledialogs)

# Add the test source files
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
file(GLOB_RECURSE TESTS "${TEST_DIR}/*.cpp")

add_executable(tests ${TESTS})

# Add the executable target with all the collected sources
add_executable(${PROJECT_NAME} ${SOURCES} ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c)
add_subdirectory(mathz)

# Skip precompiled headers for C file
set_source_files_properties(${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

target_precompile_headers(${PROJECT_NAME} PUBLIC main/pch.h)

target_include_directories(${PROJECT_NAME} PUBLIC submodules/json/include submodules/stb ${tinyfiledialogs_SOURCE_DIR})

find_package(SFML 3.0 COMPONENTS graphics window system REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE mathz sfml-graphics sfml-window sfml-system)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Add the tests to CTest
include(CTest)
include(Catch)
catch_discover_tests(tests)
