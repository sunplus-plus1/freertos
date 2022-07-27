#ifndef __SP645_HAL_H_
#define __SP645_HAL_H_

#include "sp645_hal_conf.h"
#include "sp645_cm4.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef enum
{
	HAL_TICK_FREQ_10HZ        = 100000U,
	HAL_TICK_FREQ_100HZ       = 10000U,
	HAL_TICK_FREQ_1KHZ        = 1000U,
	HAL_TICK_FREQ_1MHZ        = 1U,
	HAL_TICK_FREQ_DEFAULT     = HAL_TICK_FREQ_1MHZ
} HAL_TickFreqTypeDef;

#define IS_TICKFREQ(FREQ) (((FREQ) == HAL_TICK_FREQ_10HZ)  || \
                           ((FREQ) == HAL_TICK_FREQ_100HZ) || \
                           ((FREQ) == HAL_TICK_FREQ_1KHZ))


uint32_t HAL_GET_AOdomain_Sysclk(void);
uint32_t HAL_GET_PWM_SrcClk(void);

HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
HAL_StatusTypeDef HAL_InitTick (STC_TypeDef *STCx);
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);


void HAL_Module_Clock_enable(MODULE_ID_Type id, uint32_t enable);
void HAL_Module_Clock_gate(MODULE_ID_Type id, uint32_t enable);
void HAL_Module_Reset(MODULE_ID_Type id, uint32_t enable);


#ifdef __cplusplus
}
#endif

#endif 

