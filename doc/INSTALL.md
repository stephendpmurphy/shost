# Installation Guide
To begin with using the cli ```shost``` binary or developing you're own app using the ```shost``` static library, we will need to install several dependencies and install new udev rules allowing our application to communicate with connected FTDI hardware. If you are setting ```shost``` up in an Ubuntu instance in Windows [WSL2](https://docs.microsoft.com/en-us/windows/wsl/install), you will also need to follow along with this [write-up](https://devblogs.microsoft.com/commandline/connecting-usb-devices-to-wsl/) which walks through adding the ability to bridge USB devices between your Windows OS and the WSL2 instance. When setting up WSL and the USB bridge you will need to ensure you are running Windows 11 and the latest vesions of WSL2 as specified in the write-up.

Now that your OS is setup, either in Native Linux or WSL2, lets begin.

## System Setup
To add the new udev rules, execute the following from the repos root folder:
```md
# Copy over our rules to the udev/rules.d folder
sudo cp ./doc/99-ftdi.rules /etc/udev/rules.d

# If running in WSL2 and it's your first time adding udev rules - You'll need to run the following
sudo service udev restart

# Trigger a reload of our rules
sudo udevadm control --reload-rules && udevadm trigger
```
Next we should update our package manager registry.
```console
sudo apt-get update
```
Once our packages have been updated we can install our needed packages
```console
sudo apt-get install build-essential pkg-config libusb-1.0-0-dev cmake
```

## Building the shost software
Now that we have finished the system setup. We can work on building the software. Ensure you have cloned the submodules provided in this repository
```console
git submodule update --init --recursive
```
Now that we have all of the latest source pulled we can now build and install the ```shost``` CLI and static library. From the repo root directory:
```console
mkdir build && cd build
cmake ..
make -j8
sudo make install
```
Congratulations 🎉 The freshly built ```shost``` cli and ```libshost.a``` static library can be found in the ```output/``` folder.