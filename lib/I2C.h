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

    uint8 i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size);
    uint8 i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size);
};

#endif //MPSSE_CLI_I2C_H
