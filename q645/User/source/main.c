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
	//HAL_TIM_Start(&xTIMHandle);

	trace_info("[Timer] timer init \n");
}

#if 0
I2C_HandleTypeDef gI2C;
void i2c_init()
{
	gI2C.Instance = SP_I2CM0;
	gI2C.Index = 0;
	gI2C.gdma = SP_GDMA0;
	HAL_Module_Clock_enable(I2CM0 + gI2C.Index, 1);
	HAL_Module_Clock_gate(I2CM0 + gI2C.Index, 1);
	HAL_Module_Reset(I2CM0 + gI2C.Index, 0);

	HAL_PINMUX_Cfg(PINMUX_I2C_0 + gI2C.Index, 1);

	/* Set frequency */
	gI2C.Init.Timing = 100;
	gI2C.State = HAL_I2C_STATE_RESET;

	trace_info("[I2C] i2c init \n");
}
#endif

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

#ifdef UART_TEST
void vTaskCodeTx(void *pArg)
{
	uint8_t *temp_buf;
	size_t i, j;
	uint8_t flag = 0;
	uint32_t trans_size;

	trace_info("entry vTaskCodeTx().\n");

	trans_size = 1024;
	temp_buf = malloc(trans_size);

	for(i = 0; i < (trans_size / 8) + 1; i++) {
		for(j = 0; j < 8; j++)
			temp_buf[i * 8 + j] = 0x30 + j;
	}

	vUartStartTx(UART2_INDEX, temp_buf, trans_size, &flag);
	while(!flag) {
		free(temp_buf);
		vTaskDelete(NULL);
	}
}

void vTaskCodeRx(void *pArg)
{
	uint8_t *temp_buf1;
	uint32_t i, j;

	volatile int ret;

	trace_info("entry vTaskCodeRx().\n");

	vUartStartRx(UART1_INDEX);

	while(1) {
		/* return 0 -> data available */
		ret = vUartOutput(UART1_INDEX);
		if(ret != -1) {
			//Don't call printf(), it spend too much time to get the data out of buffer in time.
			//printf("get %c\n", ret);
		}
	}

}
#endif

int main ()
{
	trace_info("********** COMPILE DATE "DATE_COMPILE" **********\n");

	HAL_Init(); //STC init
	prvTimerInit();
	vUartInit(UART1_INDEX, 115200, 0xE0);
	vUartInit(UART2_INDEX, 115200, 0xE0);

#ifdef IRQ_TEST
	xTaskCreate(vTaskCode1, "Task1", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(vTaskCode2, "Task2", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
#endif

#ifdef GPIO_TEST
	xTaskCreate(vTaskCodeGPIO, "GPIO", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
#endif

#ifdef UART_TEST
#if 1
	xTaskCreate(vTaskCodeTx, "Uart2Tx", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vTaskCodeRx, "Uart1Rx", TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
#endif
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
