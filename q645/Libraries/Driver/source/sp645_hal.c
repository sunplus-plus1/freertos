#include "sp64xx.h"
#include "sp645_hal_def.h"
#include "sp645_hal_stc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_SYS_STC_CLK	1000000			/*1MHz*/

STC_HandleTypeDef SysStandardTimeClk;

HAL_StatusTypeDef HAL_Init(void)
{
	HAL_InitTick(STC2);
}

HAL_StatusTypeDef HAL_DeInit(void)
{

}

__weak HAL_StatusTypeDef HAL_InitTick (STC_TypeDef *STCx)
{
		SysStandardTimeClk.Instance = STCx;
		SysStandardTimeClk.ClockSource = 0; /* sysclk src */
		SysStandardTimeClk.ExtDiv = 0;
		SysStandardTimeClk.Prescaler = (HSI_VALUE/DEFAULT_SYS_STC_CLK) - 1;                 /*the 1tick = 1us, 1MHz */
		HAL_STC_Init(&SysStandardTimeClk);
}

void HAL_IncTick(void)
{


}

/*Delay ticks*/
__weak void HAL_Delay(uint32_t Delay)
{
	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = 0;

	if (wait < HAL_MAX_DELAY)
	{
		wait += Delay;
	}

	while ((HAL_GetTick() - tickstart) < wait){
	}
}

uint32_t HAL_GetTick(void)
{
	uint32_t ticks = 0;
	ticks = (uint32_t)HAL_STC_Get_Counter(&SysStandardTimeClk);
	return ticks;
}

void HAL_Module_Clock_enable(MODULE_ID_Type id, uint32_t enable)
{
	if (enable == 1)
	{
		if(READ_BIT(CLK_EN->clock_enable[id/16], id%16) == 0)
			CLK_EN->clock_enable[id/16] = RF_MASK_V_SET(1 << (id%16));
	}
	else
	{
		if(READ_BIT(CLK_EN->clock_enable[id/16], id%16) == 1)
			CLK_EN->clock_enable[id/16] = RF_MASK_V_CLR(1 << (id%16));
	}
}

void HAL_Module_Clock_gate(MODULE_ID_Type id, uint32_t enable)
{
	if (enable == 1)
	{
		if(READ_BIT(CLK_GATE->clock_gate_enable[id/16], id%16) == 0)
			CLK_GATE->clock_gate_enable[id/16] = RF_MASK_V_SET(1 << (id%16));
	}
	else
	{
		if(READ_BIT(CLK_GATE->clock_gate_enable[id/16], id%16) == 1)
			CLK_GATE->clock_gate_enable[id/16] = RF_MASK_V_CLR(1 << (id%16));
	}

}

void HAL_Module_Reset(MODULE_ID_Type id, uint32_t enable)
{
	if (enable == 1)
	{
		if(READ_BIT(MODULE_REST->reset[id/16], id%16) == 0)
			MODULE_REST->reset[id/16] = RF_MASK_V_SET(1 << (id%16));
	}
	else
	{
		if(READ_BIT(MODULE_REST->reset[id/16], id%16) == 1)
			MODULE_REST->reset[id/16] = RF_MASK_V_CLR(1 << (id%16));
	}
}

#ifdef __cplusplus
}
#endif

