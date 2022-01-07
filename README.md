<p align="center">
  <h1 align="center">shost</h1>
  <p align="center">
    FTDI MPSSE CLI application & static library for SPI, I2C and GPIO control
  </p>
</p>

### Retrieving the source
To begin, you will need to clone this project to your dev machine
```bash
$ git clone https://github.com/stephendpmurphy/mpsse-cli.git
$ cd mpsse-cli
$ git submodule update --init --recursive
```

### Setup & Installation
The shost command line utility setup process assumes you are running the Linux distro Ubuntu. It can also be executed from an Ubuntu instance in Windows [WSL2](https://docs.microsoft.com/en-us/windows/wsl/install) to which the FTDI hardware can be bridged using the Microsoft [write-up](https://devblogs.microsoft.com/commandline/connecting-usb-devices-to-wsl/). When setting up WSL and the USB bridge you will need to ensure you are running Windows 11 and the latest vesions of WSL2 as specified in the write-up.

To begin building the ```shost``` CLI and static library, check out the [INSTALLATION](./doc/INSTALL.md) guide for more info.

### Usage
// TODO