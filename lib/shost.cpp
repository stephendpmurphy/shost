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

/**
 * @brief Dump the contents of a provided array
*/
static void dump_array(uint8_t *arr, int len) {
    for(int i = 0; i < len; i++) {
        printf("%02X ", arr[i]);
    }
    printf("\n");
}

/**
 * @brief Get all of the compatible connected devices
*/
int shost_getConnectedDevices(shost_connected_devices_t *connectedDevices) {
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

    // Find how many FTDI devices are connected
    if ((devCount = ftdi_usb_find_all(ftdi, &devlist, 0, 0)) < 0)
    {
        fprintf(stderr, "ftdi_usb_find_all failed: %d (%s)\n", devCount, ftdi_get_error_string(ftdi));
        devCount = 0;
        goto do_deinit;
    }

    if( connectedDevices != NULL ) {
        // Store the string information for all the devices
        for (curdev = devlist; curdev != NULL; i++) {
            if( ftdi_usb_get_strings(ftdi, curdev->dev,
                connectedDevices->info[i].manufacturer, sizeof(connectedDevices->info[i].manufacturer),
                connectedDevices->info[i].description, sizeof(connectedDevices->info[i].description), NULL, 0) < 0) {
                fprintf(stderr, "ftdi_usb_get_strings failed: #%d (%s)\n", i, ftdi_get_error_string(ftdi));
                devCount = 0;
                goto done;
            }
            curdev = curdev->next;
        }
    }
    else {
        goto do_deinit;
    }

done:
    ftdi_list_free(&devlist);
do_deinit:
    ftdi_free(ftdi);

    return devCount;
}

/**
 * @brief Begin a transfer using shost
*/
int shost_xfer_begin(shost_xfer_t *xfer) {
    int retVal = 0;
    Protocol *IO;

    // Check if we have any available channels to begin with
    if(!shost_getConnectedDevices(NULL)) {
        printf("No devices found. Ensure your device is connected and the proper UDEV rules have been added.\n");
        retVal = -1;
        goto CLEANUP;
    }

    // Setup the interface class
    switch (xfer->intf) {
        case XFER_INTF_SPI:
            IO = new SPI();
            break;

        case XFER_INTF_I2C:
            IO = new I2C();
            break;

        default:
            retVal = -1;
            printf("Invalid transport layer type given.\n");
            goto CLEANUP;
            break;
    }

    if ( (xfer->xferType <= XFER_TYPE_NONE) || (xfer->xferType >= XFER_TYPE__MAX__) ) {
        printf("Invalid transfer type given.\n");
        retVal = -1;
        goto CLEANUP;
    }
    else {
        printf("Starting a %s using %s on channel %s with a len of %d bytes at %dHz.\n",
                (xfer->xferType == XFER_TYPE_WRITE ? "WRITE" : (xfer->xferType == XFER_TYPE_READ ? "READ" : (xfer->xferType == XFER_TYPE_READ_WRITE ? "READ/WRITE" : "?"))),
                IO->name,
                (xfer->channel == XFER_CH_A ? "A" : (xfer->channel == XFER_CH_B ? "B" : (xfer->channel == XFER_CH_C ? "C" : (xfer->channel == XFER_CH_D ? "D" : "?")))),
                (xfer->xferType == XFER_TYPE_WRITE ? xfer->tx_len: (xfer->xferType == XFER_TYPE_READ ? xfer->rx_len : (xfer->xferType == XFER_TYPE_READ_WRITE ? xfer->tx_len : 0))),
                xfer->clk);

        try {
            switch (xfer->xferType) {
                case XFER_TYPE_WRITE:
                    // As a cleanup - Since we are writing - Zero out the rx length
                    xfer->rx_len = 0;
                    IO->write(xfer);
                    break;

                case XFER_TYPE_READ:
                    // As a cleanup - Since we are reading - Zero out the tx length
                    xfer->tx_len = 0;
                    IO->read(xfer);
                    break;

                case XFER_TYPE_READ_WRITE:
                    // TODO: Implement READ/WRITE functionality for SPI
                    break;

                default:
                    retVal = -1;
                    break;
            }

            if( retVal >= 0) {
                printf("%d byte(s) transmitted over %s.\n",
                    xfer->bytesTranferred,
                    IO->name);
            }
        }
        catch (const std::system_error &e) {
            printf("Failed to %s: %s\n",
                (xfer->xferType == XFER_TYPE_WRITE ? "WRITE" : (xfer->xferType == XFER_TYPE_READ ? "READ" : (xfer->xferType == XFER_TYPE_READ_WRITE ? "READ/WRITE" : "?"))),
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