#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "libMPSSE_spi.h"
#include "util.h"

void printfArray(uint8 *buff, uint16 len) {
    printf("Array dump: ");
    for (int i = 0; i < len; i++) {
        printf("0x%02X ", buff[i]);
    }

    printf("\n");
}

int util_isFtdiModuleLoaded(void) {
    FILE *fp;
    char modRes[128] = {0x00};

    // Run the lsmod command and check if the FTDI module is loaded
    fp = popen("lsmod | grep ftdi_sio", "r");

    // Make sure the file pointer isn't null
    if (fp == NULL) {
        printf("fp is null\n");
        return -1;
    }

    // Read out any resulst from running the lsmod command
    fgets(modRes, sizeof(modRes), fp);

    // Check if the result has any characters
    if (strlen(modRes) == 0) {
        // No characters, assume the module isn't loaded then.
        return 0;
    }
    else {
        // Some characters were found. The module must be loaded.
        return 1;
    }
}

int util_removeFtdiModule(void) {
    if(syscall(__NR_delete_module, "ftdi_sio", O_NONBLOCK) != 0) {
        perror("delete_module");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

uint32_t util_getMPSSEchannelCount(void) {
    FT_STATUS ftStatus;
    uint32_t numDevs;
    uint32_t channels = 0;
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of FTDI devices connected: %d\n",numDevs);
    // retrieve the number of devices that are capable of MPSSE
    ftStatus = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of available MPSSE channels: %d\n", (int)channels);

    return channels;
}

uint32_t util_printMPSSEchannelInfo(int channels) {
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    // Iterate through the channels and print their info
    if (channels > 0) {
        for(uint32_t i = 0; i < channels; i++) {
            ftStatus = SPI_GetChannelInfo(i, &devList);
            APP_CHECK_STATUS(ftStatus);
            printf("Information on channel number %d:\n", i);
            /* print the dev info */
            printf("    Flags=0x%x\n", devList.Flags);
            printf("    Type=0x%x\n", devList.Type);
            printf("    ID=0x%x\n", devList.ID);
            printf("    LocId=0x%x\n", devList.LocId);
            printf("    SerialNumber=%s\n", devList.SerialNumber);
            printf("    Description=%s\n", devList.Description);
            // printf("    ftHandle=%d\n", (unsigned int)devList.ftHandle); /*is 0 unless open*/
        }
    }

    return 0;
}