# Set min req version of Cmake
cmake_minimum_required(VERSION 3.16)

# Set the project name
project(mpsse-cli)

# Include the libMPSSE sub-directory
add_subdirectory(libMPSSE)

# Set our include directories
include_directories(inc
                    libMPSSE/inc
                    libMPSSE/inc/linux)

# Set the binary output folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

set(APP_SRC ${PROJECT_SOURCE_DIR}/src/mpsse-cli.c
            ${PROJECT_SOURCE_DIR}/src/spi.c
            ${PROJECT_SOURCE_DIR}/src/i2c.c)

# Create our executable
add_executable(mpsse-cli ${APP_SRC})


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
# Find libftdi1
find_package(libftdi1)

# Find libmpsse
find_package(libmpsse)

if ( (${LIBFTDI1_FOUND}) AND (${LIBMPSSE_FOUND}) )
    message(STATUS "Enabling FTDI and MPSSE support")
    set(ENABLE_FTDI 1)
    target_link_libraries(mpsse-cli ${LIBFTDI1_LIBRARIES} ${LIBMPSSE_LIBRARIES})
    target_include_directories(mpsse-cli PUBLIC ${LIBFTDI1_INCLUDE_DIRS} ${LIBMPSSE_INCLUDE_DIRS})
else()
    message(STATUS "Disabling FTDI code due to missing libftdi1 or libmpsse:")
    message(STATUS "  LIBFTDI1_FOUND = ${LIBFTDI1_FOUND}")
    message(STATUS "  LIBMPSSE_FOUND = ${LIBMPSSE_FOUND}")
endif()


# Link against the MPSSE, dl and ftd2xx libs
target_link_libraries(mpsse-cli MPSSE dl ftd2xx)

# Install the cli app to our bin folder
install(TARGETS mpsse-cli DESTINATION bin)