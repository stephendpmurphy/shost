#ifndef _XFER_H_
#define _XFER_H_

#include <stdint.h>
#include "libMPSSE_spi.h"
#include "ftd2xx.h"

#define MAX_BUFF_SIZE 256

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};

#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

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
    int clk;
    int channel;
    int len;
    int bytesTranferred;
    int address;
    int _register;
    xfer_intf_t intf;
    xfer_type_t xferType;
    uint8_t buff[MAX_BUFF_SIZE];
} xfer_t;

#endif // _XFER_H_