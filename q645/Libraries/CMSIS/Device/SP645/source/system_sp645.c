#include "sp64xx.h"

#include <stdio.h>
#include <stdint.h>

uint32_t SystemCoreClock = HSI_VALUE;


void SystemInit (void)
{
	/*system  init*/
}

void SystemCoreClockUpdate (void)
{
	SystemCoreClock = HSI_VALUE;

}
