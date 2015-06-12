#ifndef __BUFFER_DESCRIPTION__BY__MACHUNLEI__H__
#define __BUFFER_DESCRIPTION__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)

typedef struct{
    LES_FunctionPtr     readReserveRtn;
    LES_FunctionPtr     readCommitRtn;
    LES_FunctionPtr     writeRtn;
    LES_FunctionPtr     availableReadRtn;
    LES_FunctionPtr     flushRtn;
    LES_FunctionPtr     lockRtn;
    LES_FunctionPtr     unlockRtn;
#ifdef LES_CONFIG_SMP
    LES_SpinLockType    spinLock; /* 为多核提供的自旋锁　*/
    int                 spinLockRet;
#else
    int                 lockRet;
#endif


    /* ACoreOs_status_code (*newBufferHookAdd)(this*, ACoreOs_status_code (*callback)(this*)) */
    /*LES_FunctionPtr     newBufferHookAdd;*/
    LES_MutexId         thresXSem;
    LES_Counter         threshold;
}LES_BufferDescription, *LES_BufferDescriptionPtr;

#ifdef __cplusplus
}
#endif

#endif
