# Set min req version of Cmake
cmake_minimum_required(VERSION 3.16)

# required for C99 designator aggregate initializer
set(CMAKE_CXX_STANDARD 20)

# Set the project name
project(shost)

# Set the output binary names
set(STATIC_LIB_NAME ${CMAKE_PROJECT_NAME})
set(CLI_NAME "${CMAKE_PROJECT_NAME}-cli")

# Set our project paths
set(LIB_MPSSE_SRC_DIR ${CMAKE_SOURCE_DIR}/vendor/libmpsse/src)
set(STATIC_LIB_FTDI_SRC_DIR ${CMAKE_SOURCE_DIR}/vendor/libftdi/src)
set(STATIC_LIB_SHOST_SRC_DIR ${CMAKE_SOURCE_DIR}/lib)
set(CLI_SRC_DIR ${CMAKE_SOURCE_DIR}/cli)

# Set the binary output folder
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Turn off building libftdi examples
option ( SHAREDLIBS "Build libftdi shared libs" OFF )
option ( EXAMPLES "Build example programs" OFF )
option ( FTDI_EEPROM "Build ftdi_eeprom" OFF )
add_subdirectory(vendor/libftdi)
add_subdirectory(lib)
add_subdirectory(cli)

# Set the output name
set_target_properties(${CLI_NAME} PROPERTIES OUTPUT_NAME ${STATIC_LIB_NAME})

# Install the cli app to our bin folder
install(TARGETS ${CLI_NAME} RUNTIME DESTINATION bin)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
