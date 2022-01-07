#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <system_error>
#include <ftdi.h>
#include "shost.h"
#include "SPI.h"
#include "I2C.h"
#include "Protocol.h"

static void dump_array(uint8_t *arr, int len) {
    for(int i = 0; i < len; i++) {
        printf("%02X ", arr[i]);
    }
    printf("\n");
}

int shost_getConnectedDevices(bool printDevInfo) {
    int devCount = 0;
    int i = 0;
    struct ftdi_context *ftdi;
    struct ftdi_device_list *devlist, *curdev;
    char manufacturer[128], description[128];

    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
        return 0;
    }

    if ((devCount = ftdi_usb_find_all(ftdi, &devlist, 0, 0)) < 0)
    {
        fprintf(stderr, "ftdi_usb_find_all failed: %d (%s)\n", devCount, ftdi_get_error_string(ftdi));
        devCount = 0;
        goto do_deinit;
    }

    if(printDevInfo) {
        printf("Number of FTDI devices found: %d\n", devCount);

        for (curdev = devlist; curdev != NULL; i++) {
            if( ftdi_usb_get_strings(ftdi, curdev->dev, manufacturer, 128, description, 128, NULL, 0) < 0) {
                fprintf(stderr, "ftdi_usb_get_strings failed: #%d (%s)\n", i, ftdi_get_error_string(ftdi));
                devCount = 0;
                goto done;
            }
            printf("\nDevice %d:\nManufacturer: %s\nDescription: %s\n", i, manufacturer, description);
            curdev = curdev->next;
        }
    }

done:
    ftdi_list_free(&devlist);
do_deinit:
    ftdi_free(ftdi);

    return devCount;
}

int shost_xfer_begin(shost_xfer_t xfer) {
    int retVal = 0;
    Protocol *IO;

    // Check if we have any available channels to begin with
    if(!shost_getConnectedDevices(false)) {
        printf("No devices found. Ensure your device is connected and the proper UDEV rules have been added.\n");
        retVal = -1;
        goto CLEANUP;
    }

    // Setup the interface class
    switch (xfer.intf) {
        case XFER_INTF_SPI:
            IO = new SPI();
            break;

        case XFER_INTF_I2C:
            IO = new I2C();
            break;

        default:
            retVal = -1;
            printf("Invalid interface type given.\n");
            goto CLEANUP;
            break;
    }

    if ( (xfer.xferType <= XFER_TYPE_NONE) || (xfer.xferType >= XFER_TYPE__MAX__) ) {
        printf("Invalid transfer type given.\n");
        retVal = -1;
    }
    else {
        printf("Starting a %s using %s on channel %d with a len of %d bytes at %dHz.\n",
                (xfer.xferType == XFER_TYPE_WRITE ? "WRITE" : (xfer.xferType == XFER_TYPE_READ ? "READ" : (xfer.xferType == XFER_TYPE_READ_WRITE ? "READ/WRITE" : "?"))),
                IO->name,
                xfer.channel,
                xfer.len,
                xfer.clk);

        try {
            switch (xfer.xferType) {
                case XFER_TYPE_WRITE:
                    IO->write(&xfer);
                    break;

                case XFER_TYPE_READ:
                    IO->read(&xfer);
                    break;

                case XFER_TYPE_READ_WRITE:
                    // TODO: Implement READ/WRITE functionality for SPI
                    break;

                default:
                    retVal = -1;
                    break;
            }

            printf("%d byte(s) transmitted over %s.\n",
                xfer.bytesTranferred,
                IO->name);
        }
        catch (const std::system_error &e) {
            printf("Failed to %s: %s\n",
                (xfer.xferType == XFER_TYPE_WRITE ? "WRITE" : (xfer.xferType == XFER_TYPE_READ ? "READ" : (xfer.xferType == XFER_TYPE_READ_WRITE ? "READ/WRITE" : "?"))),
                e.what());
            retVal = -1;
        }
        catch (const std::invalid_argument &e) {
            printf("Invalid argument: %s\n", e.what());
            retVal = -1;
        }
    }

CLEANUP:
    // Return
    return retVal;
}