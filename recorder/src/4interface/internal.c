#include <sys/types.h>
#include <tasks.h>
#include <string.h>
#include <les/interface/interface_define.h>
unsigned long long LES_TimeVal[5*1024];


int LES_Offset[5]={0,0,0,0,0};
int LES_Enable[5]={0,0,0,0,0};

void LES_timeGet(unsigned int *pTbu, unsigned int *pTbl)
{
	/*__asm__("loop:");
	__asm__("	mftbu	5");
	__asm__("	mftb	6");
	__asm__("	mftbu	7");
	__asm__("	cmpw	5,7");
	__asm__("	bne		loop");
	__asm__("	stw		5,0x0(3)");
	__asm__("	stw		6,0x0(4)");*/
}

void LES_TimeGet(int type)
{
	/*unsigned int th, tl;
	unsigned long long tt;
	if(((type!=1)&&(type!=2))||(LES_Enable[type]==0))
		return;
	LES_timeGet(&th, &tl);
	tt=((unsigned long long)th<<32)+((unsigned long long)tl);
	LES_TimeVal[LES_Offset[type]%1024+type*1024]=tt;
	LES_Offset[type]=LES_Offset[type]+1;*/
}

size_t LES_CRC32(void* stackAddr, int size){
	return 0;
}

void LES_Context2Registers(void* const a, int b, const void* c){
	ACoreOs_task_context* context = (ACoreOs_task_context*)c;
	UINT32* array = (UINT32*)a;

	/* 由于context只使用了某些寄存器，故现在对一些寄存器进行扩展应用 */
	array[0] = (UINT32)context->esp;
	array[1] = context->cr;
	array[2] = context->gpr2;
	array[3] = context->pc;
	array[4] = context->msr;

	/* 剩余5 ~ 12寄存器没有使用 */
	/*
	 * array[5] =
	 * array[6] =
	 * array[7] =
	 * array[8] =
	 * array[9] =
	 * array[10] =
	 * array[11] =
	 * array[12 =
	 * */

	/* 拷贝r13 ~ r31寄存器到数组中 */
	memcpy(&array[13], &context->gpr13, sizeof(UINT32) * (32 - 13));

	b == 33 * sizeof(int) ? array[32] = context->pc : (void)0;
}

void LES_InterruptExitReschedule(){
	/* 加入记录代码 */
	__LES__DO
	        __LES__IF(__LES__RR)
				LES_EventParamInt_1(LES_EVENT_INTERRUPTRESCHEDULE, 0);
	    __LES__SYSCALLEXTEND;
}

void intExitStub(void* context){
	return;
    __LES__DO   \
        __LES__IF(__LES__RR)    \
            LES_EventParamInt_22(LES_EVENT_INTERRUPTEXIT_CONTEXT,   \
                    context);   \
    __LES__SYSCALLEXTEND;
}
