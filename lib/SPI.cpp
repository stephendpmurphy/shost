#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <system_error>
#include "xfer.h"
#include "libMPSSE_spi.h"
#include "SPI.h"

SPI::SPI(): Protocol("SPI") {

}

void SPI::_write(xfer_t *xfer) {
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
        throw std::system_error(ENODEV, std::generic_category(), "No MPSSE channels available.");
    }
    else if( xfer->channel > (channels - 1) ) {
        // TODO this string mess in untested and not so pretty.
        std::string err = "Invalid channel. Only";
        err += std::to_string(channels);
        err += " channel(s) available.";
        throw std::system_error(ENODEV, std::generic_category(), err);
    }
    status = SPI_GetChannelInfo(xfer->channel, &devList);
    APP_CHECK_STATUS(status);

    status = SPI_OpenChannel(xfer->channel, &devList.ftHandle);
    APP_CHECK_STATUS(status);

    status = SPI_InitChannel(devList.ftHandle, &channelConf);
    APP_CHECK_STATUS(status);

    status = SPI_Write(devList.ftHandle, (uint8 *)&xfer->buff, xfer->len,
                       reinterpret_cast<uint32 *>(&xfer->bytesTranferred),
                       SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
                       SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
                       SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE );
    APP_CHECK_STATUS(status);
}

void SPI::_read(xfer_t *xfer) {
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
        throw std::system_error(ENODEV, std::generic_category(), "No MPSSE channels available.");
    }
    else if( xfer->channel > (channels - 1) ) {
        // TODO this string mess in untested and not so pretty.
        std::string err = "Invalid channel. Only";
        err += std::to_string(channels);
        err += " channel(s) available.";
        throw std::system_error(ENODEV, std::generic_category(), err);
    }

    status = SPI_GetChannelInfo(xfer->channel, &devList);
    APP_CHECK_STATUS(status);

    status = SPI_OpenChannel(xfer->channel, &devList.ftHandle);
    APP_CHECK_STATUS(status);

    status = SPI_InitChannel(devList.ftHandle, &channelConf);
    APP_CHECK_STATUS(status);

    status = SPI_Read(devList.ftHandle, (uint8 *)&xfer->buff, xfer->len,
                      reinterpret_cast<uint32 *>(&xfer->bytesTranferred),
                      SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE );
    APP_CHECK_STATUS(status);

}
