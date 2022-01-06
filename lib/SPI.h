#ifndef _SPI_H_
#define _SPI_H_

#include "Protocol.h"
#include "shost.h"

int spi_processCmd(shost_xfer_t* xfer);

class SPI: public Protocol {
public:
    SPI();

private:
    void _write(shost_xfer_t* xfer);
    void _read(shost_xfer_t* xfer);

    shost_ret_t spi_write(uint8_t *src_buffer, uint8_t *dest_buffer, size_t buffer_len);
    shost_ret_t spi_read(uint8_t *dest_buffer, size_t buffer_len);
};

#endif // _SPI_H_