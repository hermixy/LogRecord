#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(ringbuffer_slave.h)
#include LES_HEADER(user_interface.h)
#include LES_HEADER(ringbuffer_manager.h)
#include LES_HEADER(ringbuffer_operation.h)
#include LES_HEADER(buffer_description.h)
#include LES_HEADER(type_define.h)
#include LES_HEADER(memory_manager.h)

#include LES_HEADER(mutex.h)
#include LES_HEADER(task_manage.h)
#include LES_HEADER(atomic.h)
#include LES_HEADER(lock.h)

extern LES_RingBufferManagePtr LES_ringBufferManager;
extern BOOL LES_ringBufferManageInstalled;
LES_SpinLockType    LES_ringBufferMessageSpinLock;
BOOL LES_ringBufferFullNotify = FALSE;

#define LES_ringBufferManagerStackSize LES_RINGBUFFER_MANAGER_STACKSIZE
LES_Size LES_ringBufferManagerStack[LES_RINGBUFFER_MANAGER_STACKSIZE];

#ifdef LES_CONFIG_SMP
int     LES_ringBufferMessageSpinLockRet;
#else
int     LES_ringBufferMessageLockRet;
#endif



#ifdef LES_DEBUG
#	define LES_RingBufferSendMessage(msgType, ringBuffer, args)   do{\
			LES_LogMessage("LES_RingBufferSendMessage: will increase the nest level(%d)!\n", (ringBuffer)->nestLevel));	\
			if(LES_AtomicInc(&(ringBuffer)->nestLevel)  == 0){\
				BOOL doIt = TRUE;\
				LES_LogMessage("LES_RingBufferSendMessage: before increase the value is 0, but now is %d!\n", (ringBuffer)->nestLevel);	\
				if(msgType == LES_RINGBUFFER_MESSAGE_ADD){\
					LES_LogMessage("LES_RingBufferSendMessage: the send message is: \
							LES_RINGBUFFER_MESSAGE_ADD, and the msgOutstanding value is: %d!\n"\
							,(ringBuffer)->msgOutstanding);	\
					if((ringBuffer)->msgOutstanding){\
						doIt = FALSE;\
						LES_LogMessage("LES_RingBufferSendMessage: has another \
								LES_RINGBUFFER_MESSAGE_ADD is being deal!\n");	\
					}\
					else\
						(ringBuffer)->msgOutstanding = TRUE;\
				}\
				LES_LogMessage("LES_RingBufferSendMessage: function!\n");	\
				if(doIt){\
					LES_LogMessage("LES_RingBufferSendMessage: will send message!\n");	\
					LES_RingBufferManagePtr ringBufferManager =	\
					(LES_RingBufferManagePtr)((ringBuffer)->bufferManagerPtr);\
					LES_RingBufferMessageQueueLock(); \
					LES_LogMessage("prepare to write message to message queue\n");	\
					if(ringBufferManager == NULL){\
						LES_LogMessage("ring buffer manager is null pointer!\n");\
						LES_RingBufferMessageQueueUnlock(); \
						LES_AtomicDec(&(ringBuffer)->nestLevel);\
						break;	\
					}\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].slave = ringBuffer;\
					LES_LogMessage("has write message to message queue\n");	\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].arg = (void*)args;\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].messageType = msgType;\
					if(++ ringBufferManager->messageWriteIndex >= LES_RINGBUFFER_MESSAGE_SIZE)  \
						ringBufferManager->messageWriteIndex = 0;\
					LES_RingBufferMessageQueueUnlock(); \
				}\
			}\
			LES_AtomicDec(&(ringBuffer)->nestLevel);\
		}while(0)
#else
#	define LES_RingBufferSendMessage(msgType, ringBuffer, args)   do{\
			if(LES_AtomicInc(&(ringBuffer)->nestLevel)  == 0){\
				BOOL doIt = TRUE;\
				if(msgType == LES_RINGBUFFER_MESSAGE_ADD){\
					if((ringBuffer)->msgOutstanding){\
						doIt = FALSE;\
					}\
					(ringBuffer)->msgOutstanding = TRUE;\
				}\
				if(doIt){\
					LES_RingBufferManagePtr ringBufferManager = (LES_RingBufferManagePtr)((ringBuffer)->bufferManagerPtr);\
					LES_RingBufferMessageQueueLock(); \
					if(ringBufferManager == NULL){\
						LES_LogMessage("ring buffer manager is null pointer!\n");\
						LES_RingBufferMessageQueueUnlock(); \
						LES_AtomicDec(&(ringBuffer)->nestLevel);\
						break;	\
					}\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].slave = ringBuffer;\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].arg = (void*)args;\
					ringBufferManager->message[ringBufferManager->messageWriteIndex].messageType = msgType;\
					if(++ ringBufferManager->messageWriteIndex >= LES_RINGBUFFER_MESSAGE_SIZE)  \
						ringBufferManager->messageWriteIndex = 0;\
					LES_RingBufferMessageQueueUnlock(); \
				}\
			}\
			LES_AtomicDec(&(ringBuffer)->nestLevel);\
		}while(0)
#endif

static inline void LES_MARK_WRITE_BUFFER_FULL(LES_RingBufferSlavePtr buffer){
    buffer->bufferWrite->manager.spaceAvail = 0;
}

static inline void LES_MARK_WRITE_BUFFER_EMPTY(LES_RingBufferSlavePtr buffer){
    buffer->bufferWrite->manager.spaceAvail = buffer->bufferInfo.bufferSize;
    buffer->bufferWrite->manager.dataLen = 0;
    buffer->bufferWrite->manager.uncommitedRead = FALSE;
}

static inline void LES_MARK_READ_BUFFER_EMPTY(LES_RingBufferSlavePtr buffer){
    buffer->bufferRead->manager.spaceAvail = buffer->bufferInfo.bufferSize;
    buffer->bufferRead->manager.dataLen = 0;
    buffer->bufferRead->manager.uncommitedRead = FALSE;
}


LES_BufferDescriptionPtr LES_RingBufferCreate(LES_BufferConfigTypePtr params){
    LES_RingBufferSlavePtr ringBuffer;
    LES_Counter count;
    LES_RingBufferEntryPtr newBuffer;
    if(params == NULL)
        return NULL;
    if(! LES_ringBufferManageInstalled && (LES_RingBufferManageInit() == LES_Status_Successful))
        LES_ringBufferManageInstalled = TRUE;

    if(! LES_ringBufferManageInstalled){
#ifdef LES_DEBUG
        LES_LogMessage("LES_RingBuffer: LES_RingBufferManageInit is not initialized\n");
#endif
        return NULL;
    }

    if(params->mininum < 2 || (params->mininum > params->maxinum) ||
            (params->maxinum < 2) ||
            (params->bufferSize == 0)){
#ifdef LES_DEBUG
        LES_LogMessage("LES_RingBuffer: invalid create parameters\n");
#endif
        return NULL;
    }
    ringBuffer = (LES_RingBufferSlavePtr)LES_MemoryMalloc(sizeof(LES_RingBufferSlave));
    if(ringBuffer == NULL){
#ifdef LES_DEBUG
        LES_LogMessage("LES_RingBuffer: Allocate for slave error\n");
#endif
        return NULL;
    }

    ringBuffer->bufferInfo.bufferSize = params->bufferSize;
    ringBuffer->bufferInfo.minBufferNums = params->mininum;
    ringBuffer->bufferInfo.maxBufferNums = params->maxinum;
    ringBuffer->bufferInfo.threshold = params->threshold;
    /* NOTICE 最终决定两个自旋锁分开,用于提高效率 */
#ifdef LES_CONFIG_SMP
    LES_SpinLockInit(&ringBuffer->bufferDest.spinLock);
#else
    ringBuffer->bufferDest.lockRet = 0;
#endif
    ringBuffer->bufferDest.lockRtn = (LES_FunctionPtr)LES_RingBufferLock; /* 需要实现的锁函数 */
    ringBuffer->bufferDest.unlockRtn = (LES_FunctionPtr)LES_RingBufferUnlock;

    ringBuffer->numCallbacks = 0;
    ringBuffer->inCallback = 0;
    ringBuffer->bufferManagerPtr = NULL;
    /*ringBuffer->bufferDest.newBufferHookAdd = LES_RingBufferNewBufferHookAdd;*/
    /* 其中LES_ringBufferManager在LES_RingBufferManageInit中初始化的 */
#	if defined LES_DEBUG
    	LES_LogMessage("LES_RingBufferCreate: buffer manager initial to (%x)\n",
    			(unsigned int)LES_ringBufferManager);
#	endif
    ringBuffer->bufferManagerPtr = (void*)LES_ringBufferManager;

    if(LES_CreateMutex(&ringBuffer->bufferDest.thresXSem, 0, 0) == LES_Status_NotSuccessful){
#ifdef LES_DEBUG
        LES_LogMessage("LES_RingBuffer: LES_MutexInit for ringBuffer->bufferDest.thresXSem error\n");
#endif
        LES_MemoryFree(ringBuffer);
        return NULL;
    }

    ringBuffer->bufferInfo.currBufferNums =
        ringBuffer->bufferInfo.emptyBufferNums = 0;
    ringBuffer->dataRead = NULL;
    ringBuffer->dataWrite = NULL;
    ringBuffer->bufferWrite = ringBuffer->bufferRead = NULL;
    for(count = 0; count < params->mininum; count ++){
        newBuffer = LES_RingBufferEntryCreate(ringBuffer);
        if(newBuffer == NULL){
#ifdef LES_DEBUG
            LES_LogMessage("LES_RingBuffer: Allocate for LES_RingBufferEntry error\n");
#endif
            LES_RingBufferDestroy((LES_BufferDescriptionPtr)ringBuffer);
            return NULL;
        }
        LES_RingBufferInsert(ringBuffer, newBuffer);
    }

    if(LES_ringBufferManager->taskId == 0){
        /* 创建管理记录任务 */
        if(LES_Status_Successful !=  LES_TaskCreate(&LES_ringBufferManager->taskId, "LES_RingBufferManager", LES_ringBufferManager->taskPriority,
                (void*)LES_ringBufferManagerStack, LES_ringBufferManagerStackSize,
                LES_TASKATTR_DEFAULT,
                LES_RingBufferManagerTask, (void*)LES_ringBufferManager)){
#if     defined LES_DEBUG
            LES_LogMessage("LES_RingBuffer: create LES_RingBufferManagerTask error\n");
#endif
            LES_RingBufferDestroy((LES_BufferDescriptionPtr)ringBuffer);
            return NULL;
        }
    }
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferCreate: create LES_RingBufferManager task successful, task id is (%d)\n",
                LES_ringBufferManager->taskId);
#   endif


    if(LES_ringBufferManager->taskId == 0){
#ifdef LES_DEBUG
        LES_LogMessage("LES_RingBuffer: Create Ring Buffer Manager Task error\n");
#endif
        LES_RingBufferDestroy((LES_BufferDescriptionPtr)ringBuffer);
        return NULL;
    }

    LES_AtomicSet(&ringBuffer->nestLevel, 0);
    /* 下面的初始化需要完善 */
    ringBuffer->bufferRead= ringBuffer->bufferWrite;
    ringBuffer->dataRead = ringBuffer->dataWrite = (LES_BufferDataPtr)ringBuffer->bufferWrite->data;

    ringBuffer->bufferInfo.maxBufferActual = params->mininum;
    ringBuffer->bufferInfo.dataContent =
        ringBuffer->bufferInfo.writeSinceReset =
        ringBuffer->bufferInfo.readSinceReset = 0;

    ringBuffer->msgOutstanding = FALSE;

    ringBuffer->bufferDest.readReserveRtn = (LES_FunctionPtr)LES_RingBufferReadReserve;
    ringBuffer->bufferDest.readCommitRtn =  (LES_FunctionPtr)LES_RingBufferReadCommit;
    ringBuffer->bufferDest.writeRtn =       (LES_FunctionPtr)LES_RingBufferWrite;
    ringBuffer->bufferDest.flushRtn =       (LES_FunctionPtr)LES_RingBufferFlush;
    ringBuffer->bufferDest.threshold = params->threshold;
    ringBuffer->bufferDest.availableReadRtn = (LES_FunctionPtr)LES_RingBufferAvailableData;

#ifdef LES_DEBUG
    LES_LogMessage("LES_RingBufferCreate: successfully Create Ring Buffer at %x\n", (unsigned int)ringBuffer);
#endif

    return (LES_BufferDescriptionPtr)ringBuffer;
}

LES_RingBufferEntryPtr LES_RingBufferEntryCreate(LES_RingBufferSlavePtr desc){
    LES_RingBufferEntryPtr buffer = (LES_RingBufferEntryPtr)LES_MemoryMalloc(sizeof(LES_RingBufferEntry) + desc->bufferInfo.bufferSize);
    if(buffer != NULL){
        buffer->manager.spaceAvail = desc->bufferInfo.bufferSize;
        buffer->manager.dataLen = 0;
        buffer->manager.prev = NULL;
        buffer->manager.next = NULL;
        buffer->manager.dataEnd = buffer->data + desc->bufferInfo.bufferSize;
        buffer->manager.uncommitedRead = FALSE;
    }
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferEntryCreate: successfully create ringbuffer entry at (%x)\n",
                (unsigned int)buffer);
#   endif
    return buffer;
}

void LES_RingBufferLock(LES_BufferDescriptionPtr desc){
    (void)desc;
#ifdef LES_CONFIG_SMP
    desc->spinLockRet = LES_SpinLock(&desc->spinLock);
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferLock: ready to lock(spinLock)\n");
#   endif
#else
    desc->lockRet = LES_IntLock();
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferLock: ready to lock(normalLock)\n");
#   endif
#endif
}

void LES_RingBufferUnlock(LES_BufferDescriptionPtr desc){
    (void)desc;
#ifdef LES_CONFIG_SMP
    LES_SpinUnlock(&desc->spinLock, desc->spinLockRet);
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferLock: ready to unlock(spinLock)\n");
#   endif
#else
    LES_IntUnlock(desc->lockRet);
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferLock: ready to unlock(normalLock)\n");
#   endif
#endif
}

/*LES_Status LES_RingBufferNewBufferHookAdd(LES_BufferDescriptionPtr desc,
        LES_FunctionPtr func){
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
    if(func != NULL && ringBuffer->numCallbacks < LES_NEWBUFFER_CALLBACK_NUMS){
        ringBuffer->callBackList[ringBuffer->numCallbacks ++] = func;
        return LES_Status_Successful;
    }
    return LES_Status_NotSuccessful;
}*/

LES_Status LES_RingBufferInsert(LES_RingBufferSlavePtr ringBuffer,
        LES_RingBufferEntryPtr entry){
    if(NULL == ringBuffer || NULL == entry)
        return LES_Status_InvalidParam;
    if(ringBuffer->bufferWrite != NULL){
        LES_RingBufferEntryPtr tempPtr;
#       if defined LES_DEBUG_ERROR
            LES_LogMessage("LES_RingBufferInsert: some buffer entry has in ringbuffer, now insert one\n");
#       endif
        tempPtr = ringBuffer->bufferWrite->manager.next;
        ringBuffer->bufferWrite->manager.next = entry;

        entry->manager.next = tempPtr;
        entry->manager.prev = ringBuffer->bufferWrite;
        entry->manager.next->manager.prev = entry;
    }
    else{
#       if defined LES_DEBUG_ERROR
            LES_LogMessage("LES_RingBufferInsert: no buffer entry in ringbuffer, now insert one\n");
#       endif
        ringBuffer->bufferWrite = ringBuffer->bufferRead =
            entry->manager.prev = entry->manager.next =
            entry;
    }

    if(++(ringBuffer->bufferInfo.maxBufferActual) > ringBuffer->bufferInfo.maxBufferNums){
#       if defined LES_DEBUG_ERROR
            LES_LogMessage("LES_RingBufferInsert: buffer entry nums beyond the max buffer entry nums, now update max buffer entry nums\n");
#       endif
        ringBuffer->bufferInfo.maxBufferNums ++;
    }
    ringBuffer->bufferInfo.emptyBufferNums ++;
#   if defined LES_DEBUG_ERROR
        LES_LogMessage("LES_RingBufferInsert: now the empty buffer nums is (%d)\n",
                ringBuffer->bufferInfo.emptyBufferNums);
#   endif
    return LES_Status_Successful;
}

LES_Status LES_RingBufferDestroy(LES_BufferDescriptionPtr desc){
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
    /* 必须保证没有其他的任务正在操作缓冲区,否则后面会出错 */
    ringBuffer->bufferInfo.bufferSize = 0;
    if(ringBuffer->bufferWrite != NULL){
        while(ringBuffer->bufferWrite->manager.next !=
                ringBuffer->bufferWrite){
            LES_RingBufferEntryPtr delPtr =
                ringBuffer->bufferWrite->manager.next;
            ringBuffer->bufferWrite->manager.next =
                ringBuffer->bufferWrite->manager.next->manager.next;
            LES_MemoryFree((void*)delPtr);
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferDestroy: free buffer entry at (%x)\n", delPtr);
#           endif
        }
        /*NOTICE 还需要删除最后一个实体 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferDestroy: delete last entry at (%x)\n", ringBuffer->bufferWrite);
#       endif
        LES_MemoryFree((void*)ringBuffer->bufferWrite);
    }

    if(ringBuffer->bufferManagerPtr != NULL)
        LES_MemoryFree(ringBuffer->bufferManagerPtr);
    LES_MutexDestroy(&ringBuffer->bufferDest.thresXSem);
    LES_MemoryFree((void*)ringBuffer);
    ringBuffer = NULL;

    return LES_Status_Successful;
}

LES_Counter LES_RingBufferReadReserve(LES_BufferDescriptionPtr desc,
        LES_BufferDataPtrPtr dataPtr){
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
    LES_Counter bytesCanRead = 0;
    LES_RingBufferLock(desc); /* 锁住环形缓冲区 */

    /*读取该块数据区可读取的数据量*/
    bytesCanRead = ringBuffer->bufferRead->manager.dataLen;
    if(bytesCanRead != 0){
        *dataPtr = ringBuffer->dataRead;
        /* 表示数据还没有提交 */
        ringBuffer->bufferRead->manager.uncommitedRead = TRUE;
    }
    else
        *dataPtr = NULL;

    LES_RingBufferUnlock(desc); /* 对缓冲区解锁 */
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferReadReserve: ringbuffer has (%d) bytes data can read\n", bytesCanRead);
#   endif
    return bytesCanRead;
}


LES_Status LES_RingBufferReadCommit(LES_BufferDescriptionPtr desc,
        LES_Counter numOfBytes){
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferReadCommit: ready to commit data size is (%d)\n", numOfBytes);
#   endif

    LES_RingBufferLock(desc); /* 锁住环形缓冲区 */

    ringBuffer->bufferRead->manager.uncommitedRead = FALSE; /* 表明该部分数据已经上传 */
    if(numOfBytes == 0){
        LES_RingBufferUnlock(desc); /* 对缓冲区解锁 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferReadCommit: commit data size is (%d)\n", numOfBytes);
#       endif
        return LES_Status_Successful;
    }
    if(numOfBytes == ringBuffer->bufferRead->manager.dataLen){
        /*说明一旦读取之后,这个缓冲区就是空的了, 需要改变这个缓冲区的属性*/
#		ifdef LES_DEBUG_ERROR
    		LES_LogMessage("LES_RingBufferReadCommit: will free a block\n");
#		endif
    	/* 需要修订读缓冲区和空闲块的个数 */
        LES_RingBufferHandleEmpty(ringBuffer);
    }
    else if(numOfBytes < ringBuffer->bufferRead->manager.dataLen){
        /* 移动读指针,调整到合适的位置 */
#		ifdef LES_DEBUG_ERROR
    		LES_LogMessage("LES_RingBufferReadCommit: forward the pointer\n");
#		endif
        ringBuffer->bufferRead->manager.dataLen -= numOfBytes;
        ringBuffer->dataRead += numOfBytes;
    }
    else{
        LES_RingBufferUnlock(desc); /* 对缓冲区解锁 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferReadCommit: will return LES_Status_NotImplement\n");
#       endif
        return LES_Status_NotImplement;
    }

    ringBuffer->bufferInfo.dataContent -= numOfBytes;
    ringBuffer->bufferInfo.readSinceReset ++;

    LES_RingBufferUnlock(desc); /* 对缓冲区解锁 */
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferReadCommit: commit data size is (%d)\n", numOfBytes);
#   endif
    return LES_Status_Successful;
}

LES_BufferDataPtr LES_RingBufferWrite(LES_BufferDescriptionPtr desc,
        LES_Counter numOfBytes){
    LES_BufferDataPtr dataPtr = NULL;
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferWrite: ready to write data size is (%d)\n", numOfBytes);
#   endif
    if(numOfBytes > ringBuffer->bufferInfo.bufferSize){
        /* 写入的数据超过了一个缓冲区的最大长度 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferWrite: write data size(%d) is bigger than buffer entry size(%d)\n",
                    numOfBytes, ringBuffer->bufferInfo.bufferSize);
#       endif
        return NULL;
    }

    /*if(ringBuffer->bufferWrite->manager.dataLen < ringBuffer->bufferInfo.threshold &&
            (ringBuffer->bufferWrite->manager.dataLen + numOfBytes >= ringBuffer->bufferInfo.threshold))*/
    if(ringBuffer->bufferInfo.dataContent < ringBuffer->bufferInfo.threshold &&
    	    (ringBuffer->bufferInfo.dataContent + numOfBytes >= ringBuffer->bufferInfo.threshold))
        if(!(ringBuffer->bufferInfo.options & LES_RINGBUFFER_UPLOAD_DEFER)){
        /* 如果写入这个数据之后,长度会超过阈值,发送消息给环形缓冲区管理任务 */
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferWrite: after write, the ringbuffer readable data size is beyond threshold\n");
                LES_LogMessage("LES_RingBufferWrite: after write, a message of LES_RINGBUFFER_MESSAGE_THRESHOLD will be send\n");
#           endif
            LES_RingBufferSendMessage(LES_RINGBUFFER_MESSAGE_THRESHOLD, ringBuffer, 0);
#       	if defined LES_DEBUG
            	LES_LogMessage("LES_RingBufferWrite: has send the LES_RINGBUFFER_MESSAGE_THRESHOLD message\n");
#       	endif
        }


    if((LES_Counter)ringBuffer->bufferWrite->manager.spaceAvail < numOfBytes){
        /* 如果该块缓冲区剩余小于要写入的数据 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferWrite: avaliable space(%d) is less than data size(%d) be written\n",
                    ringBuffer->bufferWrite->manager.spaceAvail, numOfBytes);
            LES_LogMessage("LES_RingBufferWrite: will try to malloc some more memory\n");
#       endif
        if(LES_RingBufferHandleEntryOverflow(ringBuffer) !=
                LES_Status_Successful){
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferWrite: no memory can alloc now\n");
#           endif
            return NULL;
        }
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferWrite: the buffer write space avail is: %d\n",
              ringBuffer->bufferWrite->manager.spaceAvail);
#       endif
    }

    dataPtr = ringBuffer->dataWrite;
    /*省略hook的调用*/

    ringBuffer->bufferWrite->manager.dataLen += numOfBytes;
    ringBuffer->dataWrite += numOfBytes;
    ringBuffer->bufferWrite->manager.spaceAvail -= numOfBytes;

    ringBuffer->bufferInfo.dataContent += numOfBytes;
    /* 可以去掉 */
    ringBuffer->bufferInfo.writeSinceReset ++;

#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferWrite: write data size is (%d) successful\n", numOfBytes);
#   endif
    return dataPtr;
}

LES_Status LES_RingBufferFlush(LES_BufferDescriptionPtr desc){
    LES_RingBufferSlavePtr ringBuffer;
    LES_Counter oldThreshold;
    LES_Status ret = LES_Status_Successful;

    ringBuffer = (LES_RingBufferSlavePtr)desc;
    if(! ringBuffer->bufferInfo.dataContent){
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferFlush: no data in ringbuffer\n");
#       endif
        return LES_Status_Successful;
    }

    oldThreshold = ringBuffer->bufferInfo.threshold;
    ringBuffer->bufferInfo.threshold = 0;
    while(ringBuffer->bufferInfo.dataContent > 0 && ret != LES_Status_NotSuccessful){
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferFlush: will flush data size is (%d)\n",
                    ringBuffer->bufferInfo.dataContent);
#       endif
        if(LES_MutexRelease(&ringBuffer->bufferDest.thresXSem) == LES_Status_Successful){
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferFlush: has release the threshold semaphore\n");
#           endif
            LES_TaskDelay(0);
        }
        else
            ret = LES_Status_NotSuccessful;
    }

    ringBuffer->bufferInfo.threshold = oldThreshold;
    return ret;
}

LES_Counter LES_RingBufferAvailableData(LES_BufferDescriptionPtr desc){
    LES_RingBufferSlavePtr ringBuffer = (LES_RingBufferSlavePtr)desc;
    return ringBuffer->bufferInfo.dataContent;
}

LES_Status LES_RingBufferHandleEmpty(LES_RingBufferSlavePtr ringBuffer){
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferHandleEmpty: handle empty called\n");
#   endif
    LES_RingBufferEntryPtr buffer2Free = ringBuffer->bufferRead;
    if(ringBuffer->bufferRead != ringBuffer->bufferWrite){
    	/*如果两个指针没有相遇，说明还有一部分数据没有读取到， 那么移动到下一个可读取的块中*/
        ringBuffer->bufferRead = ringBuffer->bufferRead->manager.next;
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferHandleEmpty: forward buffer reader to next buffer\n");
#       endif
    }
    else{
    	/*说明这块缓冲区数据刚好写满，而且也被读完，现在可以直接将写指针移动到块的开头，读指针也移动到块的开头*/
        LES_MARK_READ_BUFFER_EMPTY(ringBuffer);
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferHandleEmpty: read all data, and the buffer reader and writer meet\n");
#       endif
        /* 此时不需要移动读取指针 */
        ringBuffer->dataWrite = (LES_BufferDataPtr)ringBuffer->bufferWrite->data;
        buffer2Free = ringBuffer->bufferWrite->manager.next;
    }

    ringBuffer->dataRead = (LES_BufferDataPtr)ringBuffer->bufferRead->data;
    /* 已经回收了一个缓冲块 */
    ringBuffer->bufferInfo.emptyBufferNums ++;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferHandleEmpty: recycle a buffer now the empty buffer numbers is (%d)\n",
                ringBuffer->bufferInfo.emptyBufferNums);
#   endif

    if(ringBuffer->bufferInfo.emptyBufferNums > LES_RINGBUFFER_KEEP &&
            ringBuffer->bufferInfo.currBufferNums > ringBuffer->bufferInfo.minBufferNums){
    	/* 如果空闲缓冲区大于规定可保留的空闲缓冲区，并且目前缓冲区的个数已经超过了最少要求的个数，可以进行缓冲区的回收 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferHandleEmpty: empty buffer nums(%d) beyond the LES_RINGBUFFER_KEEP(%d), meanwhile"\
                    " it also exceed the min(%d) buffer nums configured\n",
                    ringBuffer->bufferInfo.emptyBufferNums,
                    LES_RINGBUFFER_KEEP,
                    ringBuffer->bufferInfo.minBufferNums);
#       endif
        ringBuffer->bufferInfo.currBufferNums --;
        ringBuffer->bufferInfo.emptyBufferNums --;

        buffer2Free->manager.next->manager.prev = buffer2Free->manager.prev;
        buffer2Free->manager.prev->manager.next = buffer2Free->manager.next;

#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferHandleEmpty: send LES_RINGBUFFER_MESSAGE_FREE message\n");
#   endif
        LES_RingBufferSendMessage(LES_RINGBUFFER_MESSAGE_FREE, ringBuffer, buffer2Free);
    }
    return LES_Status_Successful;
}

LES_Status LES_RingBufferHandleEntryOverflow(LES_RingBufferSlavePtr ringBuffer){
    /* 由于这块数据太小,无法写入要求的数据了 */
    LES_MARK_WRITE_BUFFER_FULL(ringBuffer); /* 标示该缓冲区实体满
    同时也说明这块数据不能继续存放数据了*/
    if(ringBuffer->bufferInfo.emptyBufferNums <= 3 &&
            ringBuffer->bufferInfo.currBufferNums < ringBuffer->bufferInfo.maxBufferNums){
        /* 只有剩下一个缓冲区,然而还没有到达上界,那么可以继续分配缓冲区,这是提前分配 */
#       if defined LES_DEBUG_ERROR
            LES_LogMessage("LES_RingBufferHandleEntryOverflow: now we just left only one empty buffer, but we can alloc some(%d)\n",
                    ringBuffer->bufferInfo.maxBufferNums - ringBuffer->bufferInfo.currBufferNums);
            LES_LogMessage("LES_RingBufferHandleEntryOverflow: send LES_RINGBUFFER_MESSAGE_ADD message\n");
#       endif
        LES_RingBufferSendMessage(LES_RINGBUFFER_MESSAGE_ADD, ringBuffer, 0);
    }
    else if(ringBuffer->bufferWrite->manager.next == ringBuffer->bufferRead){
        /* 如果没有缓冲区可用,那么需要回收,如果回收失败,那么将会失败 */
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferHandleEntryOverflow: no empty buffer left, we will try recycle some\n");
#       endif
        /* 尝试回收缓冲区块 */
        if(LES_RingBufferRecycle(ringBuffer) == LES_Status_NotSuccessful){
#           if defined LES_DEBUG
                LES_LogMessage("LES_RingBufferHandleEntryOverflow: recycle not find some more available empty buffer\n");
                LES_LogMessage("LES_RingBufferHandleEntryOverflow: send LES_RINGBUFFER_MESSAGE_FULL message\n");
#           endif
            LES_RingBufferSendMessage(LES_RINGBUFFER_MESSAGE_FULL, ringBuffer, 0);
            LES_ringBufferFullNotify = TRUE;
            return LES_Status_NotSuccessful;
        }
        ringBuffer->bufferInfo.emptyBufferNums ++;
    }
    /* 执行到下面,说明至少有一个缓冲区是空的 */
    ringBuffer->bufferInfo.emptyBufferNums --;
#   if defined LES_DEBUG_ERROR
        LES_LogMessage("LES_RingBufferHandleEntryOverflow: now the empty buffer nums is (%d)\n",
                ringBuffer->bufferInfo.emptyBufferNums);
#   endif
    /* 修订写指针位置 */
    ringBuffer->bufferWrite = ringBuffer->bufferWrite->manager.next;
    LES_MARK_WRITE_BUFFER_EMPTY(ringBuffer);
    ringBuffer->dataWrite = (LES_BufferDataPtr)ringBuffer->bufferWrite->data;
#   if defined LES_DEBUG
        LES_LogMessage("LES_RingBufferHandleEntryOverflow: buffer write begin address is: (%x)\n",
                (int)ringBuffer->bufferWrite);
#   endif
    return LES_Status_Successful;
}

LES_Status LES_RingBufferRecycle(LES_RingBufferSlavePtr ringBuffer){
    LES_RingBufferEntryPtr readBuffer = ringBuffer->bufferRead;

    if((ringBuffer->bufferInfo.options & LES_RINGBUFFER_WRAPAROUND) == 0){
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferRecycle: we have configure the buffer option without LES_RINGBUFFER_WRAPAROUND\n");
#       endif
        return LES_Status_NotSuccessful;
    }
    /* NOTE: 如果缓冲区读取位置还存在没有提交的数据或者读取的位置不是该缓冲区实体的数据开始位置,
     * 由于该缓冲区不支持一个实体内既存在读数据又存在写数据(没有变量保存该实体中可写长度和可读长度) */
    if(readBuffer->manager.uncommitedRead != FALSE ||
            ringBuffer->dataRead != (LES_BufferDataPtr)readBuffer->data){
#       if defined LES_DEBUG
            LES_LogMessage("LES_RingBufferRecycle: some condition can not satisfy, can not recycle buffer\n");
#       endif
        return LES_Status_NotSuccessful;
    }
    /* 下面,覆盖整个缓冲区实体 */
#   if defined LES_DEBUG_ERROR
        LES_LogMessage("LES_RingBufferRecycle: recycle a buffer for write, but at the cost of sacrificing readable(%d) data\n",
                ringBuffer->bufferRead->manager.dataLen);
#   endif
    /* 这里会丢失数据,如果执行到这里,日志必丢失 */
    ringBuffer->bufferInfo.dataContent -= ringBuffer->bufferRead->manager.dataLen;

    ringBuffer->bufferRead = ringBuffer->bufferRead->manager.next;
    ringBuffer->dataRead = (LES_BufferDataPtr)ringBuffer->bufferRead->data;
    return LES_Status_Successful;
}

LES_RingBufferManagePtr LES_RingBufferManagerGet(LES_RingBufferSlavePtr ringBuffer){
    if(ringBuffer == NULL)
        return NULL;
    return (LES_RingBufferManagePtr)ringBuffer->bufferManagerPtr;
}
