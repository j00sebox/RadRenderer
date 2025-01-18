cmake_minimum_required(VERSION 3.10)
project(radrenderer)

set(CMAKE_CXX_STANDARD 20)

# Set the source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)

# Collect all .cpp files in the source directory
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Add the executable target with all the collected sources
add_executable(${PROJECT_NAME} ${SOURCES})
add_subdirectory(mathz)

target_link_libraries(${PROJECT_NAME} PRIVATE mathz sfml-graphics sfml-window sfml-system)
