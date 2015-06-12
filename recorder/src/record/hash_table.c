#include "headerfile_config.h"
#include LES_CONFIG_FILE
#include LES_HEADER(hash_table.h)
#include LES_HEADER(memory_manager.h)
#include LES_HEADER(mutex.h)

LES_HashTablePtr LES_HashTableCreate(LES_Size size){
    LES_HashTablePtr table;
    LES_HashTableNodePtr* nodes;
    int ix, jx;

    table = (LES_HashTablePtr)LES_MemoryMalloc(sizeof(LES_HashTable));
    if(table == NULL)
        return NULL;
    table->mutex = 0; /* 这里需要创建信号量 */

    nodes = (LES_HashTableNodePtr*)LES_MemoryMalloc(size * sizeof(LES_HashTableNodePtr));
    if(nodes == NULL){
        /* 销毁信号量　*/
        LES_MemoryFree(table);
        return NULL;
    }

    for(ix = 0; ix < size; ix ++){
        nodes[ix] = (LES_HashTableNodePtr)LES_MemoryMalloc(sizeof(LES_HashTableNode));
        if(nodes[ix] == NULL){
            for(jx = 0; jx < ix; jx ++)
                LES_MemoryFree(nodes[jx]);
            LES_MemoryFree(nodes);
            /* 销毁信号量操作　*/
            LES_MemoryFree(table);
            return NULL;
        }
        nodes[ix]->next = NULL;
        nodes[ix]->key = 0;
        nodes[ix]->data = NULL;
        nodes[ix]->size = 0;
    }

    table->size = size;
    table->table = nodes;
    return table;
}

LES_HashTableIteratorPtr LES_HashTableIteratorInit(LES_HashTablePtr htable){
    LES_HashTableIteratorPtr htableIterator;
    if(htable == NULL)
        return NULL;
    htableIterator = (LES_HashTableIteratorPtr)LES_MemoryMalloc(sizeof(LES_HashTableIterator));

    if(htableIterator == NULL)
        return NULL;

    htableIterator->index = 0;
    htableIterator->node = htable->table[0]->next;
    return htableIterator;
}

LES_HashTableNodePtr LES_HashTableIteratorNext(LES_HashTablePtr htable,
        LES_HashTableIteratorPtr iterator){
    LES_HashTableNodePtr node;
    if(htable == NULL)
        return NULL;
    node = NULL;
    while(node == NULL && iterator->index < htable->size){
        if(iterator->node == NULL){
            iterator->index ++;
            if(iterator->index < htable->size)
                iterator->node = htable->table[iterator->index]->next;
        }
        else{
            node = iterator->node;
            iterator->node = iterator->node->next;
        }
    }
    return node;
}

LES_Status LES_HashTableDestroy(LES_HashTablePtr htable){
    LES_HashTableIteratorPtr iterator;
    LES_HashTableNodePtr node;
    int ix;
    if(htable == NULL)
        return LES_Status_NotSuccessful;
    if((iterator = LES_HashTableIteratorInit(htable)) == NULL)
        return LES_Status_NotSuccessful;

    if(LES_MutexObtain(&htable->mutex) != LES_Status_Successful)
        return LES_Status_NotSuccessful;
    while((node = LES_HashTableIteratorNext(htable, iterator)) != NULL){
        LES_MemoryFree((void*)node->data);
        LES_MemoryFree((void*)node);
    }

    LES_HashTableIteratorDone(iterator);

    for(ix = 0; ix < htable->size; ++ix)
        LES_MemoryFree((void*)htable->table[ix]);

    LES_MemoryFree((void*)htable->table);
    LES_MemoryFree((void*)htable);
    LES_MutexRelease(&htable->mutex);
    return LES_Status_Successful;
}

void LES_HashTableIteratorDone(LES_HashTableIteratorPtr iterator){
    if(iterator != NULL)
        LES_MemoryFree((void*)iterator);
}

LES_BufferDataPtr LES_HashTableIteratorNextData(LES_HashTablePtr htable,
        LES_HashTableIteratorPtr iterator, LES_Size* size){
    LES_HashTableNodePtr node;
    if((node = LES_HashTableIteratorNext(htable, iterator)) == NULL)
        return NULL;
    if(size)
        *size = node->size;
    return node->data;
}
