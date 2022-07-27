#include <stdio.h>
#include "sp645_it.h" 
#include "sp645_hal_conf.h"

void STC_IRQ_Handler(void)
{
	trace_info("stc irq,stc tick=%d  \n",HAL_GetTick());
}

