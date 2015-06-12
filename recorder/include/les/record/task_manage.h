#ifndef __LES__TASK__MANAGE__BY__MACHUNLEI__H__
#define __LES__TASK__MANAGE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)
#if   defined LES_TASK_DEPENDENCE_FILE_1
#	include LES_TASK_DEPENDENCE_FILE1
#elif defined LES_TASK_DEPENDENCE_FILE_2
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#elif defined LES_TASK_DEPENDENCE_FILE_3
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#elif defined LES_TASK_DEPENDENCE_FILE_4
#   include LES_TASK_DEPENDENCE_FILE1
#   include LES_TASK_DEPENDENCE_FILE2
#   include LES_TASK_DEPENDENCE_FILE3
#   include LES_TASK_DEPENDENCE_FILE4
#elif defined LES_TASK_DEPENDENCE_FILE_5
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#elif defined LES_TASK_DEPENDENCE_FILE_6
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#	include LES_TASK_DEPENDENCE_FILE6
#elif defined LES_TASK_DEPENDENCE_FILE_7
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#	include LES_TASK_DEPENDENCE_FILE6
#	include LES_TASK_DEPENDENCE_FILE7
#elif defined LES_TASK_DEPENDENCE_FILE_8
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#	include LES_TASK_DEPENDENCE_FILE6
#	include LES_TASK_DEPENDENCE_FILE7
#	include LES_TASK_DEPENDENCE_FILE8
#elif defined LES_TASK_DEPENDENCE_FILE_9
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#	include LES_TASK_DEPENDENCE_FILE6
#	include LES_TASK_DEPENDENCE_FILE7
#	include LES_TASK_DEPENDENCE_FILE8
#	include LES_TASK_DEPENDENCE_FILE9
#elif defined LES_TASK_DEPENDENCE_FILE_10
#	include LES_TASK_DEPENDENCE_FILE1
#	include LES_TASK_DEPENDENCE_FILE2
#	include LES_TASK_DEPENDENCE_FILE3
#	include LES_TASK_DEPENDENCE_FILE4
#	include LES_TASK_DEPENDENCE_FILE5
#	include LES_TASK_DEPENDENCE_FILE6
#	include LES_TASK_DEPENDENCE_FILE7
#	include LES_TASK_DEPENDENCE_FILE8
#	include LES_TASK_DEPENDENCE_FILE9
#	include LES_TASK_DEPENDENCE_FILE10
#endif

typedef LES_TaskReturnCode (*LES_TaskEntry)(void*);
static inline LES_Status LES_TaskCreate(LES_TaskId* id, const char* name, LES_TaskPriority priority,
        void* stack, unsigned int size, unsigned int option, LES_TaskEntry entry, void* param){
    if(id == NULL)
        return LES_Status_NotSuccessful;
    if(ACOREOS_SUCCESSFUL != ACoreOs_task_create(name,
                priority, stack, size, option, NULL, id
            ))
        return LES_Status_NotSuccessful;
    if(ACOREOS_SUCCESSFUL != ACoreOs_task_start(*id,
                (ACoreOs_task_entry)entry, (UINT32)param
            ))
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}
static inline LES_Status LES_TaskDelay(int ticks){
    ACoreOs_task_wake_after(ticks);
    return LES_Status_Successful;
}

static inline LES_CpuIndexGet(){
	return sysCoreIndexGet();
}

#ifdef __cplusplus
}
#endif
#endif
