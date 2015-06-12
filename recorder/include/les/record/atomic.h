#ifndef __LES__ATOMIC__BY__MACHUNLEI__H__
#define __LES__ATOMIC__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)

#define LES_AtomicOperationDefine1(op)  \
    static inline LES_AtomicVal LES_Atomic ## op(LES_AtomicPtr key){\
        return atomic32 ## op(key); \
    }

#define LES_AtomicOperationDefine2(op)  \
    static inline LES_AtomicVal LES_Atomic ## op(LES_AtomicPtr key, LES_AtomicVal val){\
        return atomic32 ## op(key, val); \
    }

/* NOTICE: 下面的函数返回的值都是原子变量在操作之前的数值 */
LES_AtomicOperationDefine1(Clear)
LES_AtomicOperationDefine1(Get)
LES_AtomicOperationDefine1(Dec)
LES_AtomicOperationDefine1(Inc)

LES_AtomicOperationDefine2(Add)
LES_AtomicOperationDefine2(And)
LES_AtomicOperationDefine2(Nand)
LES_AtomicOperationDefine2(Or)
LES_AtomicOperationDefine2(Sub)
LES_AtomicOperationDefine2(Xor)
LES_AtomicOperationDefine2(Set)

static inline BOOL LES_AtomicCompareAndSwap(LES_AtomicPtr key, LES_AtomicVal oldVal, LES_AtomicVal newVal){
    return atomic32Cas(key, oldVal, newVal);
}


#ifdef __cplusplus
}
#endif

#endif
