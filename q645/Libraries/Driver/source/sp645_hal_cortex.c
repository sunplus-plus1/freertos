#include "sp64xx.h"
#include "sp645_hal_cortex.h"

void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
	/* Check the parameters */
	assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
	NVIC_SetPriorityGrouping(PriorityGroup);
}

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
	uint32_t prioritygroup = 0x00U;
	
	/* Check the parameters */
	assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
	assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
	
	prioritygroup = NVIC_GetPriorityGrouping();
	
	NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

void HAL_NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
	 /* Check the parameters */
	assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
	
	NVIC_SetVector(IRQn,vector);
}

uint32_t HAL_NVIC_GetVector(IRQn_Type IRQn)
{
	/* Check the parameters */
	 assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
	
	 return NVIC_GetVector(IRQn);
}

void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
	/* Check the parameters */
	assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
	
	/* Enable interrupt */
	NVIC_EnableIRQ(IRQn);
}

void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
	/* Check the parameters */
	assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
	
	/* Disable interrupt */
	NVIC_DisableIRQ(IRQn);
}

void HAL_NVIC_SystemReset(void)
{
	/* System Reset */
	NVIC_SystemReset();
}

uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb)
{
	 return SysTick_Config(TicksNumb);
}

uint32_t HAL_NVIC_GetPriorityGrouping(void)
{
	return NVIC_GetPriorityGrouping();
}

void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority)
{
	assert_param(IS_NVIC_PRIORITY_GROUP(PriorityGroup));
	NVIC_DecodePriority(NVIC_GetPriority(IRQn), PriorityGroup, pPreemptPriority, pSubPriority);
}

void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
	NVIC_SetPendingIRQ(IRQn);
}

uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
	/* Return 1 if pending else 0U */
	return NVIC_GetPendingIRQ(IRQn);
}

void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
	/* Clear pending interrupt */
	NVIC_ClearPendingIRQ(IRQn);
}

uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn)
{
	/* Return 1 if active else 0U */
	return NVIC_GetActive(IRQn);
}
