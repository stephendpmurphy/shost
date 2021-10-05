#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpsse-cli.h"
#include "libMPSSE_spi.h"

#define NUM_SPI_OPTIONS 6

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
        .cmd_short = "-f",
        .cmd_full = "--freq",
        .desc = "SPI Frequency",
        .cb = NULL
    },
    {
        .cmd_short = "-x",
        .cmd_full = "--xfer",
        .desc = "Set transfer type. [r | w | rw]",
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
};

static void printfArray(uint8 *buff, uint16 len) {
    printf("Array dump: ");
    for(int i = 0; i < len; i++) {
        printf("0x%02X ", buff[i]);
    }

    printf("\n");
}

static int8 spi_read(xfer_t *xfer) {
    FT_STATUS status = FT_OK;
    ChannelConfig channelConf = {0};
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    uint32 channels = 0;

    channelConf.ClockRate = xfer->clk;
    channelConf.LatencyTimer = 255;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    // Make sure we even have enough channels
    status = SPI_GetNumChannels(&channels);

    if( channels <= 0 ) {
        printf("No MPSSE channels available.\n");
        return -1;
    }
    else if( xfer->channel > (channels - 1) ) {
        printf("Invalid channel. Only %d channel(s) available.\n", channels);
        return -1;
    }

    status = SPI_GetChannelInfo(xfer->channel, &devList);
    APP_CHECK_STATUS(status);

    status = SPI_OpenChannel(xfer->channel, &devList.ftHandle);
    APP_CHECK_STATUS(status);

    status = SPI_InitChannel(devList.ftHandle, &channelConf);
    APP_CHECK_STATUS(status);

    status = SPI_Read(devList.ftHandle, (uint8 *)&xfer->buff, xfer->len, &xfer->bytesTranferred,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE );
    APP_CHECK_STATUS(status);

    return 0;
}

static int8 spi_write(xfer_t *xfer) {
    FT_STATUS status = FT_OK;
    ChannelConfig channelConf = {0};
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    uint32 channels = 0;

    channelConf.ClockRate = xfer->clk;
    channelConf.LatencyTimer = 255;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    // Make sure we even have enough channels
    status = SPI_GetNumChannels(&channels);

    if( channels <= 0 ) {
        printf("No MPSSE channels available.\n");
        return -1;
    }
    else if( xfer->channel > (channels - 1) ) {
        printf("Invalid channel. Only %d channel(s) available.\n", channels);
        return -1;
    }

    status = SPI_GetChannelInfo(xfer->channel, &devList);
    APP_CHECK_STATUS(status);

    status = SPI_OpenChannel(xfer->channel, &devList.ftHandle);
    APP_CHECK_STATUS(status);

    status = SPI_InitChannel(devList.ftHandle, &channelConf);
    APP_CHECK_STATUS(status);

    status = SPI_Write(devList.ftHandle, (uint8 *)&xfer->buff, xfer->len, &xfer->bytesTranferred,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE );
    APP_CHECK_STATUS(status);

    return 0;
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
    xfer_t spi_transfer = {0x00};

    spi_transfer.channel = 0;
    spi_transfer.clk = 100000;

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
        else if( (strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--freq") == 0)) {
            if( (i+1) >= argc ) {
                printf("No frequency given. Defaulting to 100kb/s.\n");
                spi_transfer.clk = 100000;
            }
            else {
                spi_transfer.clk = strtol(argv[i+1], NULL, 10);
            }
        }
        else if( (strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--len") == 0)) {
            if( (i+1) >= argc ) {
                printf("-l option found, but no value given.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            else {
                spi_transfer.len = strtol(argv[i+1], NULL, 10);
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
                spi_transfer.xferType = XFER_READ;
            }
            else if( (strcmp(argv[i+1], "w") == 0) ) {
                spi_transfer.xferType = XFER_WRITE;
            }
            else if( (strcmp(argv[i+1], "rw") == 0) ) {
                spi_transfer.xferType = XFER_READ_WRITE;
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
        case XFER_READ:
            if( spi_transfer.len < 1 ) {

                printf("You must provide a len to execute a \"read\" transfer.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            printf("Starting a READ on channel %d with a len of %d bytes at %dHz.\n", spi_transfer.channel, spi_transfer.len, spi_transfer.clk);
            retVal = spi_read(&spi_transfer);
            printf("%d bytes read over spi.\n", spi_transfer.bytesTranferred);
            printfArray(spi_transfer.buff, spi_transfer.bytesTranferred);
            break;

        case XFER_WRITE:
            if( spi_transfer.len < 1 ) {
                printf("You must provide a buffer to execute a \"write\" transfer.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            printf("Starting a WRITE on channel %d with a len of %d bytes at %dHz.\n", spi_transfer.channel, spi_transfer.len, spi_transfer.clk);
            retVal = spi_write(&spi_transfer);
            printf("%d bytes written over spi.\n", spi_transfer.bytesTranferred);
            break;

        case XFER_READ_WRITE:
            break;

        default:
            printf("No transfer type given (-x).\n");
            retVal = -1;
            goto SPI_CLEANUP;
    }

SPI_CLEANUP:
    return retVal;
}