#ifndef __LES__LOCK__BY__MACHUNLEI__H__
#define __LES__LOCK__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)

#ifdef LES_CONFIG_SMP
    static inline int LES_SpinLockInit(LES_SpinLockTypePtr lock){
        if(lock == NULL)
            return -1;
        /*XXX change spinLockIsrNdInit to spinLockIsrInit*/
/*        SPIN_LOCK_ISR_INIT((*lock), 1);*/
        lock->flag = 0;
        return 0;
    }
    static inline int LES_SpinLock(LES_SpinLockTypePtr lock){
        if(lock == NULL)
            return -1;
        return spinLockIsrNdTake(lock);
    }
    static inline int LES_SpinUnlock(LES_SpinLockTypePtr lock, int value){
        if(lock == NULL)
            return -1;
        spinLockIsrNdGive(lock, value);
        return 0;
    }
#else
    static inline int LES_IntLock(){
        return intLock();
    }
    static inline int LES_IntUnlock(int value){
        intUnlock(value);
        return 0;
    }
#endif

#ifdef __cplusplus
}
#endif

#endif
