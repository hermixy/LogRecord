#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(user_interface.h)
#include LES_HEADER(buffer_description.h)
#include LES_HEADER(log_upload.h)
#include LES_HEADER(ringbuffer_operation.h)
#include LES_HEADER(atomic.h)
#include LES_HEADER(task_manage.h)

/* 引入公共区 */
void LES_MakeSureCommonDataRegionAccessble();

/*FIXME later*/
#define LES_EVENT_CPU_ID (0x4C4553 + 600)
LES_BufferConfigType LES_bufferParameters = {
    .mininum = 60,
    .maxinum = 164,
    .threshold = 1024 * 8,
    .bufferSize = 1024 * 4
};

int writeRtn(LES_LogUploadDescriptionPtr upload, char* data, int size){
	//int i = 0;
	//printk_spinLock("LES_LogUploadDescription->writeRtn write data size is:(%d)\n", size);
	//while(i < size){
	//	printk_spinLock("LES_LogUploadDescription->writeRtn write data is:(%c)\n", *(data + i));
	//	i ++;
	//}
	/*const int everySendNums = 100;
	int hasSend = 0;
	int everyHasSend = 0;
	while(size > 0){
		everyHasSend = agtLogData(data + hasSend, everySendNums < size ?
				everySendNums : size);
		hasSend += everyHasSend;
		size -= everyHasSend;
	}
	return hasSend;*/
	int key;
	int ret;
	disableCpuInterrupt(&key);
    ret= agtLogData(data, size);
    enableCpuInterrupt(key);
    return ret;
	//return size;
}

LES_Status LES_EventLogStop();
LES_BufferDescriptionPtr LES_bufferDescriptionPointer;
LES_LogUploadDescription LES_logUploadDescription = {
    .writeRtn = writeRtn, /* FIXME 使用管道写函数代替，后续相同 */
    .errorRtn = NULL,
    .wdbPipe = NULL
};


LES_LogUploadTaskDescriptionPtr LES_logUploadTaskDescriptionPointer;

#define LES_logUploadStackSize  LES_LOG_UPLOAD_STACKSIZE
LES_Size LES_logUploadTaskStack[LES_LOG_UPLOAD_STACKSIZE];
LES_TaskPriority LES_logUploadTaskPriority = LES_LOG_UPLOAD_TASKPRIORITY;

unsigned short LES_currentCpuId = -1;

LES_Atomic LES_LogOpen = LES_LOG_OFF;
LES_Atomic LES_RecordReplay = LES_RECORD_PHASE;

#define LES_logOpen	LES_LogOpen
#define LES_recoredReplay	LES_RecordReplay

#define LES_CRIT_REGION_ENTER() \
    LES_bufferDescriptionPointer->lockRtn(LES_bufferDescriptionPointer);
#define LES_CRIT_REGION_EXIT()  \
    LES_bufferDescriptionPointer->unlockRtn(LES_bufferDescriptionPointer);	\

#define LES_LOG_RESERVE_BUFFER_SPACE(size)  \
    eventBase = (LES_EventId*)LES_bufferDescriptionPointer->writeRtn(\
            LES_bufferDescriptionPointer, size);  \
    if(!eventBase){ \
        LES_EventLogStop();   \
        LES_LogMessage("LES_LOG_RESERVE_BUFFER_SPACE: malloc data for record is error\n"); \
        LES_CRIT_REGION_EXIT(); \
        return; \
    }

#ifdef LES_CONFIG_SMP
#   define LES_SIZEOF_CPU_EVENT (sizeof(LES_EventId) + sizeof(int) + sizeof(int))
#   define LES_CPU_EVENT_EMIT(cpuId)   \
       do{\
           LES_EVENT_STORE_UINT32(eventBase, LES_EVENT_CPU_ID);    \
           timestamp ++;								\
           LES_EVENT_STORE_UINT32(eventBase, timestamp);    \
           LES_EVENT_STORE_UINT32(eventBase, cpuId);   \
       }while(0)
#else
#   define LES_SIZEOF_CPU_EVENT 0
#   define LES_CPU_EVENT_EMIT(cpuId)
#endif

#ifdef LES_CONFIG_SMP
#   define LES_CONDITION_CPU_EMIT(allocator, size) \
       do {    \
           int cpuId = LES_CpuIndexGet();  \
           if(cpuId != LES_currentCpuId){\
               allocator(size + LES_SIZEOF_CPU_EVENT); \
               LES_CPU_EVENT_EMIT((unsigned int)cpuId);   \
               LES_currentCpuId = cpuId;    \
           }   \
           else\
               allocator(size); \
       } while(0)
#else
#   define LES_CONDITION_CPU_EMIT(allocator, size)  \
        allocator(size)
#endif


#define LES_EVENT_STORE_UINT16(buffer, value)   \
    *(((short*)buffer) ++) = (value)

#if (LES_BYTE_ORDER == LES_BIG_ENDIAN)
#   if defined LES_UNALIGNED_SUPPORT
#       define LES_EVENT_STORE_UINT32(buffer, value)    \
            *buffer ++ = (value)
#   else
#       define LES_EVENT_STORE_UINT32(buffer, value)    \
        do{\
            *((short*)buffer) = (value) >> 16;  \
            *(((short*)buffer) + 1) = (value);  \
            buffer ++;  \
        }while(0)
#   endif
#else
#   if defined LES_UNALIGNED_SUPPORT
#       define LES_EVENT_STORE_UINT32(buffer, value)    \
            *buffer ++ = (value)
#   else
#       define LES_EVENT_STORE_UINT32(buffer, value)    \
        do{\
            *((short*)buffer) = (value);  \
            *(((short*)buffer) + 1) = (value) >> 16;  \
            buffer ++;  \
        }while(0)
#   endif
#endif

LES_BufferDescriptionPtr LES_RingBufferCreate(LES_BufferConfigTypePtr);

/* 主机端调用，开启日志记录函数　*/
ACoreOs_status_code LES_LogOn(){
	/* 引用一下动态区，否则无法在链接时候加入进去 */
	LES_MakeSureCommonDataRegionAccessble();
    /* 创建缓冲区 */
    LES_bufferDescriptionPointer = LES_RingBufferCreate(&LES_bufferParameters);
    /* 创建日志管理对象 */
    LES_LogManagePtr logManager = LES_LogManageCreate(LES_bufferDescriptionPointer);
    /* 开启日志记录，这里采用原子操作 */

    /* 启动上传任务 */
    LES_logUploadTaskDescriptionPointer = LES_LogUploadStart(logManager, &LES_logUploadDescription, TRUE);
    if(LES_TaskReturnCode_Successful != LES_TaskCreate(&LES_logUploadTaskDescriptionPointer->uploadTaskId,
            "LES_LogUploadTask", LES_logUploadTaskPriority,
            (void*)LES_logUploadTaskStack,
            LES_logUploadStackSize,
            LES_TASKATTR_DEFAULT,
            LES_LogUploadTask, LES_logUploadTaskDescriptionPointer)){
#ifdef LES_DEBUG
        LES_LogMessage("LES_LogOn: spawn task for LES_LogUploadTask error\n");
#endif
        /*停止任务*/
        LES_EventLogStop();
        return ACOREOS_NOT_ALL_SUCCESSFUL;
    }
    LES_AtomicSet(&LES_logOpen, LES_LOG_ON);
        LES_LogMessage("LES_LogOn: LES_logOpen's value = %d\n", LES_logOpen);
    return ACOREOS_SUCCESSFUL;
}

/* 主机端调用，关闭日志记录函数　*/
ACoreOs_status_code LES_LogOff(){
    /* 该函数需要完成的事:
     * 1. 让记录条件不成立
     * 2. 上传所有数据后, 暂停上传任务和缓冲区管理任务
     * 3. 不回收内存,方便下次开启记录的快捷 */
    LES_EventLogStop();
}

/* 如果以后修改了LES_EventId类型的大小,需要调整对eventId的保存过程 */
static int timestamp = 0;
void LES_EventLogHandle(unsigned int eventId, const void* data, int size){
    LES_EventId* eventBase;
    /*int timestamp = 0;*/
    int* intBase;
    if(data == NULL)
        return;
    if(size < 0 || size > 1000)
        return;
#ifdef LES_DEBUG
    LES_LogMessage("LES_EventLogHandle: recv event id is (%d), data address is (%d), data size is (%d) and the cpu index = %d\n",
    		eventId, (LES_Counter)data, size, LES_CpuIndexGet());
#endif
    LES_CRIT_REGION_ENTER(); /* 对缓冲区上锁 */

    LES_CONDITION_CPU_EMIT(LES_LOG_RESERVE_BUFFER_SPACE,
            size + sizeof(LES_EventId) + sizeof(int));
    LES_EVENT_STORE_UINT32(eventBase, eventId);

    intBase = (int*)eventBase;
    timestamp ++;
    LES_EVENT_STORE_UINT32(intBase, timestamp);
    memcpy((void*)intBase, data, size);

    LES_CRIT_REGION_EXIT();
}

LES_Status LES_EventLogStop(){
    LES_AtomicAnd(&LES_logOpen, ~LES_LOG_ON);
    /* TODO 是否需要暂停上传任务 */
    return LES_Status_Successful;
}
