#ifndef __SP645_HAL_CONF_H_
#define __SP645_HAL_CONF_H_

#include <string.h>
#include <stdio.h>

#include "sp645_hal_def.h"
#include "sp645_hal.h"
#include "sp645_hal_cortex.h"
#include "sp645_hal_timer.h"
#include "sp645_hal_stc.h"
#include "trace.h"


#if !defined  (HSE_VALUE)
  #define HSE_VALUE            25000000U  /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */


#if !defined  (HSI_VALUE)
  #define HSI_VALUE            600000000U  /*!< Value of the Internal PLL in Hz*/
#endif /* HSI_VALUE */



/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
   
//#define USE_FULL_ASSERT    1 

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t *file, uint32_t line);
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))

#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */


#endif

