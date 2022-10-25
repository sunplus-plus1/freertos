#ifndef __SP645_HAL_GPIO_H
#define __SP645_HAL_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sp645_hal_def.h"
#include "sp645_hal_conf.h"
#include "sp64xx.h"

/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */
typedef enum
{
  GPIO_OUT_LOW = 0,      // gpio set to low
  GPIO_OUT_HIGH          // gpio set to high
}GPIO_PinState;

typedef enum
{
  GPIO_INPUT_MODE = 0,      // set to input mode
  GPIO_PP_OUTPUT_MODE,      // set to PP-output mode
  GPIO_OD_OUTPUT_MODE       // set to OD-output mode
}GPIO_PinMode;

typedef enum
{
  GPIO_PULL_UP = 0,			  // pull up
  GPIO_PULL_DOWN,		      // pull down
  GPIO_PULL_DISABLE	         // pull disable 
}GPIO_PULLState;


typedef enum
{
  GPIO_DS_0 = 0,	   // driver selector level 0
  GPIO_DS_1,		   // driver selector level 1
  GPIO_DS_2,		   // driver selector level 2
  GPIO_DS_3,		   // driver selector level 3
  GPIO_DS_DISABLE      // driver selector disable 
}GPIO_DSState;


typedef struct
{
  /* set input/output mode                         GPIO_INPUT_MODE:input          GPIO_OUTPUT_MODE:output   */
  GPIO_PinMode Mode;
  /* set default value.for output mode/enable OD,  GPIO_OUT_LOW:low               GPIO_OUT_HIGH:high        */ 
  GPIO_PULLState pull;
  /* set output higher driving current, */
  GPIO_DSState ds;
  /* set output default value after gpio init */
  GPIO_PinState out_value;
  /* set pin number: one of GPIO_P0_00--GPIO_P8_07  */
  uint32_t Pin;
}GPIO_InitTypeDef;



#define IS_GPIO_PIN_ACTION(pin)   gpio_first_val_get(pin)

void              HAL_GPIO_Init(GPIO_InitTypeDef *GPIO_Init);
void              HAL_GPIO_DeInit(uint16_t GPIO_Pin);
GPIO_PinState     HAL_GPIO_ReadPin(uint16_t GPIO_Pin);
void              HAL_GPIO_WritePin(uint16_t GPIO_Pin, GPIO_PinState PinState);
void              HAL_GPIO_TogglePin(uint16_t GPIO_Pin);
int               HAL_GPIO_Get_Mode(uint16_t GPIO_Pin);
int               gpio_oe_val_set(uint32_t bit,int oe_set);

#ifdef __cplusplus
}
#endif
#endif
