#ifndef _MPSSE_CLI_H_
#define _MPSSE_CLI_H_

#include "ftd2xx.h"
#include "libMPSSE_spi.h"

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

// Create a function pointer type to use for cmd callbacks.
typedef int8(*cli_cmd_callback_fp)(int argc, char *argv[]);

typedef struct {
    char cmd_short[64];
    char cmd_full[64];
    char desc[256];
    cli_cmd_callback_fp cb;
} cli_cmd_t;

int8 checkIfArgIsOption(char *arg);

#endif // _MPSSE_CLI_H_