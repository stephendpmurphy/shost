//
// Created by noeel on 05-10-21.
//

#ifndef MPSSE_CLI_I2C_H
#define MPSSE_CLI_I2C_H

#include "mpsse-cli.h"
#include "cli.h"

// TODO needs documentation

uint8 i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size);
uint8 i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size);

int i2c_processCmd(arg_t *arg);

#endif //MPSSE_CLI_I2C_H
