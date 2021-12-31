// #include <stdio.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <sys/stat.h>
// #include <sys/syscall.h>
// #include <sys/types.h>
// #include <unistd.h>
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "xfer.h"
#include "util.h"

volatile xfer_t xfer = {
    10000, // Clock
    0, // Channel
    0, // Length
    0, // Bytes transferred
    0, // Address
    0, // Register
    XFER_INTF_NONE, // Interface
    XFER_TYPE_NONE, // Transfer Type
    {0x00}
};

static int parse_opt (int key, char *arg, struct argp_state *state) {
    xfer_t *xfer_ptr = (xfer_t *)state->input;

    switch (key) {
        case 'i':
            if((strcmp(arg, "SPI") == 0) || (strcmp(arg, "spi") == 0)) {
                xfer_ptr->intf = XFER_INTF_SPI;
            }
            else if((strcmp(arg, "I2C") == 0) || (strcmp(arg, "i2c") == 0)) {
                xfer_ptr->intf = XFER_INTF_I2C;
            }
            else {
                argp_failure(state, 0,0,"Invalid serial interface provided.");
            }
            break;

        case 'c':
            if(arg == NULL) {
                printf("No channel value given. Defaulting to channel 0.\n");
                xfer_ptr->channel = 0;
            }
            else {
                xfer_ptr->channel = atoi(arg);
            }
            break;

        case 'f':
            if( arg == NULL ) {
                printf("No frequency value given. Defaulting to 10kHz.\n");
                xfer_ptr->clk = 10000;
            }
            else {
                xfer_ptr->clk = atoi(arg);
            }
            break;

        case 'x':
            if((strcmp(arg, "W") == 0) || (strcmp(arg, "w") == 0)) {
                xfer_ptr->xferType = XFER_TYPE_WRITE;
            }
            else if((strcmp(arg, "R") == 0) || (strcmp(arg, "r") == 0)) {
                xfer_ptr->xferType = XFER_TYPE_READ;
            }
            else if((strcmp(arg, "RW") == 0) || (strcmp(arg, "rw") == 0)) {
                xfer_ptr->xferType = XFER_TYPE_READ_WRITE;
            }
            else {
                argp_failure(state, 0,0,"Invalid transmittion type provided.");
            }
            break;

        case 'd':
            // Parse the comma delimeted string into a data array
            break;

        case 'l':
            // Parse the length and store it in our xfer object
            xfer_ptr->len = atoi(arg);
            break;

        case 'v':
            // TODO: Implement verbosity

        case 777:
            util_printMPSSEchannelInfo( util_getMPSSEchannelCount() );
            // Return 1 so we stop parsing arguments after printing available channels
            return 1;

        case ARGP_KEY_END:
            if( state->argc <= 1 ) {
                argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            }
            else {
                xfer_begin(*xfer_ptr);
            }
            break;
    }
    return 0;
}

int main(int argc, char *argv[] ) {
    struct argp_option cli_options[] = {
        {"interface", 'i', "SPI || I2C", 0, "Serial interface selection"},
        {"channel", 'c', "NUM", 0, "MPSSE Channel # - Available channels can be retrieved with the --list option"},
        {"frequency", 'f', "NUM", 0, "Serial communication freqeuncy"},
        {"xfer", 'x', "r || w || rw", 0, "Serial transfer type - Read, Write or Read & Write"},
        {"data", 'd', "ARRAY", 0, "Comma delimted data to be written in hex."},
        {"length", 'l', "NUM", 0, "Length of data to be read during the serial transfer"},
        {"verbose", 'v', "NUM", OPTION_ARG_OPTIONAL, "Increase verbosity of logging"},
        {"list", 777, 0, OPTION_ARG_OPTIONAL, "Display information about connected FTDI devices and available MPSSE channels"},
        {0}
    };

    struct argp argp = { cli_options, parse_opt };

    return argp_parse (&argp, argc, argv, 0, 0, (void *__restrict__)&xfer);
}