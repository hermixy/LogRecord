#ifndef __LES__MEMORY__MANAGER__BY__MACHUNLEI__H__
#define __LES__MEMORY__MANAGER__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

static inline void* LES_MemoryMalloc(int size){
    return malloc(size);
}
static inline void LES_MemoryFree(void* data){
    free(data);
}

#ifdef __cplusplus
}
#endif

#endif
