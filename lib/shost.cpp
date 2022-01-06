#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <system_error>
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

int shost_xfer_begin(shost_xfer_t xfer) {
    int retVal = 0;
    Protocol *IO;

    // Check if we have any available channels to begin with

    // Setup the interface class
    switch (xfer.intf) {
        case XFER_INTF_SPI:
            IO = new SPI();
            break;

        case XFER_INTF_I2C:
            IO = new I2C();
            break;

        default:
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

    // Return
    return retVal;
}