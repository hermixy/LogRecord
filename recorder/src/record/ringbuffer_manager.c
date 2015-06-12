#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(ringbuffer_manager.h)
#include LES_HEADER(ringbuffer_operation.h)
#include LES_HEADER(memory_manager.h)
#include LES_HEADER(mutex.h)
#include LES_HEADER(lock.h)
#include LES_HEADER(task_manage.h)
#include LES_HEADER(atomic.h)

BOOL LES_ringBufferManageInstalled = FALSE;

LES_RingBufferManagePtr LES_ringBufferManager = NULL;

int LES_ringBufferPollingInterval = LES_RINGBUFFER_POLLING_INTERVAL;


#ifdef LES_CONFIG_SMP
LES_SpinLockType LES_ringBufferMessageQueueSpinLock;
int LES_ringBufferMessageSpinLockRet;
#else
int LES_ringBufferMessageLockRet;
#endif

void LES_RingBufferMessageQueueLock(){
#ifdef LES_CONFIG_SMP
    LES_ringBufferMessageSpinLockRet = LES_SpinLock(&LES_ringBufferMessageQueueSpinLock);
#else
    LES_ringBufferMessageLockRet = LES_IntLock();
#endif
}

void LES_RingBufferMessageQueueUnlock(){
#ifdef LES_CONFIG_SMP
    LES_SpinUnlock(&LES_ringBufferMessageQueueSpinLock, LES_ringBufferMessageSpinLockRet);
#else
    LES_IntUnlock(LES_ringBufferMessageLockRet);
#endif
}

LES_Status LES_RingBufferManageInit(void){
    if(LES_ringBufferManageInstalled)
        return LES_Status_Successful;

    if((LES_ringBufferManager = (LES_RingBufferManagePtr)LES_MemoryMalloc(sizeof(LES_RingBufferManage))) == NULL)
        return LES_Status_NoMemory;
    LES_ringBufferManager->taskId = 0;
    /* FIXME 这里的数字最后需要替换成宏 */
    LES_ringBufferManager->taskPriority = LES_RINGBUFFER_MANAGER_TASKPRIORITY;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferManageInit: initialize the ringbuffer manager task priority to (%d)\n",
                LES_RINGBUFFER_MANAGER_TASKPRIORITY);
#   endif
    LES_ringBufferManager->messageReadIndex =
        LES_ringBufferManager->messageWriteIndex = 0;
#if defined LES_CONFIG_SMP
    LES_SpinLockInit(&LES_ringBufferMessageQueueSpinLock);
#endif

    LES_ringBufferManageInstalled = TRUE;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferManageInit: initialize successfully\n");
#   endif
    return LES_Status_Successful;

}

LES_TaskReturnCode  LES_RingBufferManagerTask(LES_RingBufferManagePtr manager){
    LES_RingBufferEntryPtr newBuffer;
    LES_RingBufferManageMessage message;
    BOOL    messagePresent;
#ifdef LES_DEBUG
    LES_LogMessage("LES_RingBufferManagerTask: ringbuffer manager task create\n");
#endif

    while(1){
        messagePresent = FALSE;
        LES_RingBufferMessageQueueLock();

        if(manager->messageReadIndex != manager->messageWriteIndex){
            /* 从消息队列中获取消息 */
            messagePresent = TRUE;
            memcpy((void*)&message, (const void*)&manager->message[
                    manager->messageReadIndex], sizeof(message));
            if(++manager->messageReadIndex >= LES_RINGBUFFER_MESSAGE_MAX)
                manager->messageReadIndex = 0;
        }
        LES_RingBufferMessageQueueUnlock();

        if(! messagePresent){
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferManagerTask: no message recv, will sleep for (%d) ticks\n",
                        LES_ringBufferPollingInterval);
#           endif
            LES_TaskDelay(LES_ringBufferPollingInterval);
        }
        else{
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferManagerTask: read message, message is (%d)\n",
                        message.messageType);
#           endif

            switch(message.messageType){
                case LES_RINGBUFFER_MESSAGE_ADD:
                    /* TODO */
#                   if defined LES_DEBUG_ERROR
                        LES_LogMessage("LES_RingBufferManagerTask: recv LES_RINGBUFFER_MESSAGE_ADD message\n");
#                   endif
                    LES_AtomicInc(&message.slave->nestLevel);
                    newBuffer = LES_RingBufferEntryCreate(message.slave);
                    if(newBuffer != NULL){
                        LES_RingBufferLock((LES_BufferDescriptionPtr)message.slave);
                        LES_RingBufferInsert(message.slave, newBuffer);
                        LES_RingBufferUnlock((LES_BufferDescriptionPtr)message.slave);
                    }
                    LES_AtomicDec(&message.slave->nestLevel);
                    break;
                case LES_RINGBUFFER_MESSAGE_FULL:
#                   if defined LES_DEBUG_ERROR
                        LES_LogMessage("LES_RingBufferManagerTask: recv LES_RINGBUFFER_MESSAGE_FULL message\n");
#                   endif
                    if(message.slave->errorCallBack != NULL)
                        (*message.slave->errorCallBack)(LES_RINGBUFFER_ERROR_FULL);
                    break;
                case LES_RINGBUFFER_MESSAGE_FREE:
#                   if defined LES_DEBUG_ERROR
                        LES_LogMessage("LES_RingBufferManagerTask: recv LES_RINGBUFFER_MESSAGE_FREE message\n");
#                   endif
                    LES_MemoryFree((void*)message.arg);
                    break;
                case LES_RINGBUFFER_MESSAGE_THRESHOLD:
#                   if defined LES_DEBUG_ERROR
                        LES_LogMessage("LES_RingBufferManagerTask: recv LES_RINGBUFFER_MESSAGE_THRESHOLD message\n");
#                   endif
                    /*应该先判断是否真的有必要*/
                    if(message.slave->bufferInfo.dataContent < message.slave->bufferInfo.threshold){
                    	/*没有必要唤醒*/
#                   	if defined LES_DEBUG_ERROR
                        	LES_LogMessage("LES_RingBufferManagerTask: no need to release mutex\n");
#                   	endif
                    	break;
                    }
                    /* 通知日志上传任务开始上传 */
                    LES_MutexRelease(&message.slave->bufferDest.thresXSem);
                    break;
                default:
#                   if defined LES_DEBUG_ERROR
                        LES_LogMessage("LES_RingBufferManagerTask: recv UNKnown message\n");
#                   endif
                    break;
            }
            message.slave->msgOutstanding = FALSE;
        }
    }
    return LES_TaskReturnCode_Successful;
}
