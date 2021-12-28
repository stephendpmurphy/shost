#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};

#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

int util_isFtdiModuleLoaded(void);
int util_removeFtdiModule(void);
uint32_t util_getMPSSEchannelCount(void);
uint32_t util_printMPSSEchannelInfo(int channels);

#endif // _UTIL_H_