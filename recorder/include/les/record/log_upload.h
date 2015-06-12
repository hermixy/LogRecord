#ifndef __LES__LOG__UPLOAD__BY__MACHUNLEI__H__
#define __LES__LOG__UPLOAD__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif
#include LES_HEADER(type_define.h)
#include LES_HEADER(log.h)

typedef struct{
    LES_FunctionPtr writeRtn;
    LES_FunctionPtr errorRtn;
    LES_WdbPipeId   wdbPipe;
}LES_LogUploadDescription, *LES_LogUploadDescriptionPtr;

typedef struct{
    LES_TaskId uploadTaskId;
    LES_MutexId compleSem;
    BOOL    exitWhenEmpty;
    LES_LogManagePtr log;
    LES_LogUploadDescriptionPtr upload;
    int status;
}LES_LogUploadTaskDescription, *LES_LogUploadTaskDescriptionPtr;

LES_LogUploadTaskDescriptionPtr LES_LogUploadStart(LES_LogManagePtr, LES_LogUploadDescriptionPtr, BOOL);

LES_TaskReturnCode  LES_LogUploadTask(LES_LogUploadTaskDescriptionPtr);
LES_Status LES_LogUploadBaseInformation(LES_LogManagePtr, LES_LogUploadDescriptionPtr);

#ifdef __cplusplus
}
#endif
#endif
