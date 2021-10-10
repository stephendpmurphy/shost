//
// Created by noeel on 10-10-21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

const cli_cmd_t cli_options[NUM_CLI_OPTIONS] = {
        {
                .cmd_short = "-l",
                .cmd_full = "--list",
                .desc = "Displays number of FTDI devices connected, number of MPSSE channels available and available MPSSE channel information.",
                .type = ARG_LIST,
        },
        {
                .cmd_short = "-v",
                .cmd_full = "--version",
                .desc = "Displays the current cli version.",
                .type = ARG_VERSION,
        },
        {
                .cmd_short = "-h",
                .cmd_full = "--help",
                .desc = "Displays this help menu.",
                .type = ARG_HELP,
        },
        {
                .cmd_short = "",
                .cmd_full = "spi",
                .desc = "Initiate a SPI read or write transfer.",
                .type = ARG_SPI,
        },
        {
                .cmd_short = "",
                .cmd_full = "i2c",
                .desc = "Initiate I²C for read or write transfer.",
                .type = ARG_I2C,
        },
};

const cli_cmd_t sub_options[NUM_SUB_OPTIONS] = {
        {
                .cmd_short = "-h",
                .cmd_full = "--help",
                .desc = "Displays this help menu.",
        },
        {
                .cmd_short = "-c",
                .cmd_full = "--channel",
                .desc = "MPSSE Channel #. (Available channels can be retrieved with mpsse-cli -l)",
        },
        {
                .cmd_short = "-f",
                .cmd_full = "--freq",
                .desc = "SPI Frequency",
        },
        {
                .cmd_short = "-x",
                .cmd_full = "--xfer",
                .desc = "Set transfer type. [r | w | rw]",
        },
        {
                .cmd_short = "-l",
                .cmd_full = "--len",
                .desc = "Number of bytes to read/write",
        },
        {
                .cmd_short = "-d",
                .cmd_full = "--data",
                .desc = "Data to be written. Command delimeted list in hex. example: \'mpsse-cli spi -c 0 -x w -l 8 -d 0xDD,0xEE,0xAA,0xDD,0xBB,0xEE,0xEE,0xFF\'",
        },
        {
                .cmd_short = "-a",
                .cmd_full = "--address",
                .desc = "I²C address to read/write to",
        },
        {
                .cmd_short = "-r",
                .cmd_full = "--register",
                .desc = "I²C register to read/write",
        },
};


int CB_printCLIoptions(arg_t* arg) {
    printf("usage: mpsse-cli [...option] [...sub-options]]\n\n");

    if( NUM_CLI_OPTIONS > 0 )
        printf("options:\n");

    for(int i = 0; i < NUM_CLI_OPTIONS; i++) {
        printf("    ");
        if( strlen(cli_options[i].cmd_short) > 0 )
            printf("%s, ", cli_options[i].cmd_short);

        if( strlen(cli_options[i].cmd_full) > 0 )
            printf("%s", cli_options[i].cmd_full);

        printf(" - %s\n", cli_options[i].desc);
    }
    printf("\n");
    if(NUM_SUB_OPTIONS > 0 )
        printf("submodule options:\n");

    for(int i = 0; i < NUM_SUB_OPTIONS; i++) {
        printf("    ");
        if(strlen(sub_options[i].cmd_short) > 0 )
            printf("%s, ", sub_options[i].cmd_short);

        if(strlen(sub_options[i].cmd_full) > 0 )
            printf("%s", sub_options[i].cmd_full);

        printf(" - %s\n", sub_options[i].desc);
    }
    printf("\n\n");
}

int parsecli(int argc, char *argv[], arg_t* res) {

    if( argc <= 1 ) {
        return -1;
    }

    // Process the first cmd option.. It should either be a cli option or a sub-command
    for(int opt_i = 0; opt_i < NUM_CLI_OPTIONS; opt_i++) {
        if( ( (strlen(cli_options[opt_i].cmd_full) > 0) && (strcmp(argv[1], cli_options[opt_i].cmd_full) == 0) ) ||
            ( (strlen(cli_options[opt_i].cmd_short) > 0) && (strcmp(argv[1], cli_options[opt_i].cmd_short) == 0) ) ) {
            // Mark our flag saying we found a matching command.
            res->type = cli_options[opt_i].type;
        }
    }

    // Some amount of options have been given.. Loop through and try to create
    // a transfer data struct
    for(int i=2; i < argc; i=i+2) {
        // Make sure the arg we are looking at is an option (Starts with an '-')
        if( checkIfArgIsOption(argv[i]) < 0 ) {
            printf("Expected an option. Instead got: %s\n", argv[i]);
            return -1;
        }

        // Now make sure the next arg is NOT an option.
        // We expect a value for our option
        if( checkIfArgIsOption(argv[i+1]) >= 0 ) {
            printf("Expected a value for the %s option. Instead got: %s\n", argv[i], argv[i+1]);
            return -1;
        }

        // Now start running through our options and see if a valid option is found
        if( (strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--channel") == 0)) {
            if( (i+1) >= argc ) {
                printf("No channel given. Defaulting to 0.\n");
                res->channel = 0;
            }
            else {
                res->channel = strtol(argv[i+1], NULL, 10);
            }
        }
        else if( (strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--freq") == 0)) {
            if( (i+1) >= argc ) {
                printf("No frequency given. Defaulting to 100kb/s.\n");
                res->clk = 100000;
            }
            else {
                res->clk = strtol(argv[i+1], NULL, 10);
            }
        }
        else if( (strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--len") == 0)) {
            if( (i+1) >= argc ) {
                printf("-l option found, but no value given.\n");
                return -1;
            }
            else {
                res->len = strtol(argv[i+1], NULL, 10);
            }
        }
        else if( (strcmp(argv[i], "-a") == 0) || (strcmp(argv[i], "--address") == 0)) {
            if( (i+1) >= argc ) {
                printf("-a option found, but no value given.\n");
                return -1;
            }
            else {
                uint8_t address = 0;
                int index;
                parseCommaDelimetedData(argv[i+1], &address, &index);
                res->address = address;
            }
        }
        else if( (strcmp(argv[i], "-r") == 0) || (strcmp(argv[i], "--register") == 0)) {
            if( (i+1) >= argc ) {
                printf("-r option found, but no value given.\n");
                return -1;
            }
            else {
                uint8_t reg = 0;
                int index;
                parseCommaDelimetedData(argv[i+1], &reg, &index);
                res->_register = reg;
            }
        }
        else if( (strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--data") == 0)) {
            if( (i+1) >= argc ) {
                printf("Please provide data to be transfered when using the -d option.\n");
                return -1;
            }
            else {
                memset(res->buff, 0x00, sizeof(res->buff));

                // Pick through the comma delimeted list and create our tx buffer
                if( parseCommaDelimetedData(argv[i+1], res->buff, &res->len) < 0 ) {
                    return -1;
                }

                if( res->len < 1 ) {
                    printf("Data length must be greater than 0.\n");
                    return -1;
                }
            }
        }
        else if( (strcmp(argv[i], "-x") == 0) || (strcmp(argv[i], "--xfer") == 0)) {
            if( (i+1) >= argc ) {
                printf("You must provide a transfer type.\n");
                return -1;
            }

            if( (strcmp(argv[i+1], "r") == 0) ) {
                res->xferType = XFER_READ;
            }
            else if( (strcmp(argv[i+1], "w") == 0) ) {
                res->xferType = XFER_WRITE;
            }
            else if( (strcmp(argv[i+1], "rw") == 0) ) {
                res->xferType = XFER_READ_WRITE;
            }
            else {
                printf("Invalid xfer type given: %s\n", argv[i+1]);
                return -1;
            }
        }
        else {
            printf("Invalid option: %s. Ignoring it and moving on.\n", argv[i]);
        }
    }
}


int8_t parseCommaDelimetedData(char *arg, uint8_t *destBuff, int *buffIndex) {
    int listLength = 0;
    int lastCommaIndex = 0;
    char hexCharacter[5] = {0x00};
    int index = 0;

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
            destBuff[index] = strtol(hexCharacter, NULL, 16);
            index++;
        }

        // We can safely assume that if our index gets too far from the last
        // comma index.. Then something is wrong with the list
        if( (i - lastCommaIndex) > 4) {
            printf("Something is wrong with the provided list. Ensure no commas are missing in list.\n");
            return -1;
        }
    }

    // Only update the index if it is zero
    if( *buffIndex <= 0 ) {
        *buffIndex = index;
    }

    return 0;
}

int8_t checkIfArgIsOption(char *arg) {
    if( arg == NULL )
        return -1;

    // Make sure the provided arg is actually there
    if( strlen(arg) > 0 ) {
        // Check that the first character is at least a "-".
        if(strncmp(arg, "-", 1) == 0 ) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }
}
