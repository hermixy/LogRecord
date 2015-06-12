#ifndef __LES__RINGBUFFER__MANAGE__BY__MACHUNLEI__H__
#define __LES__RINGBUFFER__MANAGE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(ringbuffer_slave.h)

typedef struct{
    LES_RingBufferSlavePtr  slave;
    unsigned int messageType;
    void*  arg;
}LES_RingBufferManageMessage, *LES_RingBufferManageMessagePtr;

typedef struct{
    LES_TaskId          taskId;
    LES_TaskPriority    taskPriority;
    unsigned int        messageWriteIndex;
    unsigned int        messageReadIndex;
    LES_RingBufferManageMessage message[LES_RINGBUFFER_MESSAGE_SIZE];
}LES_RingBufferManage, *LES_RingBufferManagePtr;

/* 函数申明 */
LES_Status LES_RingBufferManageInit(void);
LES_TaskReturnCode  LES_RingBufferManagerTask(LES_RingBufferManagePtr);
void LES_RingBufferMessageQueueLock();
void LES_RingBufferMessageQueueUnlock();
#ifdef __cplusplus
}
#endif

#endif
