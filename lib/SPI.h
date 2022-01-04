#ifndef _SPI_H_
#define _SPI_H_

#include "libMPSSE_spi.h"
#include "Protocol.h"
#include "shost.h"

int spi_processCmd(shost_xfer_t* xfer);

class SPI: public Protocol {
public:
    SPI();

private:
    void _write(shost_xfer_t* xfer);
    void _read(shost_xfer_t* xfer);
};

#endif // _SPI_H_