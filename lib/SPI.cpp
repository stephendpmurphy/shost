#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <system_error>
#include "shost.h"
#include <mpsse.h>
// #include "libMPSSE_spi.h"
#include "SPI.h"

SPI::SPI(): Protocol("SPI") {

}

void SPI::_write(shost_xfer_t *xfer) {
    int ret = 0;
    // Open an MPSSE instance for SPI0 and store the context
    this->mpsse = MPSSE(modes::SPI0, xfer->clk, MSB);

    // Ensure the context is not NULL and and the mpsse channel is open
    if (this->mpsse != NULL && !this->mpsse->open) {
        // TODO what should be something like: "Failed to initialize MPSSE: %s\n", ErrorString(this->mpsse)
        throw std::system_error(EIO, std::generic_category(), ErrorString(this->mpsse));
    }

    ret = spi_write(xfer->buff, xfer->len);

    Close(this->mpsse);

    switch (ret) {
        case 1:
            // TODO printing address whould be nice
            throw std::system_error(ENXIO, std::generic_category(), "No response from peripheral.");
            break;
        case 2:
            throw std::system_error(EIO, std::generic_category(), "Hardware failure.");
            break;
        default:
            // all is good, no need to do anything.
            break;
    }
    xfer->bytesTranferred = xfer->len;
}

void SPI::_read(shost_xfer_t *xfer) {
    int ret = 0;
    // Open an MPSSE instance for SPI0 and store the context
    this->mpsse = MPSSE(modes::SPI0, xfer->clk, MSB);

    // Ensure the context is not NULL and and the mpsse channel is open
    if (this->mpsse != NULL && !this->mpsse->open) {
        // TODO what should be something like: "Failed to initialize MPSSE: %s\n", ErrorString(this->mpsse)
        throw std::system_error(EIO, std::generic_category(), ErrorString(this->mpsse));
    }

    ret = spi_read(xfer->buff, xfer->len);

    Close(this->mpsse);

    switch (ret) {
        case 1:
            // TODO printing address whould be nice
            throw std::system_error(ENXIO, std::generic_category(), "No response from peripheral.");
            break;
        case 2:
            throw std::system_error(EIO, std::generic_category(), "Hardware failure.");
            break;
        default:
            // all is good, no need to do anything.
            break;
    }
    xfer->bytesTranferred = xfer->len;

}

int8_t SPI::spi_write(uint8_t *src_buffer, size_t buffer_len) {
    char *writeBuffer = (char *) malloc(buffer_len);
    memcpy((void*)writeBuffer, (const void*)src_buffer, buffer_len);
    // Send the start condition for SPI
    Start(mpsse);
    // Write our buffer
    Write(mpsse, writeBuffer, buffer_len);
    // Send the stop condition for SPI
    Stop(mpsse);

    return 0;
}

int8_t SPI::spi_read(uint8_t *dest_buffer, size_t buffer_len) {
    // Create a read buffer to store the read result in
    char *readBuffer = (char *) malloc(buffer_len+1);
    // Send the start condition for SPI
    Start(mpsse);
    // Write our buffer
    readBuffer = Read(mpsse, buffer_len);
    // Send the stop condition for SPI
    Stop(mpsse);
    // Copy over the resulting data before it's destroyed
    memcpy((void *)dest_buffer, (const void*)readBuffer, buffer_len);
    // Free the read buffer we created
    free(readBuffer);

    return 0;
}
