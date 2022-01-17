#include <argp.h>
#include <argz.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "shost.h"
#include "version.h"

const char *argp_program_bug_address = "https://github.com/stephendpmurphy/shost/issues";
static char *outputFilePath;
static FILE *outBin = NULL;

volatile shost_xfer_t xfer = {
    10000, // Clock
    0, // Channel
    0, // Tx Length
    0, // Rx Length
    0, // Bytes transferred
    0, // Address
    0, // Register
    XFER_INTF_NONE, // Interface
    XFER_TYPE_NONE, // Transfer Type
    {0x00}
};


static void dump_array(uint8_t *arr, int len) {
    for(int i = 0; i < len; i++) {
        printf("%02X ", arr[i]);
    }
    printf("\n");
}

static int8_t parseCommaDelimetedData(char *arg, uint8_t *destBuff, int *buffIndex) {
    int listLength = 0;
    int lastCommaIndex = 0;
    char hexCharacter[5] = {0x00};

    if( (arg == NULL) || (destBuff == NULL) ) {
        printf("NULL pointer passed.\n");
        return -1;
    }

    // Clear out our buffIndex (length) before starting
    if( *buffIndex ) {
        printf("Length set via the -l option. Clearing and setting length from -d buffer length.\n");
        *buffIndex = 0;
    }

    // Determine out list length
    listLength = strlen(arg);

    // Loop through the list of characters processing until we find a comma
    for(int i = 0; i <= listLength; i++) {

        if( (!memcmp(&arg[i], ",", 0x01)) && (i == listLength-1))  {
            printf("Trailing comma on data list.\n");
            return -1;
        }
        else if( (!memcmp(&arg[i], ",", 0x01)) || (i == listLength) ) {
            // Ensure we clear out our hex character array before parsing the next
            memset(hexCharacter, 0x00, sizeof(hexCharacter));
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
                argp_failure(state, 0,0,"Invalid transfer type provided.");
                return 1;
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
            if(parseCommaDelimetedData(arg, xfer_ptr->tx_buff, &xfer_ptr->tx_len) < 0) {
                argp_failure(state, 0,0,"There was a problem parsing the data list.");
                return 1;
            }
            break;

        case 'l':
            // Parse the length and store it in our xfer object
            xfer_ptr->rx_len = atoi(arg);
            break;

        case 'o':
            outputFilePath = (char *)malloc(strlen(arg));
            strcpy(outputFilePath, arg);
            break;

        case 'v':
            // TODO: Implement verbosity
            break;

        case 777:
            shost_getConnectedDevices(true);
            return 1;

        case ARGP_KEY_END:
            if( state->argc <= 1 ) {
                argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            }
            else {
                if( 0 == shost_xfer_begin(xfer_ptr) ) {
                    // Print the TX and RX buffers
                    if( xfer_ptr->bytesTranferred ) {
                        if( xfer_ptr->tx_len ) {
                            printf("TX: ");
                            dump_array(xfer_ptr->tx_buff, xfer_ptr->bytesTranferred);
                        }
                        if( xfer_ptr->rx_len ) {
                            printf("RX: ");
                            dump_array(xfer_ptr->rx_buff, xfer_ptr->bytesTranferred);

                            if( outputFilePath != NULL ) {
                                // If the filepath isn't NULL, then write it out;
                                outBin = fopen(outputFilePath,"wb");  // w for write, b for binary
                                fwrite(xfer_ptr->rx_buff, 0x01, xfer_ptr->bytesTranferred, outBin); // write 10 bytes from our buffer
                                fclose(outBin);
                                free(outputFilePath);
                            }
                        }
                    }
                }
            }
            break;
    }
    return 0;
}

static void print_version(FILE *stream, struct argp_state *state) {
    fprintf(stream, "shost v%d.%d.%d\n", SHOST_VERSION_MAJOR, SHOST_VERSION_MINOR, SHOST_VERSION_PATCH);
    fprintf(stream, "MIT License - Copyright (c) 2022 Stephen Murphy\n");
    fprintf(stream, "written by @stephendpmurphy\n");
}

int main(int argc, char *argv[] ) {
    // Generate a list of our available CLI options
    struct argp_option cli_options[] = {
        {0,0,0,0, "General serial options:", 1},
        {"interface", 'i', "SPI || I2C", 0, "Serial interface selection"},
        {"xfer", 'x', "r || w || rw", 0, "Serial transfer type - Read (r), Write (w) or Read/Write (rw)"},
        {"channel", 'c', "NUM", 0, "MPSSE Channel # - Available channels can be retrieved with the --list option"},
        {"frequency", 'f', "NUM", 0, "Serial communication freqeuncy"},
        {0,0,0,0, "Serial WRITE options:", 2},
        {"data", 'd', "ARRAY", 0, "Comma delimited data to be written in hex."},
        {0,0,0,0, "Serial READ options:", 3},
        {"length", 'l', "NUM", 0, "Length of data to be read during the serial transfer"},
        {0,0,0,0, "I2C options:", 4},
        {"address", 'a', "NUM", 0, "Address of the I2C device you want to communicate with"},
        {"register", 'r', "NUM", 0, "Address of the I2C register you want to interact with"},
        {0,0,0,0, "Debug options:", 5},
        {"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Execute with verbose logging"},
        {"list", 777, 0, OPTION_ARG_OPTIONAL, "Display information about connected FTDI devices"},
        {"out", 'o', 0, 0, "Filepath to write RX data to after the transfer completes."},
        {0}
    };

    // Creat our argp struct
    struct argp argp = { cli_options, parse_opt };

    // Store our program version hook function
    argp_program_version_hook = print_version;

    // Begin parsing options
    return argp_parse (&argp, argc, argv, 0, 0, (void *__restrict__)&xfer);
}