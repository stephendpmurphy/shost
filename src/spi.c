#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpsse-cli.h"
#include "libMPSSE_spi.h"

#define NUM_SPI_OPTIONS 4
#define MAX_BUFF_SIZE 256

typedef enum {
    SPI_XFER_NONE = 0x00,
    SPI_XFER_WRITE,
    SPI_XFER_READ,
    SPI_XFER_READ_WRITE,
    SPI_XFER__MAX__
} spi_xfer_type_t;

typedef struct {
    FT_HANDLE ftHandle;
    int channel;
    int len;
    int bytesTranferred;
    spi_xfer_type_t xferType;
    uint8 buff[MAX_BUFF_SIZE];
} spi_xfer_t;

int8 CB_printSPIclioptions(int argc, char *argv[]);

const cli_cmd_t spi_options[NUM_SPI_OPTIONS] = {
    {
        .cmd_short = "-h",
        .cmd_full = "--help",
        .desc = "Displays this help menu.",
        .cb = CB_printSPIclioptions
    },
    {
        .cmd_short = "-c",
        .cmd_full = "--channel",
        .desc = "MPSSE Channel #. (Available channels can be retrieved with mpsse-cli -l)",
        .cb = NULL
    },
    {
        .cmd_short = "-x",
        .cmd_full = "--xfer",
        .desc = "Set transfer type. [r | w | rw]",
        .cb = NULL
    },
    {
        .cmd_short = "-d",
        .cmd_full = "--data",
        .desc = "Data to be written. Command delimeted list in hex. example: \'mpsse-cli spi -c 0 -x w -l 8 -d 0xDD,0xEE,0xAA,0xDD,0xBB,0xEE,0xEE,0xFF\'",
        .cb = NULL
    },
};

static void printfArray(uint8 *buff, uint16 len) {
    printf("Array dump: ");
    for(int i = 0; i < len; i++) {
        printf("0x%02X ", buff[i]);
    }

    printf("\n");
}

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

static int8 spi_write(spi_xfer_t *xfer) {
    FT_STATUS status = FT_OK;
    ChannelConfig channelConf = {0};
    FT_DEVICE_LIST_INFO_NODE devList = {0};

    channelConf.ClockRate = 5000;
	channelConf.LatencyTimer = 255;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    status = SPI_GetChannelInfo(xfer->channel, &devList);
    APP_CHECK_STATUS(status);

    status = SPI_OpenChannel(xfer->channel, &devList.ftHandle);
    APP_CHECK_STATUS(status);

    status = SPI_InitChannel(devList.ftHandle, &channelConf);
    APP_CHECK_STATUS(status);

    status = SPI_ToggleCS(devList.ftHandle, 1);
    APP_CHECK_STATUS(status);

    status = SPI_Write(devList.ftHandle, (uint8 *)&xfer->buff, xfer->len, &xfer->bytesTranferred,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
        // SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
        // SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE );
    APP_CHECK_STATUS(status);

    status = SPI_ToggleCS(devList.ftHandle, 0);
    APP_CHECK_STATUS(status);
}

int8 CB_printSPIclioptions(int argc, char *argv[]) {
    printf("usage: mpsse-cli spi [...option]\n\n");

    if( NUM_SPI_OPTIONS > 0 )
        printf("options:\n");

    for(int i = 0; i < NUM_SPI_OPTIONS; i++) {
        printf("    ");
        if( strlen(spi_options[i].cmd_short) > 0 )
            printf("%s, ", spi_options[i].cmd_short);

        if( strlen(spi_options[i].cmd_full) > 0 )
            printf("%s", spi_options[i].cmd_full);

        printf(" - %s\n", spi_options[i].desc);
    }

    printf("\n\n");
}

int8 spi_processCmd(int argc, char *argv[]) {
    int retVal = -1;
    spi_xfer_t spi_transfer = {0x00};

    if( argc < 1 ) {
        printf("Please provide at least one option and value.\n\n");
        (void)CB_printSPIclioptions(0, NULL);
        retVal = -1;
        goto SPI_CLEANUP;
    }

    // Make sure there isnt a help option within the arg list.. If so..
    // Just display the help screen and return
    for(int i=0; i < argc; i++) {
        if( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            (void)CB_printSPIclioptions(0, NULL);
            retVal = -1;
            goto SPI_CLEANUP;
        }
    }

    // Some amount of options have been given.. Loop through and try to create
    // a transfer data struct
    for(int i=0; i < argc; i=i+2) {
        // Make sure the arg we are looking at is an option (Starts with an '-')
        if( checkIfArgIsOption(argv[i]) < 0 ) {
            printf("Expected an option. Instead got: %s\n", argv[i]);
            retVal = -1;
            goto SPI_CLEANUP;
        }

        // Now make sure the next arg is NOT an option.
        // We expect a value for our option
        if( checkIfArgIsOption(argv[i+1]) >= 0 ) {
            printf("Expected a value for the %s option. Instead got: %s\n", argv[i], argv[i+1]);
            retVal = -1;
            goto SPI_CLEANUP;
        }

        // Now start running through our options and see if a valid option is found
        if( (strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--channel") == 0)) {
            if( (i+1) >= argc ) {
                printf("No channel given. Defaulting to 0.\n");
                spi_transfer.channel = 0;
            }
            else {
                spi_transfer.channel = strtol(argv[i+1], NULL, 10);
            }
        }
        else if( (strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--data") == 0)) {
            if( (i+1) >= argc ) {
                printf("Please provide data to be transfered when using the -d option.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            else {
                memset(spi_transfer.buff, 0x00, sizeof(spi_transfer.buff));

                // Pick through the comma delimeted list and create our tx buffer
                if( parseCommaDelimetedData(argv[i+1], spi_transfer.buff, &spi_transfer.len) < 0 ) {
                    retVal = -1;
                    goto SPI_CLEANUP;
                }

                if( spi_transfer.len < 1 ) {
                    printf("Data length must be greater than 0.\n");
                    retVal = -1;
                    goto SPI_CLEANUP;
                }
            }
        }
        else if( (strcmp(argv[i], "-x") == 0) || (strcmp(argv[i], "--xfer") == 0)) {
            if( (i+1) >= argc ) {
                printf("You must provide a transfer type.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }

            if( (strcmp(argv[i+1], "r") == 0) ) {
                spi_transfer.xferType = SPI_XFER_READ;
            }
            else if( (strcmp(argv[i+1], "w") == 0) ) {
                spi_transfer.xferType = SPI_XFER_WRITE;
            }
            else if( (strcmp(argv[i+1], "rw") == 0) ) {
                spi_transfer.xferType = SPI_XFER_READ_WRITE;
            }
            else {
                printf("Invalid xfer type given: %s\n", argv[i+1]);
                retVal = -1;
                goto SPI_CLEANUP;
            }
        }
        else {
            printf("Invalid option: %s. Ignoring it and moving on.\n", argv[i]);
        }
    }

SPI_XFER:
    switch( spi_transfer.xferType ) {
        case SPI_XFER_READ:
            break;

        case SPI_XFER_WRITE:
            if( spi_transfer.len < 1 ) {
                printf("You must provide a buffer to execute a \"write\" transfer.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            printf("Starting a write on channel %d with a len of %d bytes.\n", spi_transfer.channel, spi_transfer.len);
            spi_write(&spi_transfer);
            printf("%d bytes written over spi.\n", spi_transfer.bytesTranferred);
            break;

        case SPI_XFER_READ_WRITE:
            break;

        default:
            printf("No transfer type given (-x).\n");
            retVal = -1;
            goto SPI_CLEANUP;
    }

SPI_CLEANUP:
    return retVal;
}