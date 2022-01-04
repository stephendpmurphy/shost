#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <system_error>
#include "mpsse-cli.h"
#include "I2C.h"
#include "cli.h"
#include "SPI.h"

static int checkIfFtdiModuleLoaded(void) {
    FILE *fp;
    char modRes[128] = {0x00};

    // Run the lsmod command and check if the FTDI module is loaded
    fp = popen("lsmod | grep ftdi_sio", "r");

    // Make sure the file pointer isn't null
    if (fp == NULL) {
        printf("fp is null\n");
        return -1;
    }

    // Read out any resulst from running the lsmod command
    fgets(modRes, sizeof(modRes), fp);

    // Check if the result has any characters
    if (strlen(modRes) == 0) {
        // No characters, assume the module isn't loaded then.
        return 0;
    } else {
        // Some characters were found. The module must be loaded.
        return 1;
    }
}

#define delete_module(name, flags) syscall(__NR_delete_module, name, flags)

int removeFtdiModule(void) {
    if (delete_module("ftdi_sio", O_NONBLOCK) != 0) {
        perror("delete_module");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static uint32 getMPSSEchannelCount(void) {
    FT_STATUS ftStatus;
    uint32 numDevs;
    uint32 channels = 0;
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of FTDI devices connected: %d\n", numDevs);
    // retrieve the number of devices that are capable of MPSSE
    ftStatus = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of available MPSSE channels: %d\n", (int) channels);

    return channels;
}

static void printMPSSEchannelInfo(int channels) {
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    // Iterate through the channels and print their info
    if (channels > 0) {
        for (uint32 i = 0; i < channels; i++) {
            ftStatus = SPI_GetChannelInfo(i, &devList);
            APP_CHECK_STATUS(ftStatus);
            printf("Information on channel number %d:\n", i);
            /* print the dev info */
            printf("    Flags=0x%x\n", devList.Flags);
            printf("    Type=0x%x\n", devList.Type);
            printf("    ID=0x%x\n", devList.ID);
            printf("    LocId=0x%x\n", devList.LocId);
            printf("    SerialNumber=%s\n", devList.SerialNumber);
            printf("    Description=%s\n", devList.Description);
            // broken in cpp 20, but since we will be moving away from the properitary lib I will not be fixing it.
//            printf("    ftHandle=0x%x\n", (unsigned int) devList.ftHandle); /*is 0 unless open*/
        }
    }
}

int CB_printCliVersion(arg_t *arg) {
    printf("0.1.0\n");
    return 0;
}

int CB_printFTDIdevices(arg_t *arg) {
    printMPSSEchannelInfo(getMPSSEchannelCount());
    return 0;
}

int main(int argc, char *argv[]) {
    int retVal = 0;
    bool useProtocol = false;
    Protocol *IO;
    arg_t arg = {
            .clk = 100000,
            .channel = 0,
            .len = 1,
    };

    // prase the cli to a usable object
    if (parsecli(argc, argv, &arg) < 0) {
        printf("Please provide at least one option and value.\n\n");
        // print help screen to help the user
        arg.type = ARG_HELP;
    }

    // call correct function for subcommand or option
    switch (arg.type) {
        case ARG_SPI:
            useProtocol = true;
            // Check if the FTDI serial module is loaded. If so, remove it.
            // This requires sudo when running after a build. Not required when installed.
            if (checkIfFtdiModuleLoaded() > 0) {
                removeFtdiModule();
            }

            /* init library */
            Init_libMPSSE();
            IO = new SPI();
            break;
        case ARG_I2C:
            useProtocol = true;
            IO = new I2C();
            break;
        case ARG_HELP:
            retVal = CB_printCLIoptions(&arg);
            break;
        case ARG_VERSION:
            retVal = CB_printCliVersion(&arg);
            break;
        case ARG_LIST:
            retVal = CB_printFTDIdevices(&arg);
            break;
        case ARG_NONE:
        default:
            // We made it this far.. Assume no valid argument was given.
            printf("Invalid arguments given.\n");
            retVal = -1;
    }
    if (useProtocol) {

        if (arg.xferType == XFER_WRITE) {
            try {
                printf("Starting a WRITE using %s on channel %d with a len of %d bytes at %dHz.\n", IO->name,
                       arg.channel, arg.len, arg.clk);
                IO->write(&arg);
                printf("%d byte(s) written over %s.\n", arg.bytesTranferred, IO->name);
            }
            catch (const std::system_error &e) {
                printf("Failed to write: %s\n", e.what());
                retVal = -1;
            }
            catch (const std::invalid_argument &e) {
                printf("Invalid argument: %s\n", e.what());
                retVal = -1;
            }
        } else if (arg.xferType == XFER_READ) {
            try {
                printf("Starting a READ using %s on channel %d with a len of %d bytes at %dHz.\n", IO->name,
                       arg.channel, arg.len, arg.clk);
                IO->read(&arg);
                printf("%d byte(s) read over %s.\n", arg.bytesTranferred, IO->name);
                printfArray(arg.buff, arg.bytesTranferred);
            }
            catch (const std::system_error &e) {
                printf("Failed to read: %s\n", e.what());
                retVal = -1;
            }
            catch (const std::invalid_argument &e) {
                printf("Invalid argument: %s\n", e.what());
                retVal = -1;
            }

        } else {
            printf("No transfer type given (-x).\n");
            retVal = -1;
        }

    }
    if (arg.type == ARG_SPI)
        Cleanup_libMPSSE();

    return retVal;
}

static void printfArray(uint8 *buff, uint16 len) {
    printf("Array dump: ");
    for (int i = 0; i < len; i++) {
        printf("0x%02X ", buff[i]);
    }

    printf("\n");
}