//
// Created by noeel on 03-12-21.
//

#ifndef MPSSE_CLI_PROTOCOL_H
#define MPSSE_CLI_PROTOCOL_H

#include "shost.h"

/**
 * Protocol base class, used by I²C and SPI to create a abstraction for communication.
 */
class Protocol{
public:
    /**
     * Name of the protocol
     */
    const char* name;

    /**
     * Protocol object, should be created by child to give a proper name used for documentation
     * @param name name of the protocol
     */
    Protocol(const char* name);

    /**
     * read data using set protocol.
     * Read data is returned in rx_buff
     * @param xfer requires:
     *  -   rx_len,     length of data to read
     *  -   clk,        clock speed to use (Hertz)
     *  -   channel,    MPSSE channel to use
     *  -   rx_buff,    stores read data
     *  -   xferType    set to READ
     *
     *  Please note that depending on the protocol being used additional parameters may be required.
     *  See the relevant protocol (_read) for more information.
     *
     *  @throws std::system_error when things go wrong
     *  The operation was successful if no exception was raised.
     */
    void read(shost_xfer_t* xfer);

    /**
     * Write data using set protocol.
     * @param xfer requires:
     *  -   tx_len,     length of data to write
     *  -   clk,        clock speed to use (Hertz)
     *  -   channel,    MPSSE channel to use
     *  -   tx_buff,    pointer to data array to write
     *  -   xferType    set to WRITE
     *
     *  Please note that depending on the protocol being used additional parameters may be required.
     *  See the relevant protocol (_write) for more information.
     *
     *  @throws std::system_error when things go wrong
     *  The operation was successful if no exception was raised.
     */
    void write(shost_xfer_t* xfer);

    /**
     * Not yet implemented!
     * @param xfer
     */
    void read_write(shost_xfer_t* xfer);

protected:
    /**
     * MPSSE instance object used for interfacing with FT223x.
     * Open and close the connection in every function.
     * See https://github.com/stephendpmurphy/shost/issues/43
     */
    struct mpsse_context* mpsse;

private:

    /**
     * Protocol specific implementation of the write command.
     * @param xfer supply documentation on required parameters!
     */
    void virtual _write(shost_xfer_t* xfer) = 0;

    /**
     * Protocol specific implementation of the read command.
     * @param xfer supply documentation on required parameters!
     */
    void virtual _read(shost_xfer_t* xfer) = 0;
};


#endif //MPSSE_CLI_PROTOCOL_H
