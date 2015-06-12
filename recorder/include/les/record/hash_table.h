#ifndef __LES__HASH__TABLE__BY__MACHUNLEI__H__
#define __LES__HASH__TABLE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif
#include LES_HEADER(type_define.h)

typedef struct les_hashtable{
    LES_BufferDataPtr       data;
    LES_Size                size;
    int                     key;
    struct les_hashtable*   next;
}LES_HashTableNode, *LES_HashTableNodePtr;

typedef struct{
    LES_Size                size;
    LES_HashTableNodePtr*   table;
    LES_MutexId         mutex;
}LES_HashTable, *LES_HashTablePtr;

typedef struct{
    int index;
    LES_HashTableNodePtr node;
}LES_HashTableIterator, *LES_HashTableIteratorPtr;


LES_HashTablePtr LES_HashTableCreate(LES_Size);

LES_HashTableIteratorPtr LES_HashTableIteratorInit(LES_HashTablePtr);

LES_HashTableNodePtr LES_HashTableIteratorNext(LES_HashTablePtr,
        LES_HashTableIteratorPtr);

LES_Status LES_HashTableDestroy(LES_HashTablePtr);

void LES_HashTableIteratorDone(LES_HashTableIteratorPtr);

LES_BufferDataPtr LES_HashTableIteratorNextData(LES_HashTablePtr, LES_HashTableIteratorPtr, LES_Size*);

#ifdef __cplusplus
}
#endif

#endif
