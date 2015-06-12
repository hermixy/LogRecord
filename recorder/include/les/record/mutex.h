#ifndef __LES__MUTEX__BY__MACHUNLEI__H__
#define __LES__MUTEX__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif
#include LES_HEADER(type_define.h)

#if   defined LES_MUTEX_DEPENDENCE_FILE_1
#	include LES_MUTEX_DEPENDENCE_FILE1
#elif defined LES_MUTEX_DEPENDENCE_FILE_2
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#elif defined LES_MUTEX_DEPENDENCE_FILE_3
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#elif defined LES_MUTEX_DEPENDENCE_FILE_4
#   include LES_MUTEX_DEPENDENCE_FILE1
#   include LES_MUTEX_DEPENDENCE_FILE2
#   include LES_MUTEX_DEPENDENCE_FILE3
#   include LES_MUTEX_DEPENDENCE_FILE4
#elif defined LES_MUTEX_DEPENDENCE_FILE_5
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#elif defined LES_MUTEX_DEPENDENCE_FILE_6
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#	include LES_MUTEX_DEPENDENCE_FILE6
#elif defined LES_MUTEX_DEPENDENCE_FILE_7
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#	include LES_MUTEX_DEPENDENCE_FILE6
#	include LES_MUTEX_DEPENDENCE_FILE7
#elif defined LES_MUTEX_DEPENDENCE_FILE_8
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#	include LES_MUTEX_DEPENDENCE_FILE6
#	include LES_MUTEX_DEPENDENCE_FILE7
#	include LES_MUTEX_DEPENDENCE_FILE8
#elif defined LES_MUTEX_DEPENDENCE_FILE_9
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#	include LES_MUTEX_DEPENDENCE_FILE6
#	include LES_MUTEX_DEPENDENCE_FILE7
#	include LES_MUTEX_DEPENDENCE_FILE8
#	include LES_MUTEX_DEPENDENCE_FILE9
#elif defined LES_MUTEX_DEPENDENCE_FILE_10
#	include LES_MUTEX_DEPENDENCE_FILE1
#	include LES_MUTEX_DEPENDENCE_FILE2
#	include LES_MUTEX_DEPENDENCE_FILE3
#	include LES_MUTEX_DEPENDENCE_FILE4
#	include LES_MUTEX_DEPENDENCE_FILE5
#	include LES_MUTEX_DEPENDENCE_FILE6
#	include LES_MUTEX_DEPENDENCE_FILE7
#	include LES_MUTEX_DEPENDENCE_FILE8
#	include LES_MUTEX_DEPENDENCE_FILE9
#	include LES_MUTEX_DEPENDENCE_FILE10
#endif


#define LES_MUTEX_PRIORITY  10
#define LES_MUTEX_EMPTY 1

static inline LES_Status LES_CreateMutex(LES_MutexId* mutex, int priority, int value){
    if(ACOREOS_SUCCESSFUL != ACoreOs_semaphore_create(NULL,
            value,
            ACOREOS_BINARY_SEMAPHORE | ACOREOS_PRIORITY,
            (ACoreOs_task_priority)priority,
            mutex))
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}
static inline LES_Status LES_MutexDestroy(LES_MutexId* mutex){
    if(mutex == NULL)
        return LES_Status_NotSuccessful;
    if(ACOREOS_SUCCESSFUL != ACoreOs_semaphore_delete(*mutex))
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}
static inline LES_Status LES_MutexRelease(LES_MutexId* mutex){
    if(mutex == NULL)
        return LES_Status_NotSuccessful;
    if(ACOREOS_SUCCESSFUL != ACoreOs_semaphore_release(*mutex))
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}
static inline LES_Status LES_MutexObtain(LES_MutexId* mutex){
    if(mutex == NULL)
        return LES_Status_NotSuccessful;
    if(ACOREOS_SUCCESSFUL != ACoreOs_semaphore_obtain(
                *mutex,
                ACOREOS_WAIT,
                ACOREOS_FOREVER))
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}
#ifdef __cplusplus
}
#endif

#endif
