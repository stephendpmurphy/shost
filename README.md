<p align="center">
  <h1 align="center">MPSSE-CLI</h1>
  <p align="center">
    FTDI MPSSE CLI application for SPI, I2C and GPIO control
  </p>
</p>

## Getting started
To get started with the libMPSSE library you will need a couple pieces of software and a piece of hardware.
-   [CMake](https://cmake.org/download/)
-   [FTDI D2xx driver](https://ftdichip.com/drivers/d2xx-drivers/)
-   [MPSSE capable FTDI device](https://ftdichip.com/product-category/products/)

## Retrieving the source
To begin, you will need to clone this project to your dev machine
```bash
$ git clone https://github.com/stephendpmurphy/mpsse-cli.git
$ cd mpsse-cli
$ git submodule update --init --recursive
```

## Building the CLI application
Once you have installed the required tools mentioned above and retrieved the source you can build the CLI application
```bash
$ cd mpsse-cli
$ mkdir -p build
# Debug build
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
# OR
# Release build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j8
```

## Installing the CLI application
Once you have built the application, you can install to your **bin** folder.
```bash
$ cd build
$ sudo make install
```