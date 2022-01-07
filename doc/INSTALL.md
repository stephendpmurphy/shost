# Installation Guide
To begin with using the cli ```shost``` binary or developing you're own app using ```shost```, we will need to install several dependencies and install new udev rules allowing our application to communicate with connected FTDI hardware.

To add the new udev rules, execute the following from the repos root folder:
```console
sudo cp ./doc/99-ftdi.rules /etc/udev/rules.d
sudo udevadm control --reload-rules && udevadm trigger
```
Once our udev rules are installed we can work on the software installation. Ensure you have cloned the submodules provided in this repository
```console
git submodule update --init --recursive
```
Next we should update our package manager registry.
```console
sudo apt-get update
```
Once our packages have been updated we can install our needed packages
```console
sudo apt-get install build-essential pkg-config libusb-1.0-0-dev swig cmake python-dev libconfuse-dev libboost-all-dev libftdi-dev autoconf automake libtool
```
With our packages installed we can now build and install the ```libftdi1``` library. This is an open-source alternative for the low-level USB comms with our FTDI serial bridge device. From the repo root directory:
```console
cd vendor/libftdi1
mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX="/usr/" ../
make -j8
sudo make install
```
Next we will build and install the open-source ```libmpsse``` used to send I2C, SPI and GPIO data through the FTDI serial bridge. From the repo root directory:
```console
cd vendor/libmpsse/src
autoreconf --install
./configure
make -j8
sudo make install
```
Finally we can build and install the ```shost``` cli. From the repo root directory:
```console
mkdir build && cd build
cmake ..
make -j8
sudo make install
```
