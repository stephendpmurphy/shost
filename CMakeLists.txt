# Set min req version of Cmake
cmake_minimum_required(VERSION 3.16)

# required for C99 designator aggregate initializer
set(CMAKE_CXX_STANDARD 20)

# Set the project name
project(shost)

set(LIB_NAME shost)
set(EXE_NAME shost-cli)

# Set our include directories
include_directories(src
                    lib
                    libMPSSE/inc
                    libMPSSE/inc/linux)

# Set the binary output folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

set(LIB_SRC ${PROJECT_SOURCE_DIR}/lib/shost.cpp
        ${PROJECT_SOURCE_DIR}/lib/Protocol.cpp
        ${PROJECT_SOURCE_DIR}/lib/SPI.cpp
        ${PROJECT_SOURCE_DIR}/lib/I2C.cpp
)

set(APP_SRC ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

# Create our executable
add_executable(${EXE_NAME} ${APP_SRC})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
# Find libftdi1
find_package(libftdi1)

# Find libmpsse
find_package(libmpsse)

if ( (${LIBFTDI1_FOUND}) AND (${LIBMPSSE_FOUND}) )
    message(STATUS "Enabling FTDI and MPSSE support")
    set(ENABLE_FTDI 1)
    target_link_libraries(${LIB_NAME} ${LIBFTDI1_LIBRARIES} ${LIBMPSSE_LIBRARIES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIBFTDI1_INCLUDE_DIRS} ${LIBMPSSE_INCLUDE_DIRS})
else()
    message(STATUS "Disabling FTDI code due to missing libftdi1 or libmpsse:")
    message(STATUS "  LIBFTDI1_FOUND = ${LIBFTDI1_FOUND}")
    message(STATUS "  LIBMPSSE_FOUND = ${LIBMPSSE_FOUND}")
endif()


# Link against the MPSSE, dl and ftd2xx libs
target_link_libraries(${EXE_NAME} dl ${LIB_NAME})

set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})

# Install the cli app to our bin folder
install(TARGETS ${EXE_NAME} DESTINATION bin)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
