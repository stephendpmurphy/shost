#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "ftd2xx.h"
#include "libMPSSE_spi.h"

#define delete_module(name, flags) syscall(__NR_delete_module, name, flags)

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

int checkIfFtdiModuleLoaded(void) {
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

int removeFtdiModule(void) {
    if (delete_module("ftdi_sio", O_NONBLOCK) != 0) {
        perror("delete_module");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

uint32 getMPSSEchannelCount(void) {
    FT_STATUS ftStatus;
    uint32 numDevs;
    uint32 channels = 0;
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of devices FTDI devices connected: %d\n",numDevs);
    // retrieve the number of devices that are capable of MPSSE
    ftStatus = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of available MPSSE channels: %d\n", (int)channels);
}

int main(int argc, char *argv[] ) {
    FT_STATUS status = FT_OK;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    uint32 channels = 0;

    // Check if the FTDI serial module is loaded. If so, remove it. (This currently requires sudo)
    if (checkIfFtdiModuleLoaded() > 0) {
        removeFtdiModule();
    }

    /* init library */
#ifdef _MSC_VER
    Init_libMPSSE();
#endif

    // Reterieve the number of available MPSSE channels
    channels = getMPSSEchannelCount();

    // Iterate through the channels and print their info
    if (channels > 0) {
        for(uint32 i = 0; i < channels; i++) {
            status = SPI_GetChannelInfo(i, &devList);
            APP_CHECK_STATUS(status);
            printf("Information on channel number %d:\n", i);
            /* print the dev info */
            printf("    Flags=0x%x\n", devList.Flags);
            printf("    Type=0x%x\n", devList.Type);
            printf("    ID=0x%x\n", devList.ID);
            printf("    LocId=0x%x\n", devList.LocId);
            printf("    SerialNumber=%s\n", devList.SerialNumber);
            printf("    Description=%s\n", devList.Description);
            printf("    ftHandle=0x%x\n", (unsigned int)devList.ftHandle); /*is 0 unless open*/
        }
    }

#ifdef _MSC_VER
    Cleanup_libMPSSE();
#endif
    return 0;
}