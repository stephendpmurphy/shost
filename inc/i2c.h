//
// Created by noeel on 05-10-21.
//

#ifndef MPSSE_CLI_I2C_H
#define MPSSE_CLI_I2C_H

#include "mpsse-cli.h"

#define NUM_I2C_OPTIONS 8

// TODO needs documentation

int8 CB_printI2Cclioptions(int argc, char *argv[]);
uint8 i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size);
uint8 i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size);

// TODO copied from spi, not DRY
// TODO needs to defined here but compiler was complaining
//const cli_cmd_t i2c_options[NUM_I2C_OPTIONS];

int8 i2c_processCmd(int argc, char *argv[]);
int parsecli(int argc, char *argv[], xfer_t* res);

#endif //MPSSE_CLI_I2C_H
