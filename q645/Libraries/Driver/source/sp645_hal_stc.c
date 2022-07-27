#include "sp645_hal.h"
#include "sp645_hal_stc.h"

#ifdef __cplusplus
extern "C" {
#endif


void HAL_STC_EnableSTCClock(STC_HandleTypeDef *Hstc)
{
	MODULE_ID_Type mode_id = STC_0;
	
	assert_param(IS_STC_INSTANCE(Hstc->Instance));
	
	switch((uint32_t)Hstc->Instance)
	{
		case (uint32_t)STC0: 
			mode_id = STC_0;
			break;
		case (uint32_t)STC1: 
			mode_id = STC_AV0;
			break;
		case (uint32_t)STC2: 
			mode_id = STC_AV1;
			break;
		case (uint32_t)STC3: 
			mode_id = STC_AV2;
			break;
		default: 
			break;
	}

	HAL_Module_Clock_enable(mode_id, 1);
	HAL_Module_Clock_gate(mode_id, 1);
	HAL_Module_Reset(mode_id, 0);

}

HAL_StatusTypeDef HAL_STC_Init(STC_HandleTypeDef *Hstc)
{
	if (Hstc == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));
	
	HAL_STC_EnableSTCClock(Hstc);
	memset(Hstc->Instance, 0, sizeof(STC_TypeDef));	
	Hstc->Instance->stc_prescale_val = 0;
	Hstc->Instance->stc_config = 0;
	/*External Clock source*/
	if (Hstc->ClockSource)
	{
		Hstc->Instance->stc_config = Hstc->ExtDiv;
		MODIFY_REG(Hstc->Instance->stc_prescale_val, STC_TRIG_SRC, 1<<STC_TRIG_SRC_Pos);
	}
	
	MODIFY_REG(Hstc->Instance->stc_prescale_val, STC_PRESCALER, Hstc->Prescaler<<STC_PRESCALER_Pos);
 	Hstc->Instance->stc_63_48 = 0;
	Hstc->Instance->stc_47_32 = 0;
	Hstc->Instance->stc_31_16 = 0;
	Hstc->Instance->stc_15_0 = 0;

	return HAL_OK;
}

HAL_StatusTypeDef HAL_STC_SetPrescaler(STC_HandleTypeDef *Hstc, uint32_t u32Prescaler)
{
	if (Hstc == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));

	MODIFY_REG(Hstc->Instance->stc_prescale_val, STC_PRESCALER, Hstc->Prescaler<<STC_PRESCALER_Pos);
	Hstc->Prescaler = u32Prescaler;
	return HAL_OK;
}

HAL_StatusTypeDef HAL_STC_SetExtDiv(STC_HandleTypeDef *Hstc, uint32_t u32div)
{
	if (Hstc == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));

	MODIFY_REG(Hstc->Instance->stc_config, STC_EXT_DIV, Hstc->Instance->stc_config << STC_EXT_DIV_Pos);
	Hstc->ExtDiv = u32div;
	
	return HAL_OK;
}

uint32_t HAL_STC_GetPrescaler(STC_HandleTypeDef *Hstc)
{
	if (Hstc == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));

	return (Hstc->Instance->stc_prescale_val&STC_PRESCALER_Msk)>>STC_PRESCALER_Pos;

}

uint32_t HAL_STC_GetExtDiv(STC_HandleTypeDef *Hstc)
{
	if (Hstc == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));

	return (Hstc->Instance->stc_config&STC_EXT_DIV_Msk)>>STC_EXT_DIV_Pos;

}

uint32_t HAL_STC_GetClk(STC_TypeDef *STC_Instance)
{
	uint32_t u32Src=0,src_clk;
	if (STC_Instance == NULL)
		return HAL_ERROR;
	assert_param(IS_STC_INSTANCE(STC_Instance));
	u32Src = READ_BIT(STC_Instance->stc_prescale_val, STC_TRIG_SRC)>>STC_PRESCALER_Pos;
	if(u32Src == 0) /* stc used sysclk */
	{
		src_clk = HSI_VALUE;
	}
	else
	{
		src_clk = HSE_VALUE/(2*(STC_Instance->stc_config+1));
	}
	
	return src_clk/((STC_Instance->stc_prescale_val&STC_PRESCALER_Msk)>>STC_PRESCALER_Pos + 1);
}


uint32_t HAL_STC_Reset_Counter(STC_HandleTypeDef *Hstc)
{
	if (Hstc == NULL)
		return HAL_ERROR;

	Hstc->Instance->stc_64 = 0x1; //write this bit to reset count
	Hstc->Instance->stc_63_48 = 0;
	Hstc->Instance->stc_47_32 = 0;
	Hstc->Instance->stc_31_16 = 0;
	Hstc->Instance->stc_15_0 = 0;
	
	return HAL_OK;
}

uint64_t HAL_STC_Get_Counter(STC_HandleTypeDef *Hstc)
{
	uint64_t ret = 0;
	if (Hstc == NULL)
		return 0;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));
	ret = Hstc->Instance->stc_15_0; //read stc_15_0 to latch data
	ret = (((uint64_t)(Hstc->Instance->stc_63_48) << 48) | ((uint64_t)(Hstc->Instance->stc_47_32) << 32) \
		| ((uint64_t)(Hstc->Instance->stc_31_16) << 16) | Hstc->Instance->stc_15_0) ;
	return ret;
}

uint64_t HAL_STC_Get_latchCounter(STC_HandleTypeDef *Hstc)
{
	uint64_t ret = 0;
	if (Hstc == NULL)
		return 0;
	assert_param(IS_STC_INSTANCE(Hstc->Instance));
	WRITE_REG(Hstc->Instance->stcl_32, 0x1234);
	ret = ((Hstc->Instance->stc_31_16) << 16) | Hstc->Instance->stc_15_0;
	if (READ_REG(Hstc->Instance->stcl_32)&1)
		ret |= 0x100000000;
	return ret;
}

#ifdef __cplusplus
}
#endif
