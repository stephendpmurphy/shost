#ifndef _SPI_H_
#define _SPI_H_

#include "Protocol.h"
#include "shost.h"

int spi_processCmd(shost_xfer_t* xfer);

class SPI: public Protocol {
public:
    /**
     * @brief SPI Controller
     */
    SPI();

private:
    /**
     * @brief Write using SPI
     * @param xfer struct holding the transfer information
     */
    void _write(shost_xfer_t* xfer);

    /**
     * @brief Read using SPI
     * @param xfer struct holding the transfer information
     */
    void _read(shost_xfer_t* xfer);

    /**
     * @brief Write data to SPI device
     * @param address 7bit address of salve
     * @param reg register to write data to
     * @param data pointer to byte array
     * @param data_size size of data array
     * @return 0 if success, 1 if no response and 2 if hardware failed.
     */
    shost_ret_t spi_write(uint8_t *src_buffer, uint8_t *dest_buffer, size_t buffer_len);

    /**
     * @brief Read data from the SPI device
     * @param *dest_buffer pointer to buffer where the read data should be stored
     * @param buffer_len length of data to be read
     * @return shost standard return code (see shost_ret_t)
     */
    shost_ret_t spi_read(uint8_t *dest_buffer, size_t buffer_len);
};

#endif // _SPI_H_