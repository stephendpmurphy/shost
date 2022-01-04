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
} shost_xfer_intf_t;

typedef enum {
    XFER_TYPE_NONE = 0x00,
    XFER_TYPE_WRITE,
    XFER_TYPE_READ,
    XFER_TYPE_READ_WRITE,
    XFER_TYPE__MAX__
} shost_xfer_type_t;

typedef struct {
    int clk;
    int channel;
    int len;
    int bytesTranferred;
    int address;
    int _register;
    shost_xfer_intf_t intf;
    shost_xfer_type_t xferType;
    uint8_t buff[MAX_BUFF_SIZE];
} shost_xfer_t;

int shost_xfer_begin(shost_xfer_t xfer);

#endif // _XFER_H_