#ifndef _SPI_H_
#define _SPI_H_

#include "libMPSSE_spi.h"
#include "Protocol.h"
#include "xfer.h"

int spi_processCmd(xfer_t* xfer);

class SPI: public Protocol {
public:
    SPI();

private:
    void _write(xfer_t* xfer);
    void _read(xfer_t* xfer);
};

#endif // _SPI_H_