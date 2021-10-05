//
// Created by noeel on 05-10-21.
//
//#include "libMPSSE_i2c.h"
#include "i2c.h"
#include <mpsse.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>

const static size_t ACK_TIME = 0;
struct mpsse_context* mpsse;

// TODO copied from spi, not DRY
const cli_cmd_t i2c_options[NUM_I2C_OPTIONS] = {
        {
                .cmd_short = "-h",
                .cmd_full = "--help",
                .desc = "Displays this help menu.",
                .cb = CB_printI2Cclioptions
        },
        {
                .cmd_short = "-c",
                .cmd_full = "--channel",
                .desc = "MPSSE Channel #. (Available channels can be retrieved with mpsse-cli -l)",
                .cb = NULL
        },
        {
                .cmd_short = "-f",
                .cmd_full = "--freq",
                .desc = "SPI Frequency",
                .cb = NULL
        },
        {
                .cmd_short = "-x",
                .cmd_full = "--xfer",
                .desc = "Set transfer type. [r | w]",
                .cb = NULL
        },
        {
                .cmd_short = "-l",
                .cmd_full = "--len",
                .desc = "Number of bytes to read/write",
                .cb = NULL
        },
        {
                .cmd_short = "-d",
                .cmd_full = "--data",
                .desc = "Data to be written. Command delimeted list in hex. example: \'mpsse-cli spi -c 0 -x w -l 8 -d 0xDD,0xEE,0xAA,0xDD,0xBB,0xEE,0xEE,0xFF\'",
                .cb = NULL
        },
        {
                .cmd_short = "-a",
                .cmd_full = "--address",
                .desc = "I²C address to read/write to",
                .cb = NULL
        },
        {
                .cmd_short = "-r",
                .cmd_full = "--register",
                .desc = "I²C register to read/write",
                .cb = NULL
        },
};


int8 i2c_processCmd(int argc, char *argv[]) {
    xfer_t options;
    if(parsecli(argc, argv, &options) < 0)
        return -1;
    //    open mpsse device
    // TODO make freq adjustable
    mpsse = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB);
    if (mpsse != NULL && !mpsse->open) {
        printf("Failed to initialize MPSSE: %s\n", ErrorString(mpsse));
        return -1;
    }
    int ret = 0;

    if (options.xferType == XFER_WRITE)
        ret = i2c_write(options.address, options._register, options.buff, options.len);
    else if (options.xferType == XFER_READ)
        ret = i2c_read(options.address, options._register, options.buff, options.len);

    // TODO use enum for ret values
    switch (ret) {
        case 1:
            printf("No response from I²C slave.\n");
            break;
        case 2:
            printf("Hardware failure.\n");
            break;
        case 0:
        default:
            printf("Success!\n");
            break;
    }
    // TODO return read value in stdout

    Close(mpsse);
}

// TODO again stolen from spi
int parsecli(int argc, char *argv[], xfer_t* res) {

    if( argc < 1 ) {
        printf("Please provide at least one option and value.\n\n");
        (void)CB_printI2Cclioptions(0, NULL);
        return -1;

    }
    // Make sure there isnt a help option within the arg list.. If so..
    // Just display the help screen and return
    for(int i=0; i < argc; i++) {
        if( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            (void)CB_printI2Cclioptions(0, NULL);
            return 0;
        }
    }

    // Some amount of options have been given.. Loop through and try to create
    // a transfer data struct
    for(int i=0; i < argc; i=i+2) {
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
                uint8 address = 0;
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
                uint8 reg = 0;
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

// TODO copied from spi, not DRY
int8 CB_printI2Cclioptions(int argc, char *argv[]) {
    printf("usage: mpsse-cli i2c [...option]\n\n");

    if( NUM_I2C_OPTIONS > 0 )
        printf("options:\n");

    for(int i = 0; i < NUM_I2C_OPTIONS; i++) {
        printf("    ");
        if( strlen(i2c_options[i].cmd_short) > 0 )
            printf("%s, ", i2c_options[i].cmd_short);

        if( strlen(i2c_options[i].cmd_full) > 0 )
            printf("%s", i2c_options[i].cmd_full);

        printf(" - %s\n", i2c_options[i].desc);
    }

    printf("\n\n");
}

uint8 i2c_write(uint8 address, uint8 reg, uint8 *data, size_t data_size) {
    size_t write_size = 2 + data_size; // address + reg + datasize
    char *writebuffer = (char *) malloc(write_size);
    uint8 ret = 0;
    writebuffer[0] = ((address << 1) | 0b00000000);
    writebuffer[1] = reg;
    for (int i = 0; i < data_size; ++i) {
        writebuffer[i + 2] = data[i];
    }
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, writebuffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK)
                ret = 0;
            else
                ret = 1;
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = 2;
    free(writebuffer);
    return ret;
}


/**
 * Read data from I2C device
 * @param address address of slave
 * @param reg register to read
 * @param buffer store read data here
 * @param buffer_size read size
 * @return 0 if success, 1 if no response and 2 if hardware failed.
 */
uint8 i2c_read(uint8 address, uint8 reg, uint8 *buffer, size_t buffer_size) {

    uint8 ret = 2;
    const static size_t write_size = 2; // address + reg
    char write_buffer[write_size];
    uint8 *read_buffer = (uint8 *) malloc(buffer_size);
    write_buffer[0] = ((address << 1) | 0b00000000); // write mode
    write_buffer[1] = reg;
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, write_buffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK) {
                if (Start(mpsse) == MPSSE_OK) {
                    char w = ((address << 1) | 0b00000001); // read mode
                    if (Write(mpsse, &w, 1) == MPSSE_OK) {
                        if (GetAck(mpsse) == ACK) {
                            read_buffer = (uint8 *)Read(mpsse, buffer_size);
                            memcpy(buffer, read_buffer, buffer_size);
                            SendNacks(mpsse);
                            ret = 0;
                        } else {
                            ret = 1;
                        }
                    }
                }
            } else {
                ret = 1;
            }
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = 2;
    free(read_buffer);
    return ret;
}
