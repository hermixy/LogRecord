#ifndef __LES__RINGBUFFER__DEFINE__BY__MACHUNLEI__H__
#define __LES__RINGBUFFER__DEFINE__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

#include LES_HEADER(type_define.h)

struct les_ringbufferentry;
typedef struct{
    struct les_ringbufferentry*     next;
    struct les_ringbufferentry*     prev;
    LES_Size                    dataLen; /*表示有效的数据长度 */
    LES_BufferDataPtr           dataEnd;
    LES_Size                    spaceAvail;
    BOOL                        uncommitedRead;
}LES_RingEntryManager, *LES_RingEntryManagerPtr;

typedef struct les_ringbufferentry{
    LES_RingEntryManager    manager;
    LES_BufferData          data[0];
}LES_RingBufferEntry, *LES_RingBufferEntryPtr;


typedef struct{
    LES_Size        bufferSize;
    LES_Counter     currBufferNums;
    LES_Counter     threshold;
    LES_Counter     minBufferNums;
    LES_Counter     maxBufferNums;
    LES_Counter     maxBufferActual;
    LES_Counter     emptyBufferNums;
    LES_Counter     dataContent;
    LES_Counter     writeSinceReset;
    LES_Counter     readSinceReset;
    LES_Counter     options;
}LES_RingBufferInformation;

#ifdef __cplusplus
}
#endif

#endif
