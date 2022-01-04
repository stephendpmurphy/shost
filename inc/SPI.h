#ifndef _SPI_H_
#define _SPI_H_

#include "libMPSSE_spi.h"
#include "Protocol.h"
#include "cli.h"

int spi_processCmd(arg_t* arg);

class SPI: public Protocol {
public:
    SPI();

private:
    void _write(arg_t* arg);
    void _read(arg_t* arg);
};

#endif // _SPI_H_