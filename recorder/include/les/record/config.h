#ifndef __LES__CONFIG__BY__MACHUNLEI__H__
#define __LES__CONFIG__BY__MACHUNLEI__H__

#define LES_TASKARGUMENTNUMS    10 /*配置任务参数个数*/
#define LES_TASKREGISTERNUMS    32 /*配置保存任务上下文中寄存器的个数，R1~Rx*/

/*
 * 以下选项配置内核中任务TCB中插桩代码
 */
#undef  LES__DEFINERECORDFLAG /*表明启用内核中定义的LES_RecordFlag结构体定义，具体参见data_define.h文件*/
#define LES__COUNTERNUMS 3 /*配置内核中计数器的个数*/
#define LES__FLAGBITWIDTH 3 /*配置标识所占位宽*/

#define SYS_TCB     Thread_Control

#define LES_MAXNAMELEN  128


#define LES_RINGBUFFER_KEEP     1


/* 缓冲区配置 */
/* 消息部分 */
#ifndef LES_RINGBUFFER_MESSAGE_MAX
#define LES_RINGBUFFER_MESSAGE_MAX          20
#endif

#ifndef LES_RINGBUFFER_MESSAGE_SIZE
#define LES_RINGBUFFER_MESSAGE_SIZE         50
#endif

#define LES_RINGBUFFER_MESSAGE_ADD          0x1
#define LES_RINGBUFFER_MESSAGE_FREE         0x2
#define LES_RINGBUFFER_MESSAGE_THRESHOLD    0x3
#define LES_RINGBUFFER_MESSAGE_FULL         0x4
#define LES_RINGBUFFER_ERROR_FULL           0x10
#define LES_RINGBUFFER_UPLOAD_DEFER         0x11


#define LES_RINGBUFFER_MANAGER_STACKSIZE    4096
#define LES_LOG_UPLOAD_STACKSIZE            4096
#define LES_TASKATTR_DEFAULT                ACOREOS_DEFAULT_ATTRIBUTES

#define LES_HASHTABLE_ENTRY_NUMS            100

/* 上传失败的最大次数 */
#define LES_LOGUPLOAD_ATTEMPT_MAX           10
/* 缓冲区管理任务睡眠的时间间隔 */
#define LES_RINGBUFFER_POLLING_INTERVAL     10
/* 日志上传任务的优先级 */
#define LES_LOG_UPLOAD_TASKPRIORITY         19

#define LES_RINGBUFFER_MANAGER_TASKPRIORITY	20


#endif
