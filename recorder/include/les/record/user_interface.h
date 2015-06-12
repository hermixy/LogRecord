#ifndef __USER__INTERFACE__BY__MACHUNLEI__H__
#define __USER__INTERFACE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#if   defined LES_STATUS_DEPENDENCE_FILE_1
#	include LES_STATUS_DEPENDENCE_FILE1
#elif defined LES_STATUS_DEPENDENCE_FILE_2
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#elif defined LES_STATUS_DEPENDENCE_FILE_3
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#elif defined LES_STATUS_DEPENDENCE_FILE_4
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#elif defined LES_STATUS_DEPENDENCE_FILE_5
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#elif defined LES_STATUS_DEPENDENCE_FILE_6
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#	include LES_STATUS_DEPENDENCE_FILE6
#elif defined LES_STATUS_DEPENDENCE_FILE_7
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#	include LES_STATUS_DEPENDENCE_FILE6
#	include LES_STATUS_DEPENDENCE_FILE7
#elif defined LES_STATUS_DEPENDENCE_FILE_8
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#	include LES_STATUS_DEPENDENCE_FILE6
#	include LES_STATUS_DEPENDENCE_FILE7
#	include LES_STATUS_DEPENDENCE_FILE8
#elif defined LES_STATUS_DEPENDENCE_FILE_9
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#	include LES_STATUS_DEPENDENCE_FILE6
#	include LES_STATUS_DEPENDENCE_FILE7
#	include LES_STATUS_DEPENDENCE_FILE8
#	include LES_STATUS_DEPENDENCE_FILE9
#elif defined LES_STATUS_DEPENDENCE_FILE_10
#	include LES_STATUS_DEPENDENCE_FILE1
#	include LES_STATUS_DEPENDENCE_FILE2
#	include LES_STATUS_DEPENDENCE_FILE3
#	include LES_STATUS_DEPENDENCE_FILE4
#	include LES_STATUS_DEPENDENCE_FILE5
#	include LES_STATUS_DEPENDENCE_FILE6
#	include LES_STATUS_DEPENDENCE_FILE7
#	include LES_STATUS_DEPENDENCE_FILE8
#	include LES_STATUS_DEPENDENCE_FILE9
#	include LES_STATUS_DEPENDENCE_FILE10
#endif

typedef struct{
    int     mininum;
    int     maxinum;
    int     bufferSize;
    int     threshold;
}LES_BufferConfigType, *LES_BufferConfigTypePtr;

ACoreOs_status_code LES_LogOn();
ACoreOs_status_code LES_LogOff();

void LES_EventLogHandle(unsigned int, const void*, int);


#ifdef __cplusplus
}
#endif

#endif