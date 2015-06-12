#ifndef __LES__RINGBUFFER__SLAVE__BY__MACHUNLEI__H__
#define __LES__RINGBUFFER__SLAVE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)
#include LES_HEADER(buffer_description.h)
#include LES_HEADER(ringbuffer_define.h)

typedef struct{
    LES_BufferDescription   bufferDest;
    LES_RingBufferEntryPtr  bufferRead;
    LES_RingBufferEntryPtr  bufferWrite;
    LES_BufferDataPtr       dataRead;
    LES_BufferDataPtr       dataWrite;
    LES_WdbPipeId           pipeId;
    LES_FunctionPtr         errorCallBack;
    LES_MutexId             readMutex;
    LES_MutexId             bufferFull;
    LES_Atomic              nestLevel;
    LES_RingBufferInformation   bufferInfo;
    /* 自己管理的消息结构的锁，不确定是否应该需要 */

    void*                   bufferManagerPtr;
    unsigned int            msgOutstanding;

    int                     numCallbacks;
    /*LES_Status (*callBackList[LES_NEWBUFFER_CALLBACK_NUMS](LES_BufferDescriptionPtr));*/
    int                     inCallback;
}LES_RingBufferSlave, *LES_RingBufferSlavePtr;



#ifdef __cplusplus
}
#endif

#endif
