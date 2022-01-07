//
// Created by noeel on 05-10-21.
//
//#include "libMPSSE_i2c.h"
#include "I2C.h"
#include <mpsse.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <system_error>
#include <cassert>

I2C::I2C(): Protocol("I²C") {

}

void I2C::_write(shost_xfer_t *xfer) {

    int ret = 0;
    //    open mpsse device
    this->mpsse = MPSSE(modes::I2C, xfer->clk, MSB);

    if (this->mpsse != NULL && !this->mpsse->open) {
        // TODO what should be something like: "Failed to initialize MPSSE: %s\n", ErrorString(this->mpsse)
        throw std::system_error(EIO, std::generic_category(), ErrorString(this->mpsse));
    }

    ret = i2c_write(xfer->address, xfer->_register, xfer->tx_buff, xfer->tx_len);

    Close(this->mpsse);

    switch (ret) {
        case 1:
            // TODO printing address whould be nice
            throw std::system_error(ENXIO, std::generic_category(), "No response from slave.");
            break;
        case 2:
            throw std::system_error(EIO, std::generic_category(), "Hardware failure.");
            break;
        default:
            // all is good, no need to do anything.
            break;
    }
    xfer->bytesTranferred = xfer->tx_len;
}

void I2C::_read(shost_xfer_t *xfer) {
    int ret = 0;
    // TODO make freq adjustable
    printf("Frequency set to 100Khz.\n");
    //    open mpsse device
    this->mpsse = MPSSE(modes::I2C, ONE_HUNDRED_KHZ, MSB);

    if (this->mpsse != NULL && !this->mpsse->open) {
        // TODO what should be something like: "Failed to initialize MPSSE: %s\n", ErrorString(this->mpsse)
        throw std::system_error(EIO, std::generic_category(), ErrorString(this->mpsse));
    }

//    TODO function is (almost) 1:1 with _write, not so DRY!
    ret = i2c_read(xfer->address, xfer->_register, xfer->rx_buff, xfer->rx_len);

    Close(this->mpsse);

    switch (ret) {
        case 1:
            // TODO printing address whould be nice
            throw std::system_error(ENXIO, std::generic_category(), "No response from slave.");
            break;
        case 2:
            throw std::system_error(EIO, std::generic_category(), "Hardware failure.");
            break;
        default:
            // all is good, no need to do anything.
            break;
    }

}

uint8_t I2C::i2c_write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size) {
    size_t write_size = 2 + data_size; // address + reg + datasize
    char *writebuffer = (char *) malloc(write_size);
    uint8_t ret = 0;
    writebuffer[0] = ((address << 1) | 0b00000000);
    writebuffer[1] = reg;
    for (int i = 0; i < data_size; ++i) {
        writebuffer[i + 2] = data[i];
    }
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, writebuffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK)
                ret = 0;
            else
                ret = 1;
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = 2;
    free(writebuffer);
    return ret;
}


/**
 * Read data from I2C device
 * @param address address of slave
 * @param reg register to read
 * @param buffer store read data here
 * @param buffer_size read size
 * @return 0 if success, 1 if no response and 2 if hardware failed.
 */
uint8_t I2C::i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size) {

    uint8_t ret = 2;
    const static size_t write_size = 2; // address + reg
    char write_buffer[write_size];
    uint8_t *read_buffer = (uint8_t *) malloc(buffer_size);
    write_buffer[0] = ((address << 1) | 0b00000000); // write mode
    write_buffer[1] = reg;
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, write_buffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK) {
                if (Start(mpsse) == MPSSE_OK) {
                    char w = ((address << 1) | 0b00000001); // read mode
                    if (Write(mpsse, &w, 1) == MPSSE_OK) {
                        if (GetAck(mpsse) == ACK) {
                            read_buffer = (uint8_t *)Read(mpsse, buffer_size);
                            memcpy(buffer, read_buffer, buffer_size);
                            SendNacks(mpsse);
                            ret = 0;
                        } else {
                            ret = 1;
                        }
                    }
                }
            } else {
                ret = 1;
            }
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = 2;
    free(read_buffer);
    return ret;
}