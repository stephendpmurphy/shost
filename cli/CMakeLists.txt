# Set min req version of Cmake
cmake_minimum_required(VERSION 3.16)
# required for C99 designator aggregate initializer
set(CMAKE_CXX_STANDARD 20)
set(CLI_SRC ${CLI_SRC_DIR}/cli.cpp)

# Set our include directories to find the lib headers
include_directories(${STATIC_LIB_SHOST_SRC_DIR})

# Create our executable
add_executable(${CLI_NAME} ${CLI_SRC})

# Link against the shost libs
target_link_libraries(${CLI_NAME} dl ${STATIC_LIB_NAME})