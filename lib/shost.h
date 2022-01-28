#ifndef _SHOST_H_
#define _SHOST_H_

#include <stdint.h>
#include <stddef.h>
#include "ftdi.h"

/**
 * @brief shost standard return codes
 */
typedef enum {
    SHOST_RET_OK = 0x00,    /* All OK */
    SHOST_RET_LIB_ERR,      /* Library error */
    SHOST_RET_HW_ERR,       /* General HW error */
    SHOST_RET_NAK,          /* Device NAK error*/
    SHOST_RET__MAX__        /* __MAX__ return code */
} shost_ret_t;

/**
 * @brief shost interface type
 */
typedef enum {
    XFER_INTF_NONE = 0x00,  /* NO interface */
    XFER_INTF_SPI,          /* SPI interface */
    XFER_INTF_I2C,          /* I2C interface */
    XFER_INTF__MAX__        /* __MAX__ interface */
} shost_xfer_intf_t;

/**
 * @brief shost transfer types
 */
typedef enum {
    XFER_TYPE_NONE = 0x00,  /* NO transfer */
    XFER_TYPE_WRITE,        /* WRITE transfer*/
    XFER_TYPE_READ,         /* READ transfer */
    XFER_TYPE_READ_WRITE,   /* Read & Write transfer */
    XFER_TYPE__MAX__        /* __MAX__ transfer */
} shost_xfer_type_t;


/**
 * @brief shost channel for transfer
 */
typedef enum {
    XFER_CH_A = 0x00,   /* Channel A */
    XFER_CH_B,          /* Channel B */
    XFER_CH_C,          /* Channel C */
    XFER_CH_D,          /* Channel D */
    XFER_CH__MAX__      /* Channel __MAX__ */
} shost_xfer_channel_t;

/**
 * @brief shost device information
 */
typedef struct {
    char manufacturer[128];
    char description[128];
} shost_device_info_t;

/**
 * @brief shost list for holding connected devices
 */
typedef struct {
    shost_device_info_t info[128];
} shost_connected_devices_t;

/**
 * @brief shost transfer object
 */
typedef struct {
    int clk;                        /* Transfer clock rate */
    shost_xfer_channel_t channel;   /* MPSSE device channel (A, B, C, D) */
    int tx_len;                     /* Transfer transmit length */
    int rx_len;                     /* Transfer receive length */
    int bytesTranferred;            /* Number of bytes actually transmitted */
    int address;                    /* I2C device address */
    int _register;                  /* I2C device register address */
    shost_xfer_intf_t intf;         /* Transfer interface (SPI or I2C) */
    shost_xfer_type_t xferType;     /* Transfer type (Read, Write, Read/Write) */
    uint8_t *tx_buff;               /* Pointer to the the Transmit buffer */
    uint8_t *rx_buff;               /* Pointer to the receive buffer */
} shost_xfer_t;

/**
 * @brief Get all of the compatible connected devices
 * @param *connectedDevices Pointer to array where connected device strings will be stored
 * @return number of connected devices
 */
int shost_getConnectedDevices(shost_connected_devices_t *connectedDevices);

/**
 * @brief Begin a transfer using shost
 * @param *xfer Pointer to our transfer object
 * @return shost standard return code shost_ret_t
 */
int shost_xfer_begin(shost_xfer_t *xfer);

#endif // _SHOST_H_