#ifndef __LES__HEADER__FILE__BY__MACHUNLEI__H__
#define __LES__HEADER__FILE__BY__MACHUNLEI__H__

#define LES_HEADER(file) <les/record/file>
#define LES_CONFIG_FILE LES_HEADER(config.h)

#define LES_MUTEX_DEPENDENCE_FILE_4
#define LES_MUTEX_DEPENDENCE_FILE1  \
    <status.h>
#define LES_MUTEX_DEPENDENCE_FILE2  \
    <tasks.h>
#define LES_MUTEX_DEPENDENCE_FILE3  \
    <options.h>
#define LES_MUTEX_DEPENDENCE_FILE4  \
    <acoreos.h>

#define LES_TASK_DEPENDENCE_FILE_3
#define LES_TASK_DEPENDENCE_FILE1   \
    <status.h>
#define LES_TASK_DEPENDENCE_FILE2   \
    <tasks.h>
#define LES_TASK_DEPENDENCE_FILE3	\
	<smpConfig.h>

#define LES_STATUS_DEPENDENCE_FILE_1
#define LES_STATUS_DEPENDENCE_FILE1  \
    <status.h>

#define LES_TYPEDEFINE_DEPENDENCE_FILE_3
#define LES_TYPEDEFINE_DEPENDENCE_FILE1	\
		<atomicArchLib.h>
#define LES_TYPEDEFINE_DEPENDENCE_FILE2	\
		<types.h>
#define LES_TYPEDEFINE_DEPENDENCE_FILE3	\
		<tasks.h>

/*#define LES_DEBUG 1*/
#undef LES_DEBUG_ERROR 1

#define LES_CONFIG_SMP 1
#define LES_LogMessage(fmt, args...)	do{\
		printk_spinLock(fmt, ##args);	\
	}while(0)

#endif
