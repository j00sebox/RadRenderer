cmake_minimum_required(VERSION 3.10)
project(radrenderer)

set(CMAKE_CXX_STANDARD 20)

# Set the source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)

# Collect all .cpp files in the source directory
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0 # Use the latest stable version
)
FetchContent_MakeAvailable(Catch2)

# Add the test source files
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
file(GLOB_RECURSE TESTS "${TEST_DIR}/*.cpp")

add_executable(tests ${TESTS})

# Add the executable target with all the collected sources
add_executable(${PROJECT_NAME} ${SOURCES})
add_subdirectory(mathz)

target_link_libraries(${PROJECT_NAME} PRIVATE mathz sfml-graphics sfml-window sfml-system)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Add the tests to CTest
include(CTest)
include(Catch)
catch_discover_tests(tests)
