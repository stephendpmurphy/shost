//
// Created by noeel on 05-10-21.
//

#ifndef MPSSE_CLI_I2C_H
#define MPSSE_CLI_I2C_H

#include "shost.h"
#include "Protocol.h"

class I2C : public Protocol {
public:
    /**
     * I²C Master
     */
    I2C();

private:
    /**
     * Write using I²C.
     * @param xfer requires:
     *  -   address,    7 bit address of slave.
     *  -   _register,  register to read.
     */
    void _write(shost_xfer_t *xfer);

    /**
     * Read using I²C.
     * @param xfer requires:
     *  -   address,    7 bit address of slave.
     *  -   _register,  register to write.
     */
    void _read(shost_xfer_t *xfer);

    /**
     * Write data to I²C device
     * @param address 7bit address of salve
     * @param reg register to write data to
     * @param data pointer to byte array
     * @param data_size size of data array
     * @return 0 if success, 1 if no response and 2 if hardware failed.
     */
    uint8_t i2c_write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size);

    /**
     * Read data from I2C device
     * @param address address of slave
     * @param reg register to read
     * @param buffer store read data here
     * @param buffer_size read size
     * @return 0 if success, 1 if no response and 2 if hardware failed.
     */
    uint8_t i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size);
};

#endif //MPSSE_CLI_I2C_H
