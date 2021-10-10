#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpsse-cli.h"
#include "libMPSSE_spi.h"

static void printfArray(uint8 *buff, uint16 len) {
    printf("Array dump: ");
    for(int i = 0; i < len; i++) {
        printf("0x%02X ", buff[i]);
    }

    printf("\n");
}

static int spi_read(arg_t *xfer) {
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

static int spi_write(arg_t *xfer) {
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
int spi_processCmd(arg_t* arg) {
    int retVal = -1;

    switch( arg->xferType ) {
        case XFER_READ:
            if( arg->len < 1 ) {

                printf("You must provide a len to execute a \"read\" transfer.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            printf("Starting a READ on channel %d with a len of %d bytes at %dHz.\n", arg->channel, arg->len, arg->clk);
            retVal = spi_read(arg);
            printf("%d bytes read over spi.\n", arg->bytesTranferred);
            printfArray(arg->buff, arg->bytesTranferred);
            break;

        case XFER_WRITE:
            if( arg->len < 1 ) {
                printf("You must provide a buffer to execute a \"write\" transfer.\n");
                retVal = -1;
                goto SPI_CLEANUP;
            }
            printf("Starting a WRITE on channel %d with a len of %d bytes at %dHz.\n", arg->channel, arg->len, arg->clk);
            retVal = spi_write(arg);
            printf("%d bytes written over spi.\n", arg->bytesTranferred);
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