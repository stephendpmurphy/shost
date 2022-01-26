# Set min req version of Cmake
cmake_minimum_required(VERSION 3.16)
# required for C99 designator aggregate initializer
set(CMAKE_CXX_STANDARD 20)
set(LIB_SRC ${STATIC_LIB_SHOST_SRC_DIR}/shost.cpp
            ${STATIC_LIB_SHOST_SRC_DIR}/Protocol.cpp
            ${STATIC_LIB_SHOST_SRC_DIR}/SPI.cpp
            ${STATIC_LIB_SHOST_SRC_DIR}/I2C.cpp
            ${LIB_MPSSE_SRC_DIR}/fast.c
            ${LIB_MPSSE_SRC_DIR}/mpsse.c
            ${LIB_MPSSE_SRC_DIR}/support.c
            ${LIB_MPSSE_SRC_DIR}/mpsse.h
            ${LIB_MPSSE_SRC_DIR}/support.h
)

# Create our static library
add_library(${STATIC_LIB_NAME} ${LIB_SRC})

target_include_directories(${STATIC_LIB_NAME} PUBLIC ${STATIC_LIB_FTDI_SRC_DIR})
target_link_libraries(${STATIC_LIB_NAME} ftdi1-static)