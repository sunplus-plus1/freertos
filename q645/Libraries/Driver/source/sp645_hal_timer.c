#include "sp645_hal_timer.h"
#include "sp645_hal_stc.h"


#ifdef __cplusplus
extern "C" {
#endif

static IRQn_Type HAL_TIM_GetIRQ(TIM_TypeDef *tim)
{
	IRQn_Type IRQn = MAX_IRQ_n;
	if ((tim == NULL) || (tim == (TIM_TypeDef*)0xFFFFFFFF))
		return IRQn;

	assert_param(IS_TIM_INSTANCE(tim));

	switch ((uint32_t)tim)
	{
		case  (uint32_t)TIM0 ... (uint32_t)TIM1:
			IRQn = STC_TIMER2_IRQn + ((uint32_t)tim - (uint32_t)TIM0)/_OFFSET_BETWEEN_TIMERS;
			break;
		case  (uint32_t)TIM2 ... (uint32_t)TIM3:
			IRQn = STC_AV0_TIMER2_IRQn + ((uint32_t)tim - (uint32_t)TIM2)/_OFFSET_BETWEEN_TIMERS;
			break;
		case  (uint32_t)TIM4 ... (uint32_t)TIM5:
			IRQn = STC_AV1_TIMER2_IRQn + ((uint32_t)tim - (uint32_t)TIM4)/_OFFSET_BETWEEN_TIMERS;
			break;
  		case  (uint32_t)TIM6 ... (uint32_t)TIM7:
			IRQn = STC_AV2_TIMER2_IRQn + ((uint32_t)tim - (uint32_t)TIM6)/_OFFSET_BETWEEN_TIMERS;
			break;
		default:
		break;
	}
	return IRQn;
}

timerObj_t *HAL_TIM_Get_timer_obj(TIM_HandleTypeDef *htim)
{
  timerObj_t *obj;
  obj = (timerObj_t *)((char *)htim - offsetof(timerObj_t, handle));
  return (obj);
}

void _HAL_TIM_Set_TimerClk(TIM_HandleTypeDef *htim,uint32_t enable)
{
	MODULE_ID_Type mode_id = STC_0;
	switch((uint32_t)htim->Instance)
	{
		case (uint32_t)TIM0 ... (uint32_t)TIM1: 
			mode_id = STC_0;
			break;
		case (uint32_t)TIM2 ... (uint32_t)TIM3: 
			mode_id = STC_AV0;
			break;
		case (uint32_t)TIM4 ... (uint32_t)TIM5: 
			mode_id = STC_AV1;
			break;
		case (uint32_t)TIM6 ... (uint32_t)TIM7: 
			mode_id = STC_AV2;
			break;
			break;
		default: 
			break;
	}
	HAL_Module_Clock_enable(mode_id, enable);
	HAL_Module_Clock_gate(mode_id, enable);
	HAL_Module_Reset(mode_id, !enable);
}

void HAL_TIM_EnableTimerClock(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	_HAL_TIM_Set_TimerClk(htim,1);
}

void HAL_TIM_DisableTimerClock(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	_HAL_TIM_Set_TimerClk(htim,0);
}

void TIM_SetConfig(TIM_TypeDef *TIMx, TIM_InitTypeDef *Structure)
{
	MODIFY_REG(TIMx->control, TIMER_TRIG_SRC, Structure->ClockSource<<TIMER_TRIG_SRC_Pos);
	MODIFY_REG(TIMx->control, TIMER_RPT, Structure->AutoReloadPreload<<TIMER_RPT_pos);
	TIMx->counter_val = Structure->Counter;
	TIMx->reload_val = Structure->ReloadCounter;
	TIMx->prescale_val = Structure->Prescaler;
}

HAL_TIM_StateTypeDef HAL_TIM_GetState(TIM_HandleTypeDef *htim)
{
	return htim->State;
}

HAL_StatusTypeDef HAL_TIM_Start(TIM_HandleTypeDef *htim)
{
	/* Check the parameters */
	assert_param(IS_TIM_INSTANCE(htim->Instance));

	/* Set the TIM state */
	htim->State = HAL_TIM_STATE_BUSY;

	MODIFY_REG(htim->Instance->control, TIMER_GO, 1<<TIMER_GO_Pos);

	/* Change the TIM state*/
	htim->State = HAL_TIM_STATE_READY;

	/* Return function status */
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Stop(TIM_HandleTypeDef *htim)
{
	/* Check the parameters */
	assert_param(IS_TIM_INSTANCE(htim->Instance));

	/* Set the TIM state */
	htim->State = HAL_TIM_STATE_BUSY;
	

	MODIFY_REG(htim->Instance->control, TIMER_GO, 0<<TIMER_GO_Pos);

	/* Change the TIM state*/
	htim->State = HAL_TIM_STATE_READY;

	/* Return function status */
	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_SetPrescaler(TIM_HandleTypeDef *htim, uint32_t u32Prescaler)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	htim->Instance->prescale_val = u32Prescaler;
	htim->Init.Prescaler = u32Prescaler;
	return HAL_OK;
}

uint32_t HAL_TIM_GetPrescaler(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
 	return htim->Instance->prescale_val;
}

HAL_StatusTypeDef HAL_TIM_setCount(TIM_HandleTypeDef *htim, uint32_t u32Count)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	htim->Instance->counter_val = u32Count;
	htim->Instance->reload_val = u32Count;
	return HAL_OK;
}

uint32_t HAL_TIM_GetCount(TIM_HandleTypeDef *htim)
{
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	return htim->Instance->counter_val;
}

uint32_t HAL_TIM_GetCLKSrc(TIM_HandleTypeDef *htim)
{
	uint32_t u32Src = 0;
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	return (READ_BIT(htim->Instance->control, TIMER_TRIG_SRC)>>TIMER_TRIG_SRC_Pos);
}

uint32_t HAL_TIM_GetMasterCLKFreq(TIM_HandleTypeDef *htim)
{
	TIM_TypeDef *hMtim = 0;
	uint32_t u32Feq = 0;
	uint32_t u32Src = 0;
	uint32_t u32Prescaler = 0;
	uint32_t u32Counter = 0;
		
	assert_param(IS_TIM_INSTANCE(htim->Instance));
	if(HAL_TIM_GetCLKSrc(htim) == CLK_SLAVE_WRAP_SRC) /* time0 used timer1 clk src,get timer1's freq */
	{
		switch ((uint32_t)htim->Instance)
		{
			case  (uint32_t)TIM0:
			case  (uint32_t)TIM2:
			case  (uint32_t)TIM4:
			case  (uint32_t)TIM6:
				hMtim = htim->Instance + _OFFSET_BETWEEN_TIMERS;
				break;
			case  (uint32_t)TIM1:
			case  (uint32_t)TIM3:
			case  (uint32_t)TIM5:
			case  (uint32_t)TIM7:
				hMtim = htim->Instance - _OFFSET_BETWEEN_TIMERS;
				break;
			default:
			break;
		}
	}
	
	u32Src = READ_BIT(hMtim->control, TIMER_TRIG_SRC)>>TIMER_TRIG_SRC_Pos;
	u32Prescaler = hMtim->prescale_val;
	u32Counter = hMtim->counter_val;

	switch (u32Src){
		case CLK_SYS_SRC:
			u32Feq = HSI_VALUE;
			break;
		case CLK_STC_SRC:
			u32Feq = HAL_STC_GetClk((STC_TypeDef *)(((uint32_t)hMtim / _REG_GROUP_SIZE) * _REG_GROUP_SIZE));/*get stc base address by timer address */ 
			break;
		case CLK_RTC_SRC:
			break;
		case CLK_EXT_SRC:
			u32Feq = HSE_VALUE/2;
			break;
		default:
			u32Feq = HSI_VALUE;
			break;
	}
	
	u32Feq = u32Feq/(u32Prescaler+1);
	if ((READ_BIT(htim->Instance->control, TIMER_TRIG_SRC)>>TIMER_TRIG_SRC_Pos) == CLK_SLAVE_WRAP_SRC)
	{
		if(u32Counter != 0)
			u32Feq /= u32Counter;
	}	
	return u32Feq;
}

HAL_StatusTypeDef HAL_TIM_Enable_Interrupt(TIM_HandleTypeDef *htim)
{
	IRQn_Type irqn = MAX_IRQ_n;

	if (htim == NULL)
	{
		return HAL_ERROR;
	}

	assert_param(IS_TIM_INSTANCE(htim->Instance));
	irqn = HAL_TIM_GetIRQ(htim->Instance);
	HAL_NVIC_EnableIRQ(irqn);

	return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Disable_Interrupt(TIM_HandleTypeDef *htim)
{
	IRQn_Type irqn = MAX_IRQ_n;

	if (htim == NULL)
	{
		return HAL_ERROR;
	}

	assert_param(IS_TIM_INSTANCE(htim->Instance));
	irqn = HAL_TIM_GetIRQ(htim->Instance);
	HAL_NVIC_DisableIRQ(irqn);

	return HAL_OK;
}


HAL_StatusTypeDef HAL_TIM_Init(TIM_HandleTypeDef *htim)
{
	IRQn_Type irqn =  MAX_IRQ_n;
		
	if (htim == NULL)
	{
		return HAL_ERROR;
	}

	assert_param(IS_TIM_INSTANCE(htim->Instance));

	__HAL_LOCK(htim);

	if (htim->State == HAL_TIM_STATE_RESET)
	{
		htim->Lock = HAL_UNLOCKED;
		irqn = HAL_TIM_GetIRQ(htim->Instance);
		HAL_NVIC_SetVector(irqn, (uint32_t)htim->IrqHandle);
		HAL_NVIC_EnableIRQ(irqn);
	}
	htim->State = HAL_TIM_STATE_BUSY;
	htim->Instance->control = 0;
	htim->Instance->reload_val = 0;
	htim->Instance->counter_val = 0;
	htim->Instance->prescale_val = 0;
	TIM_SetConfig(htim->Instance, &htim->Init);

	/* Initialize the TIM state*/
	htim->State = HAL_TIM_STATE_READY;

	__HAL_UNLOCK(htim);

	return HAL_OK;
}


HAL_StatusTypeDef HAL_TIM_DeInit(TIM_HandleTypeDef *htim)
{
	/* Check the parameters */
	assert_param(IS_TIM_INSTANCE(htim->Instance));

	__HAL_LOCK(htim);

	htim->State = HAL_TIM_STATE_BUSY;

	/* Disable the TIM Peripheral Clock */
	MODIFY_REG(htim->Instance->control, TIMER_GO, 0<<TIMER_GO_Pos);
	
	/* Change TIM state */
	htim->State = HAL_TIM_STATE_RESET;

	/* Release Lock */
	__HAL_UNLOCK(htim);

	return HAL_OK;
}


#ifdef __cplusplus
}
#endif

