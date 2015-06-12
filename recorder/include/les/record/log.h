#ifndef __LES__LOG__BY__MACHUNLEI__H__
#define __LES__LOG__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif
#include LES_HEADER(list.h)
#include LES_HEADER(hash_table.h)
#include LES_HEADER(buffer_description.h)


typedef struct{
    LES_ListNode                node;
    LES_BufferDescriptionPtr    buffer;
    LES_BufferDataPtr           configEvent;
    LES_Size                    configEventSize;
    LES_HashTablePtr            table;
}LES_LogManage, *LES_LogManagePtr;


LES_LogManagePtr LES_LogManageCreate(LES_BufferDescriptionPtr);
LES_Status LES_LogEventStop();

#ifdef __cplusplus
}
#endif
#endif
