//
// Created by noeel on 05-10-21.
//

#ifndef MPSSE_CLI_I2C_H
#define MPSSE_CLI_I2C_H

#include "shost.h"
#include "Protocol.h"

// TODO needs documentation

class I2C: public Protocol {
public:
    I2C();
private:
    void _write(shost_xfer_t* xfer);
    void _read(shost_xfer_t* xfer);

    uint8_t i2c_write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size);
    uint8_t i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size);
};

#endif //MPSSE_CLI_I2C_H
