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
#include "semphr.h"

#include<FreeRTOSConfig.h>
#include "sp645_hal_conf.h"
#include "sp645_uart.h"
#include "sp645_it.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//#define TIME_TEST
//#define IRQ_TEST
//#define GPIO_TEST
//#define UART_TEST

#define TASK_STACK_SIZE 128

//const char DateInfo[] = DATE_COMPILE;

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

extern void initialise_monitor_handles(void);
extern void vRegisterSampleCLICommands( void );
extern void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );

TIM_HandleTypeDef xTIMHandle = {0};
void prvTimerInit()
{
	xTIMHandle.Instance = TIM3;
	xTIMHandle.Init.ClockSource = CLK_SYS_SRC;
	xTIMHandle.Init.Prescaler = 59999; //16bit  600M/60000 = 10000tick
	xTIMHandle.Init.Counter = 10000; //1s
	xTIMHandle.Init.ReloadCounter = 10000;
	xTIMHandle.Init.AutoReloadPreload = REPEAT_MODE;
	xTIMHandle.IrqHandle = STC_IRQ_Handler;

	HAL_TIM_Init(&xTIMHandle);
	HAL_TIM_Start(&xTIMHandle);

	trace_info("[Timer] timer init \n");
}

#ifdef IRQ_TEST
SemaphoreHandle_t xBinarySemaphore1 = NULL;
SemaphoreHandle_t xBinarySemaphore2 = NULL;

void vMailboxISR1(void)
{
    	trace_info("entry vMAILBOXISR111.\n");
	u32 ulData = REG_MBOX7;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(xBinarySemaphore1, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

void vMailboxISR2(void)
{
    	trace_info("entry vMAILBOXISR222.\n");
	u32 ulData = REG_MBOX6;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(xBinarySemaphore2, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

void vTaskCode1(void *pArg)
{
	trace_info("entry vTaskCode1.\n");

	IRQn_Type irq1 = IPC_CA552CM4_INT7_IRQn;
	IRQn_Type irq2 = IPC_CA552CM4_INT6_IRQn;
	int pri;

	NVIC_SetVector(irq1, (u32)vMailboxISR1); // Set IRQ Handler
	NVIC_SetPriority(irq1, 0xE0);
	pri = NVIC_GetPriority(irq1);
	trace_info("@@@@@@@@@@pri %d.\n", pri);
	NVIC_EnableIRQ(irq1); // Enable Interrupt

	NVIC_SetVector(irq2, (u32)vMailboxISR2);
	NVIC_SetPriority(irq2, 0xE0);
	NVIC_EnableIRQ(irq2);

	vTaskDelay(pdMS_TO_TICKS(2));

	xBinarySemaphore1 = xSemaphoreCreateBinary();
	REG_MBOX7 = 1; // Trigger

	for(;;) {
		if (xSemaphoreTake(xBinarySemaphore1, portMAX_DELAY) == pdTRUE)
        		trace_info("Success to get xBinarySemaphore1.\n");
    	}
}

void vTaskCode2(void *pArg)
{
	trace_info("entry vTaskCode2.\n");

	vTaskDelay(pdMS_TO_TICKS(2));

	xBinarySemaphore2 = xSemaphoreCreateBinary();
	REG_MBOX6 = 1; // Trigger

	for(;;) {
		if (xSemaphoreTake(xBinarySemaphore2, portMAX_DELAY) == pdTRUE)
        		trace_info("Success to get xBinarySemaphore2.\n");
    	}
}
#endif

#ifdef GPIO_TEST
GPIO_InitTypeDef gpio;
static int trigger;
#define HIGH 1

void vHandleGPIO(void)
{
	trace_info("entry vHandleGPIO.\n");

	if (trigger == HIGH)
	{
		HAL_GPIO_WritePin(GPIO_89, GPIO_OUT_LOW);
	}
}

void vTaskCodeGPIO(void *pArg)
{
	int i = 0;
	IRQn_Type irq = EXTI0_IRQn;

	trace_info("entry vTaskCodeGPIO.\n");

	trigger = HIGH;

	gpio.Pin = GPIO_89;
	gpio.ds = GPIO_DS_DISABLE;
	gpio.Mode = GPIO_PP_OUTPUT_MODE;
	gpio.out_value = 0; //1:High 0:LOW
	HAL_GPIO_Init(&gpio);

	NVIC_SetVector(irq, (u32)vHandleGPIO); // Set IRQ Handler
	NVIC_SetPriority(irq, 0xE0);
	NVIC_EnableIRQ(irq); // Enable Interrupt
	REG(1, 4) = (1 << 11) | (1 << 27); //Set GPIO for extern interrupt 0

	if(trigger == HIGH)
	{
		/* 1 pulses , 1 seconds high level*/
		i = 2;

		while(1)
		{
			HAL_GPIO_WritePin(GPIO_89, GPIO_OUT_LOW);
			vTaskDelay(pdMS_TO_TICKS(5000));//delay 5 seconds
			HAL_GPIO_WritePin(GPIO_89, GPIO_OUT_HIGH);
			vTaskDelay(pdMS_TO_TICKS(5000));
			HAL_GPIO_WritePin(GPIO_89, GPIO_OUT_LOW);
		}

	}
}
#endif

int main ()
{
	trace_info("********** COMPILE DATE "DATE_COMPILE" **********\n");

#ifdef TIME_TEST
	HAL_Init(); //STC init
	prvTimerInit();
#endif

/* There is a sample case. For details about how to use UART interfaces,
 * see file UARTCommandConsole.c and serial.c.
 */
#ifdef UART_TEST
	vRegisterSampleCLICommands();
	vUARTCommandConsoleStart( TASK_STACK_SIZE, tskIDLE_PRIORITY + 1 );
#endif

#ifdef IRQ_TEST
	xTaskCreate(vTaskCode1, "Task1", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(vTaskCode2, "Task2", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
#endif

#ifdef GPIO_TEST
	xTaskCreate(vTaskCodeGPIO, "GPIO", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
#endif

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
