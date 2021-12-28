#ifndef _XFER_H_
#define _XFER_H_

#include <stdint.h>

#define MAX_BUFF_SIZE 256

typedef enum {
    XFER_INTF_NONE = 0x00,
    XFER_INTF_SPI,
    XFER_INTF_I2C,
    XFER_INTF__MAX__
} xfer_intf_t;

typedef enum {
    XFER_NONE = 0x00,
    XFER_WRITE,
    XFER_READ,
    XFER_READ_WRITE,
    XFER__MAX__
} xfer_type_t;

typedef struct {
    xfer_intf_t intf;
    xfer_type_t xferType;
    int clk;
    int channel;
    int len;
    int bytesTranferred;
    uint8_t buff[MAX_BUFF_SIZE];
} xfer_t;

#endif // _XFER_H_