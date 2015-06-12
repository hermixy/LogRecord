#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(type_define.h)
#include LES_HEADER(log.h)
#include LES_HEADER(memory_manager.h)
#include LES_HEADER(atomic.h)

LES_LogManagePtr LES_logManagerPointer;

LES_LogManagePtr LES_LogManageCreate(LES_BufferDescriptionPtr buffer){
    LES_LogManagePtr logManager;
    LES_HashTablePtr hashTable;

    logManager = (LES_LogManagePtr)LES_MemoryMalloc(sizeof(LES_LogManage));
    if(logManager == NULL){
#ifdef LES_DEBUG
        LES_LogMessage("LES_LogManage: memory malloc for LES_LogManage error\n");
#endif
        return NULL;
    }
    /* 创建hash table */
    hashTable = LES_HashTableCreate(LES_HASHTABLE_ENTRY_NUMS);
    if(hashTable == NULL){
        LES_MemoryFree(logManager);
#ifdef LES_DEBUG
        LES_LogMessage("LES_LogManage: LES_HashTableCreate called return error\n");
#endif
        return NULL;
    }

    logManager->table = hashTable;
    logManager->buffer = buffer;
    logManager->configEvent = NULL;
    logManager->configEventSize = 0;

    /* 记录系统基本信息　*/

    return logManager;
}

