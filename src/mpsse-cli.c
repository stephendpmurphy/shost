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
	if( fp == NULL ) {
		printf("fp is null\n");
		return -1;
	}

	// Read out any resulst from running the lsmod command
	fgets(modRes, sizeof(modRes), fp);

	// Check if the result has any characters
	if( strlen(modRes) == 0) {
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


int main() {
    FT_STATUS status = FT_OK;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    ChannelConfig channelConf = {0};
    uint8 address = 0;
    uint32 channels = 0;
    uint16 data = 0;
    uint8 i = 0;
    uint8 latency = 255;

    channelConf.ClockRate = 5000;
    channelConf.LatencyTimer = latency;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3; // | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x00000000;                                                     /*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    // Check if the FTDI serial module is loaded. If so, remove it. (This currently requires sudo)
    if (checkIfFtdiModuleLoaded() > 0) {
        removeFtdiModule();
    }

    /* init library */
#ifdef _MSC_VER
    Init_libMPSSE();
#endif

    status = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(status);
    printf("Number of available SPI channels = %d\n", (int)channels);

    if (channels > 0) {
        for (i = 0; i < channels; i++) {
            status = SPI_GetChannelInfo(i, &devList);
            APP_CHECK_STATUS(status);
            printf("Information on channel number %d:\n", i);
            /* print the dev info */
            printf("		Flags=0x%x\n", devList.Flags);
            printf("		Type=0x%x\n", devList.Type);
            printf("		ID=0x%x\n", devList.ID);
            printf("		LocId=0x%x\n", devList.LocId);
            printf("		SerialNumber=%s\n", devList.SerialNumber);
            printf("		Description=%s\n", devList.Description);
            printf("		ftHandle=0x%x\n", (unsigned int)devList.ftHandle); /*is 0 unless open*/
        }
    }

#ifdef _MSC_VER
    Cleanup_libMPSSE();
#endif
    return 0;
}