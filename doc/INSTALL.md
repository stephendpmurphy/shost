# Installation Guide
To begin with using the cli ```shost``` binary or developing you're own app using ```shost```, we will need to install several dependencies. First we should update our package manager registry.
```console
sudo apt-get update
```
Once our packages have been updated we can install our needed packages
```console
sudo apt-get install git build-essential libusb-1.0-0-dev swig cmake python-dev libconfuse-dev libboost-all-dev libftdi autoconf automake libtool
```
With our packages installed we can then clone and install the ```libftdi1``` library. This is an open-source alternative for the low-level USB comms with our FTDI serial bridge device.
```console
sudo git clone git://developer.intra2net.com/libftdi ~/libftdi1 && cd ~/libftdi1 && mkdir -p build \
&& cd build && cmake -DCMAKE_INSTALL_PREFIX="/usr/" ../ && make -j8 && make install
```
Next we will install the open-source MPSSE lib used to send I2C, SPI and GPIO data through the FTDI serial bridge.
```console
sudo git clone https://github.com/l29ah/libmpsse.git ~/libmpsse && cd ~/libmpsse/src \
&& autoreconf --install && ./configure && make -j8 && make install
```
To build and install the ```shost``` cli, we do the following
```console
sudo git clone https://github.com/stephendpmurphy/shost ~/shost && cd ~/shost \
&& mkdir -p build && cd build && cmake .. && make -j8 && make install
```