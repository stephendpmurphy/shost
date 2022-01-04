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

uint8 I2C::i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size) {
    size_t write_size = 2 + data_size; // address + reg + datasize
    char *writebuffer = (char *) malloc(write_size);
    uint8 ret = 0;
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
uint8 I2C::i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size) {

    uint8 ret = 2;
    const static size_t write_size = 2; // address + reg
    char write_buffer[write_size];
    uint8 *read_buffer = (uint8 *) malloc(buffer_size);
    write_buffer[0] = ((address << 1) | 0b00000000); // write mode
    write_buffer[1] = reg;
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, write_buffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK) {
                if (Start(mpsse) == MPSSE_OK) {
                    char w = ((address << 1) | 0b00000001); // read mode
                    if (Write(mpsse, &w, 1) == MPSSE_OK) {
                        if (GetAck(mpsse) == ACK) {
                            read_buffer = (uint8 *)Read(mpsse, buffer_size);
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

void I2C::_write(xfer_t *xfer) {

    int ret = 0;
    // TODO make freq adjustable
    printf("Frequency set to 100Khz.\n");
    //    open mpsse device
    this->mpsse = MPSSE(modes::I2C, ONE_HUNDRED_KHZ, MSB);

    if (this->mpsse != NULL && !this->mpsse->open) {
        // TODO what should be something like: "Failed to initialize MPSSE: %s\n", ErrorString(this->mpsse)
        throw std::system_error(EIO, std::generic_category(), ErrorString(this->mpsse));
    }

    ret = i2c_write(xfer->address, xfer->_register, xfer->buff, xfer->len);

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
    xfer->bytesTranferred = xfer->len;

}

void I2C::_read(xfer_t *xfer) {
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
    ret = i2c_read(xfer->address, xfer->_register, xfer->buff, xfer->len);

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

I2C::I2C(): Protocol("I²C") {

}
