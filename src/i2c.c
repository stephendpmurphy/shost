//
// Created by noeel on 05-10-21.
//
//#include "libMPSSE_i2c.h"
#include "i2c.h"
#include <mpsse.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include "cli.h"

const static size_t ACK_TIME = 0;
struct mpsse_context* mpsse;

int i2c_processCmd(arg_t *arg) {
    //    open mpsse device
    // TODO make freq adjustable
    mpsse = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB);
    if (mpsse != NULL && !mpsse->open) {
        printf("Failed to initialize MPSSE: %s\n", ErrorString(mpsse));
        return -1;
    }
    int ret = 0;

    if (arg->xferType == XFER_WRITE)
        ret = i2c_write(arg->address, arg->_register, arg->buff, arg->len);
    else if (arg->xferType == XFER_READ)
        ret = i2c_read(arg->address, arg->_register, arg->buff, arg->len);

    // TODO use enum for ret values
    switch (ret) {
        case 1:
            printf("No response from I²C slave.\n");
            break;
        case 2:
            printf("Hardware failure.\n");
            break;
        case 0:
        default:
            printf("Success!\n");
            break;
    }
    // TODO return read value in stdout

    Close(mpsse);
    return 0;
}

uint8 i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size) {
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
uint8 i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size) {

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
