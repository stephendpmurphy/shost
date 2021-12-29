#ifndef _MPSSE_CLI_H_
#define _MPSSE_CLI_H_

#include "ftd2xx.h"
#include "libMPSSE_spi.h"
#include "cli.h"

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

int CB_printCliVersion(arg_t *arg);
int CB_printFTDIdevices(arg_t *arg);

static void printfArray(uint8 *buff, uint16 len);

#endif // _MPSSE_CLI_H_