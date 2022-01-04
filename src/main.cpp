#include <argp.h>
#include <argz.h>
#include <stdlib.h>
#include <string.h>
#include "shost.h"
#include "util.h"

const char *argp_program_bug_address = "stephendpmurphy@msn.com";
const char *argp_program_version = "version 1.0";

volatile shost_xfer_t xfer = {
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

static int8 parseCommaDelimetedData(char *arg, uint8 *destBuff, int *buffIndex) {
    int listLength = 0;
    int lastCommaIndex = 0;
    char hexCharacter[5] = {0x00};

    if( (arg == NULL) || (destBuff == NULL) ) {
        printf("NULL pointer passed.\n");
        return -1;
    }

    listLength = strlen(arg);

    // Loop through the list of characters processing until we find a comma
    for(int i = 0; i <= listLength; i++) {

        if( (!memcmp(&arg[i], ",", 0x01)) && (i == listLength-1))  {
            printf("Trailing comma on data list.\n");
            return -1;
        }
        else if( (!memcmp(&arg[i], ",", 0x01)) || (i == listLength) ) {
            // We found a comma. grab the substring from last commad
            // to our current index and try to parse it as a hex value
            memcpy(hexCharacter, &arg[lastCommaIndex], i - lastCommaIndex);
            // Store the new "lastCommaIndex" value
            lastCommaIndex = i+1;

            // Now attempt to parse the character
            destBuff[*buffIndex] = strtol(hexCharacter, NULL, 16);
            *buffIndex = *buffIndex+1;
        }

        // We can safely assume that if our index gets too far from the last
        // comma index.. Then something is wrong with the list
        if( (i - lastCommaIndex) > 4) {
            printf("Something is wrong with the provided list. Ensure no commas are missing in list.\n");
            return -1;
        }
    }

    // printfArray(destBuff, buffIndex);

    return 0;
}

static int parse_opt (int key, char *arg, struct argp_state *state) {
    shost_xfer_t *xfer_ptr = (shost_xfer_t *)state->input;

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

        case 'a':
            // Parse the i2c address and store it in our xfer object
            xfer_ptr->address = atoi(arg);
            break;

        case 'r':
            // Parse the i2c register address
            xfer_ptr->_register = atoi(arg);
            break;

        case 'd':
            // Parse the comma delimeted string into a data array
            parseCommaDelimetedData(arg, xfer_ptr->buff, &xfer_ptr->len);
            printfArray(xfer_ptr->buff, xfer_ptr->len);
            break;

        case 'l':
            // Parse the length and store it in our xfer object
            xfer_ptr->len = atoi(arg);
            break;

        case 'v':
            // TODO: Implement verbosity
            break;

        case 777:
            util_printMPSSEchannelInfo( util_getMPSSEchannelCount() );
            // Return 1 so we stop parsing arguments after printing available channels
            return 1;

        case ARGP_KEY_END:
            if( state->argc <= 1 ) {
                argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            }
            else {
                // If the interface is SPI, do the libMPSSE init
                if( xfer_ptr->intf == XFER_INTF_SPI ) {
                    // Check if the FTDI serial module is loaded. If so, remove it.
                    // This requires sudo when running after a build. Not required when installed.
                    if (util_isFtdiModuleLoaded() > 0) {
                        util_removeFtdiModule();
                    }

                    /* init library */
                    Init_libMPSSE();
                }
                shost_xfer_begin(*xfer_ptr);
                // Cleanup
                if( xfer_ptr->intf == XFER_INTF_SPI ) {
                    Cleanup_libMPSSE();
                }
            }
            break;
    }
    return 0;
}

int main(int argc, char *argv[] ) {
    struct argp_option cli_options[] = {
        {0,0,0,0, "General serial options:", 1},
        {"interface", 'i', "SPI || I2C", 0, "Serial interface selection"},
        {"xfer", 'x', "r || w || rw", 0, "Serial transfer type - Read, Write or Read & Write"},
        {"channel", 'c', "NUM", 0, "MPSSE Channel # - Available channels can be retrieved with the --list option"},
        {"frequency", 'f', "NUM", 0, "Serial communication freqeuncy"},
        {"data", 'd', "ARRAY", 0, "Comma delimted data to be written in hex."},
        {"length", 'l', "NUM", 0, "Length of data to be read during the serial transfer"},
        {0,0,0,0, "I2C options:", 2},
        {"address", 'a', "NUM", 0, "Address of the I2C device you want to communicate with"},
        {"register", 'r', "NUM", 0, "Address of the I2C register you want to interact with"},
        {0,0,0,0, "Debug options:", 3},
        {"verbose", 'v', "NUM", OPTION_ARG_OPTIONAL, "Increase verbosity of logging"},
        {"list", 777, 0, OPTION_ARG_OPTIONAL, "Display information about connected FTDI devices and available MPSSE channels"},
        {0}
    };

    struct argp argp = { cli_options, parse_opt };

    return argp_parse (&argp, argc, argv, 0, 0, (void *__restrict__)&xfer);
}