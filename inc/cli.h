//
// Created by noeel on 10-10-21.
//

#ifndef MPSSE_CLI_CLI_H
#define MPSSE_CLI_CLI_H

#include <stdint-gcc.h>

#define MAX_BUFF_SIZE 256
#define NUM_SUB_OPTIONS 8
#define NUM_CLI_OPTIONS 5

typedef enum {
    XFER_NONE = 0x00,
    XFER_WRITE,
    XFER_READ,
    XFER_READ_WRITE,
    XFER__MAX__
} xfer_type_t;

typedef enum {
    ARG_NONE,
    ARG_SPI,
    ARG_I2C,
    ARG_HELP,
    ARG_VERSION,
    ARG_LIST,
    ARG__MAX__,
} arg_type_t;

typedef struct {
    int clk;
    int channel;
    int len;
    int bytesTranferred;
    int address;
    int _register;
    arg_type_t type;
    xfer_type_t xferType;
    uint8_t buff[MAX_BUFF_SIZE];
} arg_t;

typedef struct {
    char cmd_short[64];
    char cmd_full[64];
    char desc[256];
    arg_type_t type;
} cli_cmd_t;

extern const cli_cmd_t cli_options[NUM_CLI_OPTIONS];

extern const cli_cmd_t sub_options[NUM_SUB_OPTIONS];

int8_t parseCommaDelimetedData(char *arg, uint8_t *destBuff, int *buffIndex);

int8_t checkIfArgIsOption(char *arg);

int parsecli(int argc, char *argv[], arg_t* res);

int CB_printCLIoptions(arg_t* arg);

#endif //MPSSE_CLI_CLI_H
