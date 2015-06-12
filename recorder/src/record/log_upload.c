#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(log_upload.h)
#include LES_HEADER(mutex.h)
#include LES_HEADER(memory_manager.h)
#include LES_HEADER(hash_table.h)

LES_Counter LES_logUploadMaxAttempt = LES_LOGUPLOAD_ATTEMPT_MAX; /* 上传重试次数 */

LES_LogUploadTaskDescriptionPtr LES_LogUploadStart(LES_LogManagePtr logManager, LES_LogUploadDescriptionPtr upload, BOOL uploadContinue){
    LES_LogUploadTaskDescriptionPtr uploadTaskDesc;
    if(upload == NULL || logManager == NULL)
        return NULL;
    if((uploadTaskDesc = (LES_LogUploadTaskDescriptionPtr)LES_MemoryMalloc(sizeof(LES_LogUploadTaskDescription))) == NULL){
#ifdef LES_DEBUG
        LES_LogMessage("LES_LogUploadStart: malloc for LES_LogUploadTaskDescriptionPtr error");
#endif
        return NULL;
    }

#   if defined LES_DEBUG
        LES_LogMessage("LES_LogUploadStart: malloc for LES_LogUploadTaskDescription successful at (%x)\n",
                (unsigned int)uploadTaskDesc);
#   endif
    /* 初始化信号量 */
    LES_CreateMutex(&uploadTaskDesc->compleSem, 0, 0);

    uploadTaskDesc->log = logManager;
    uploadTaskDesc->status = LES_Status_Successful;
    uploadTaskDesc->upload = upload;

    if(uploadContinue)
        uploadTaskDesc->exitWhenEmpty = FALSE;
    else
        uploadTaskDesc->exitWhenEmpty = TRUE;

    /* 创建任务 */

    return uploadTaskDesc;
}

LES_Status LES_LogUploadWriteToWdbPipe(LES_LogUploadDescriptionPtr upload, LES_BufferDataPtr data, LES_Size size){
    int trys;
    LES_Size bytesHasWritten;
    LES_Size bytesOfWrite;
    int bytesOfThis;
    LES_BufferDataPtr writePtr;
    writePtr = data;

    bytesOfWrite = size;
    bytesHasWritten = 0;
    trys  = 0;

    if(upload == NULL || upload->writeRtn == NULL)
        return LES_Status_NotSuccessful;

    while(bytesOfWrite > 0 && trys < LES_logUploadMaxAttempt){
        if((bytesOfThis = upload->writeRtn(upload, writePtr, bytesOfWrite)) < 0){
#           if defined LES_DEBUG
                LES_LogMessage("LES_LogUploadWriteToWdbPipe: write data whose size is (%d) into pipe error\n",
                        bytesOfWrite);
#           endif
           ;
        }
        else{
#           if defined LES_DEBUG
                LES_LogMessage("LES_LogUploadWriteToWdbPipe: write (%d) data into pipe successful\n",
                        bytesOfThis);
#           endif
            bytesHasWritten += bytesOfThis;
            bytesOfWrite -= bytesOfThis;
            writePtr += bytesOfThis;
        }
        ++ trys;
    }
    if(bytesHasWritten < size){
#       if defined LES_DEBUG
            LES_LogMessage("LES_LogUploadWriteToWdbPipe: write (%d) data into pipe, but less than (%d) be expected to be written\n",
                    bytesHasWritten, size);
#       endif
        return LES_Status_NotSuccessful;
    }
#   if defined LES_DEBUG
        LES_LogMessage("LES_LogUploadWriteToWdbPipe: write (%d) data into pipe equal (%d) be expected to be written\n",
                bytesHasWritten, size);
#   endif
    return LES_Status_Successful;
}

LES_TaskReturnCode LES_LogUploadTask(LES_LogUploadTaskDescriptionPtr uploadTaskDesc){
    LES_Counter bytesOfWrite;
    LES_Counter bytesOfUpload;
    LES_Counter bytesOfCommit;
    LES_Counter bytesOfAvaliable;
    LES_Counter tmp;
    BOOL bufferEmpty;
    LES_BufferDataPtr data;
    if(uploadTaskDesc == NULL)
        return LES_TaskReturnCode_NotSuccessful;
    if(uploadTaskDesc->log == NULL)
        goto error;
    if(uploadTaskDesc->upload == NULL)
        goto error;
#   define LES_bufferPtr    uploadTaskDesc->log->buffer

    LES_LogUploadBaseInformation(uploadTaskDesc->log, uploadTaskDesc->upload); /* 上传hash里面存放的信息 */
    bytesOfWrite = 0;
    bufferEmpty = FALSE;
    while(1){
        bytesOfUpload = 0;
        bufferEmpty = FALSE;

        /* 读取得到缓冲区中可上传的数据 */
        tmp = LES_bufferPtr->availableReadRtn(LES_bufferPtr);
#       if defined LES_DEBUG_ERROR
            LES_LogMessage("LES_LogUploadTask: now ring buffer has readable data size is (%d)\n", tmp);
#       endif
        while(! uploadTaskDesc->exitWhenEmpty && (LES_bufferPtr->availableReadRtn(LES_bufferPtr) < LES_bufferPtr->threshold)){
        	/* 如果数据没有空闲并且当前的数据又没有到达阈值，那么等待信号量的唤醒 */
#       	if defined LES_DEBUG_ERROR
            	LES_LogMessage("LES_LogUploadTask: Take semaphore before\n");
#       	endif
            LES_MutexObtain(&LES_bufferPtr->thresXSem);
#       	if defined LES_DEBUG_ERROR
            	LES_LogMessage("LES_LogUploadTask: Take semaphore after\n");
#       	endif
        }

        while((bytesOfUpload < LES_bufferPtr->threshold) && !bufferEmpty){
        	/*读取该块缓冲块可读取的数据量*/
            bytesOfAvaliable = LES_bufferPtr->readReserveRtn(LES_bufferPtr, &data);
            if(bytesOfAvaliable == (LES_Counter)-1){
            	/* 如果可用数据个数读取失败， 这个现象一般不会发生 */
#           	if defined LES_DEBUG_ERROR
                	LES_LogMessage("LES_LogUploadTask: read data from ringbuffer error\n");
#           	endif
                LES_EventLogStop();
                goto error;
            }

#           if defined LES_DEBUG_ERROR
                LES_LogMessage("LES_LogUploadTask: read data from ringbuffer successful, "
                		"data size is (%d), data addr is (0x%x)\n",
                        bytesOfAvaliable, (unsigned int)data);
#           endif
            if(bytesOfAvaliable == 0){
            	/*如果目前没有可上传数据*/
                bufferEmpty = TRUE;
            }
            else{
                bufferEmpty = FALSE;
            }

            bytesOfWrite = (bytesOfAvaliable > (LES_bufferPtr->threshold - bytesOfUpload)) ?
                (LES_bufferPtr->threshold - bytesOfUpload) : bytesOfAvaliable;
            bytesOfCommit = bytesOfWrite;
            /* 写入管道 */
#           if defined LES_DEBUG
                LES_LogMessage("LES_LogUploadTask: write data whose size is (%d) to pipe\n",
                        bytesOfCommit);
#           endif
            LES_LogUploadWriteToWdbPipe(uploadTaskDesc->upload, data, bytesOfCommit);
#           if defined LES_DEBUG
                LES_LogMessage("LES_LogUploadTask: commit data whose size is (%d)\n",
                        bytesOfCommit);
#           endif
            tmp = LES_bufferPtr->readCommitRtn(LES_bufferPtr, bytesOfCommit);

            bytesOfUpload += bytesOfCommit;
        }

        if(uploadTaskDesc->exitWhenEmpty && bufferEmpty){
#           if defined LES_DEBUG
                LES_LogMessage("LES_LogUploadTask: ringbuffer now has not readable data\n");
#           endif
            uploadTaskDesc->status = LES_Status_Successful;
            LES_MutexRelease(&uploadTaskDesc->compleSem);
            return LES_TaskReturnCode_Successful;
        }
    }
#   undef  LES_bufferPtr

error:
#   if defined LES_DEBUG_ERROR
        LES_LogMessage("LES_LogUploadTask: error happend\n");
#   endif
    uploadTaskDesc->status = LES_Status_NotSuccessful;
    LES_MutexRelease(&uploadTaskDesc->compleSem);
    return LES_TaskReturnCode_NotSuccessful;
}
LES_Status LES_LogUploadConfig(LES_LogManagePtr, LES_LogUploadDescriptionPtr);
LES_Status LES_LogUploadHashTable(LES_LogManagePtr, LES_LogUploadDescriptionPtr);

LES_Status LES_LogUploadBaseInformation(LES_LogManagePtr logManager, LES_LogUploadDescriptionPtr upload){
    if(LES_LogUploadConfig(logManager, upload) != LES_Status_Successful)
        return LES_Status_NotSuccessful;
    if(LES_LogUploadHashTable(logManager, upload) != LES_Status_NotSuccessful)
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}

LES_Status LES_LogUploadConfig(LES_LogManagePtr logManager, LES_LogUploadDescriptionPtr upload){
    int res;
    if(logManager == NULL || upload == NULL)
        return LES_Status_NotSuccessful;
    if(logManager->configEvent == NULL) /* 没有配置数据 */
        return LES_Status_Successful;
    res = upload->writeRtn(upload, logManager->configEvent, logManager->configEventSize);

    if(res < 0)
        return LES_Status_NotSuccessful;
    return LES_Status_Successful;
}

LES_Status LES_LogUploadHashTable(LES_LogManagePtr logManager, LES_LogUploadDescriptionPtr upload){
    LES_HashTableIteratorPtr iterator;
    LES_BufferDataPtr data;
    LES_Size dataSize;
    int res;
    LES_HashTablePtr htable;

    htable = logManager->table;

    if(htable == NULL)
        return LES_Status_NotSuccessful;
    if((iterator = LES_HashTableIteratorInit(htable)) == NULL)
        return LES_Status_NotSuccessful;

    while((data = LES_HashTableIteratorNextData(htable, iterator, &dataSize)) != NULL){
        res = upload->writeRtn(upload, data, dataSize);
        if(res < 0){
            LES_HashTableIteratorDone(iterator);
            return LES_Status_NotSuccessful;
        }
    }
    LES_HashTableIteratorDone(iterator);
    return LES_Status_Successful;
}
