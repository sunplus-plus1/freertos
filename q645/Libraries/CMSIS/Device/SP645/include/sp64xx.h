#ifndef __SP64XX_H
#define __SP64XX_H

#include "sp645_cm4.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif


typedef enum
{
	RESET = 0,
	SET = !RESET
} FlagStatus;
  


typedef enum
{
	DISABLE = 0,
	ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#ifndef TRUE
#define TRUE    1
#define true    1
#endif

#ifndef FALSE
#define FALSE    0
#define false    0
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

typedef enum 
{
    ERROR = 0,
    SUCCESS = !ERROR
} ErrorStatus;


#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))


#include "sp645_hal.h"



#ifdef __cplusplus
}
#endif

#endif 

