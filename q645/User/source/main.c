/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <FreeRTOS.h>
#include <task.h>

#include<FreeRTOSConfig.h>
#include "sp645_hal_conf.h"
#include "sp645_it.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

extern void initialise_monitor_handles(void);

//#define IRQ_TEST	79	// CPU0_TO_2_DIRECT_INT7

#ifdef IRT_TEST
void isr_mbox7(void)
{
	u32 data = REG_MBOX7;
	REG_STAMP = data;
}
#endif

TIM_HandleTypeDef gTim;
void timer_init()
{
	gTim.Instance = TIM3;
	gTim.Init.ClockSource = CLK_SYS_SRC;
	gTim.Init.Prescaler = 59999; //16bit  600M/60000 = 10000tick
	gTim.Init.Counter = 10000; //1s
	gTim.Init.ReloadCounter = 10000;
	gTim.Init.AutoReloadPreload = REPEAT_MODE;
	gTim.IrqHandle = STC_IRQ_Handler;
	
	HAL_TIM_Init(&gTim);
	HAL_TIM_Start(&gTim);
	
	trace_info("[Timer] timer init \n");
}

int main ()
{

#ifdef IRT_TEST
	NVIC_SetVector(IRQ_TEST, (u32)isr_mbox7); // Set IRQ Handler
	NVIC_EnableIRQ(IRQ_TEST); // Enable Interrupt
	MBOX7 = IRQ_TEST; // Trigger
#endif
	
	HAL_Init(); //STC init 
	timer_init();
	
	trace_info("system start!!! \n");
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    taskDISABLE_INTERRUPTS();
    for( ;; ){};
}
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
    volatile unsigned long looping = 0;
    taskENTER_CRITICAL();
    {
        /* Use the debugger to set ul to a non-zero value in order to step out
                of this function to determine why it was called. */
        while( looping == 0LU )
        {
            portNOP();
        }
    }
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* func,uint32_t line)
{
    printf("[assert error] %s %d \n",func,line);

}
#endif
void vLoggingPrintf( const char *pcFormat, ... )
{
        va_list arg;

        va_start( arg, pcFormat );
        vprintf( pcFormat, arg );
        va_end( arg );
}
