#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <system_error>
#include "xfer.h"
#include "SPI.h"
#include "I2C.h"
#include "Protocol.h"
#include "util.h"

int xfer_begin(xfer_t xfer) {
    int retVal = 0;
    Protocol *IO;

    // Check if we have any available channels to begin with

    // If the interface is SPI, do the libMPSSE init
    if( xfer.intf = XFER_INTF_SPI ) {
        // Check if the FTDI serial module is loaded. If so, remove it.
        // This requires sudo when running after a build. Not required when installed.
        if (util_isFtdiModuleLoaded() > 0) {
            util_removeFtdiModule();
        }

        /* init library */
        Init_libMPSSE();
    }

    // Do the transfer
    if (xfer.xferType == XFER_WRITE) {
        try {
            printf("Starting a WRITE using %s on channel %d with a len of %d bytes at %dHz.\n",
                IO->name,
                xfer.channel,
                xfer.len,
                xfer.clk);
            IO->write(&xfer);
            printf("%d byte(s) written over %s.\n",
                xfer.bytesTranferred,
                IO->name);
        }
        catch (const std::system_error &e) {
            printf("Failed to write: %s\n", e.what());
            retVal = -1;
        }
        catch (const std::invalid_argument &e) {
            printf("Invalid xferument: %s\n", e.what());
            retVal = -1;
        }
    } else if (xfer.xferType == XFER_READ) {
        try {
            printf("Starting a READ using %s on channel %d with a len of %d bytes at %dHz.\n",
                IO->name,
                xfer.channel,
                xfer.len,
                xfer.clk);
            IO->read(&xfer);
            printf("%d byte(s) read over %s.\n", xfer.bytesTranferred, IO->name);
            printfArray(xfer.buff, xfer.bytesTranferred);
        }
        catch (const std::system_error &e) {
            printf("Failed to read: %s\n", e.what());
            retVal = -1;
        }
        catch (const std::invalid_argument &e) {
            printf("Invalid xferument: %s\n", e.what());
            retVal = -1;
        }

    } else {
        printf("No transfer type given (-x).\n");
        retVal = -1;
    }

    // Cleanup
    if( xfer.intf = XFER_INTF_SPI ) {
        Cleanup_libMPSSE();
    }

    // Return
    return retVal;
}