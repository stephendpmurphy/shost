#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "ftd2xx.h"
#include "libMPSSE_spi.h"

#define NUM_CLI_OPTIONS 3
#define NUM_CLI_SUBCMDS 1

#define delete_module(name, flags) syscall(__NR_delete_module, name, flags)

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

int8 CB_printCliVersion(int argc, char *argv[]);
int8 CB_printFTDIdevices(int argc, char *argv[]);
int8 CB_printCLIoptions(int argc, char *argv[]);

// Create a function pointer type to use for cmd callbacks.
typedef int8(*cli_cmd_callback_fp)(int argc, char *argv[]);

typedef struct {
    char cmd_short[64];
    char cmd_full[64];
    char desc[128];
    cli_cmd_callback_fp cb;
} cli_cmds_t;

const cli_cmds_t cli_options[NUM_CLI_OPTIONS] = {
    {
        .cmd_short = "-l",
        .cmd_full = "--list",
        .desc = "Displays number of FTDI devices connected, number of MPSSE channels available and available MPSSE channel information.",
        .cb = CB_printFTDIdevices
    },
    {
        .cmd_short = "-v",
        .cmd_full = "--version",
        .desc = "Displays the current cli version.",
        .cb = CB_printCliVersion
    },
        {
        .cmd_short = "-h",
        .cmd_full = "--help",
        .desc = "Displays this help menu.",
        .cb = CB_printCLIoptions
    },

};

const cli_cmds_t cli_subcmds[NUM_CLI_SUBCMDS] = {
    {
        .cmd_short = "",
        .cmd_full = "spi",
        .desc = "Initiate a SPI read or write transfer.",
        .cb = NULL
    }
};

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
    printf("Number of FTDI devices connected: %d\n",numDevs);
    // retrieve the number of devices that are capable of MPSSE
    ftStatus = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(ftStatus);
    printf("Number of available MPSSE channels: %d\n", (int)channels);

    return channels;
}

uint32 printMPSSEchannelInfo(int channels) {
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    // Iterate through the channels and print their info
    if (channels > 0) {
        for(uint32 i = 0; i < channels; i++) {
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
            printf("    ftHandle=0x%x\n", (unsigned int)devList.ftHandle); /*is 0 unless open*/
        }
    }
}

int8 CB_printCliVersion(int argc, char *argv[]) {
    printf("0.1.0\n");
}

int8 CB_printFTDIdevices(int argc, char *argv[]) {
    printMPSSEchannelInfo( getMPSSEchannelCount() );
}

int8 CB_printCLIoptions(int argc, char *argv[]) {
    printf("usage: mpsse-cli [-h] [-v] [-l] SUB-COMMAND\n\n");

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

    if( NUM_CLI_SUBCMDS > 0 )
        printf("\n\nsub-commands:\n");

    for(int i = 0; i < NUM_CLI_SUBCMDS; i++) {
        printf("    ");
        if( strlen(cli_subcmds[i].cmd_short) > 0 )
            printf("%s, ", cli_subcmds[i].cmd_short);

        if( strlen(cli_subcmds[i].cmd_full) > 0 )
            printf("%s", cli_subcmds[i].cmd_full);

        printf(" - %s\n", cli_subcmds[i].desc);
    }
    printf("\n\n");
}

int main(int argc, char *argv[] ) {
    FT_STATUS status = FT_OK;
    uint32 channels = 0;
    int cmd_found = 0;
    int retVal = -1;

    if( argc <= 1 ) {
        printf("Please provide an argument.\n");
        retVal = -1;
        goto CLI_CLEANUP;
    }

    // Check if the FTDI serial module is loaded. If so, remove it.
    // This requires sudo when running after a build. Not required when installed.
    if (checkIfFtdiModuleLoaded() > 0) {
        removeFtdiModule();
    }

    /* init library */
#ifdef _MSC_VER
    Init_libMPSSE();
#endif

    // Process the first cmd option.. It should either be a cli option or a sub-command
    for(int opt_i = 0; opt_i < NUM_CLI_OPTIONS; opt_i++) {
        if( ( (strlen(cli_options[opt_i].cmd_full) > 0) && (strcmp(argv[1], cli_options[opt_i].cmd_full) == 0) ) ||
            ( (strlen(cli_options[opt_i].cmd_short) > 0) && (strcmp(argv[1], cli_options[opt_i].cmd_short) == 0) ) ) {
            // Mark our flag saying we found a matching command.
            cmd_found = 1;
            // The command matched.. Execute the associated callback with that command.
            if( cli_options[opt_i].cb != NULL ) {
                retVal = cli_options[opt_i].cb(0, NULL);
                goto CLI_CLEANUP;
            }
            else {
                printf("No callback associated with that option.\n");
                retVal = -1;
                goto CLI_CLEANUP;
            }
        }
    }

    // Process the first cmd option.. It should either be a cli option or a sub-command
    for(int i = 0; i < NUM_CLI_SUBCMDS; i++) {
        if( ( (strlen(cli_subcmds[i].cmd_full) > 0) && (strcmp(argv[1], cli_subcmds[i].cmd_full) == 0) ) ||
            ( (strlen(cli_subcmds[i].cmd_short) > 0) && (strcmp(argv[1], cli_subcmds[i].cmd_short) == 0) ) ) {
            // Mark our flag saying we found a matching command.
            cmd_found = 1;
            // The command matched.. Execute the associated callback with that command.
            if( cli_subcmds[i].cb != NULL ) {
                retVal = cli_subcmds[i].cb(0, NULL);
                goto CLI_CLEANUP;
            }
            else {
                printf("No callback associated with that sub-command.\n");
                retVal = -1;
                goto CLI_CLEANUP;
            }
        }
    }

    // We made it this far.. Assume no valid argument was given.
    printf("Invalid arguments given.\n");
    retVal = -1;

CLI_CLEANUP:

#ifdef _MSC_VER
    Cleanup_libMPSSE();
#endif

    return retVal;
}