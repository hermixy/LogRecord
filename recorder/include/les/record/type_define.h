#ifndef __LES__TYPE__DEFINE__BY__MACHUNLEI__H__
#define __LES__TYPE__DEFINE__BY__MACHUNLEI__H__
#include <sys/types.h>

#if   defined LES_TYPEDEFINE_DEPENDENCE_FILE_1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_6
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE6
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_7
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE6
#	include LES_TYPEDEFINE_DEPENDENCE_FILE7
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_8
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE6
#	include LES_TYPEDEFINE_DEPENDENCE_FILE7
#	include LES_TYPEDEFINE_DEPENDENCE_FILE8
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_9
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE6
#	include LES_TYPEDEFINE_DEPENDENCE_FILE7
#	include LES_TYPEDEFINE_DEPENDENCE_FILE8
#	include LES_TYPEDEFINE_DEPENDENCE_FILE9
#elif defined LES_TYPEDEFINE_DEPENDENCE_FILE_10
#	include LES_TYPEDEFINE_DEPENDENCE_FILE1
#	include LES_TYPEDEFINE_DEPENDENCE_FILE2
#	include LES_TYPEDEFINE_DEPENDENCE_FILE3
#	include LES_TYPEDEFINE_DEPENDENCE_FILE4
#	include LES_TYPEDEFINE_DEPENDENCE_FILE5
#	include LES_TYPEDEFINE_DEPENDENCE_FILE6
#	include LES_TYPEDEFINE_DEPENDENCE_FILE7
#	include LES_TYPEDEFINE_DEPENDENCE_FILE8
#	include LES_TYPEDEFINE_DEPENDENCE_FILE9
#	include LES_TYPEDEFINE_DEPENDENCE_FILE10
#endif

typedef ACoreOs_id                      LES_MutexId;
typedef size_t                          LES_MessageQueueId;
typedef ACoreOs_id                      LES_TaskId;
typedef size_t                          LES_EventId;
typedef ACoreOs_task_priority           LES_TaskPriority;
typedef int                             LES_Size;
typedef size_t                          LES_Address;
typedef size_t                          LES_Argument;

typedef size_t                          LES_Register;
typedef size_t                          LES_CheckSum;
typedef size_t                          LES_Counter;

typedef size_t                          LES_String[LES_MAXNAMELEN / sizeof(LES_Size)];

/* add for ring buffer */
/* should use kernel struct to redefine it */
typedef spinlockIsrNd_t                 LES_SpinLockType;
typedef LES_SpinLockType*               LES_SpinLockTypePtr;
typedef size_t                          LES_WdbPipeId;


typedef unsigned char                   LES_BufferData;
typedef LES_BufferData*                 LES_BufferDataPtr;
typedef LES_BufferDataPtr*              LES_BufferDataPtrPtr;

typedef enum{
    LES_Status_Successful = 0,
    LES_Status_NoMemory = 1,
    LES_Status_InvalidParam = 2,
    LES_Status_NotSuccessful,
    LES_Status_NotImplement,
    LES_Status_Max,
}LES_Status;

typedef atomic32_t                      LES_Atomic;
typedef LES_Atomic*                     LES_AtomicPtr;
typedef atomic32Val_t                   LES_AtomicVal;

typedef LES_Status (*LES_FunctionPtr)();

typedef enum{
    LES_TaskReturnCode_Successful = 0,
    LES_TaskReturnCode_NotSuccessful,
}LES_TaskReturnCode;


#ifndef BOOL
#define BOOL    unsigned char
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef LES_LOG_ON
#define LES_LOG_ON      1
#endif

#ifndef LES_LOG_OFF
#define LES_LOG_OFF     2
#endif

#ifndef LES_RECORD_PHASE
#define LES_RECORD_PHASE    1
#endif

#endif
