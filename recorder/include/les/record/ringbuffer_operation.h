#ifndef __RINGBUFFER__OPERATION__BY__MACHUNLEI__H__
#define __RINGBUFFER__OPERATION__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(buffer_description.h)
#include LES_HEADER(ringbuffer_slave.h)
#include LES_HEADER(type_define.h)
#include LES_HEADER(ringbuffer_manager.h)

/* 一旦设置该参数,允许缓冲区的覆盖 */
#define LES_RINGBUFFER_WRAPAROUND 0x1


void LES_RingBufferLock(LES_BufferDescriptionPtr);
void LES_RingBufferUnlock(LES_BufferDescriptionPtr);

LES_Status LES_RingBufferNewBufferHook(LES_BufferDescriptionPtr,
        LES_FunctionPtr);

LES_Status LES_RingBufferInsert(LES_RingBufferSlavePtr,
        LES_RingBufferEntryPtr);

LES_RingBufferEntryPtr LES_RingBufferEntryCreate(LES_RingBufferSlavePtr);

LES_Status LES_RingBufferDestroy(LES_BufferDescriptionPtr);

LES_Counter LES_RingBufferReadReserve(LES_BufferDescriptionPtr,
        LES_BufferDataPtrPtr);

LES_Status LES_RingBufferReadCommit(LES_BufferDescriptionPtr,
        LES_Counter);

LES_BufferDataPtr LES_RingBufferWrite(LES_BufferDescriptionPtr,
        LES_Counter);

LES_Status LES_RingBufferFlush(LES_BufferDescriptionPtr);

LES_Counter LES_RingBufferAvailableData(LES_BufferDescriptionPtr);

LES_Status LES_RingBufferHandleEmpty(LES_RingBufferSlavePtr);

LES_Status LES_RingBufferHandleEntryOverflow(LES_RingBufferSlavePtr);

LES_Status LES_RingBufferRecycle(LES_RingBufferSlavePtr);

LES_RingBufferManagePtr LES_RingBufferManagerGet(LES_RingBufferSlavePtr);


#ifdef __cplusplus
}
#endif

#endif
