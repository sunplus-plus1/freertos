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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "sp645_uart.h"

/* Demo application includes. */
#include "serial.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE				( ( QueueHandle_t ) 0 )
#define serNO_BLOCK					( ( TickType_t ) 0 )
#define serTX_BLOCK_TIME				( 40 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

/* UART interrupt handler. */
void vUARTInterruptHandler( void );

/*-----------------------------------------------------------*/

static xComPort xPorts[ serMAX_PORTS ];

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
	IRQn_Type irq;
	UART_CTRL_Type *instance;
	eCOMPort ePort = serCOM1;
	xComPortHandle pxPort = &xPorts[ePort];

	/* Create the queues used to hold Rx/Tx characters. */
	pxPort->xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	pxPort->xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	/* If the queue/semaphore was created correctly then setup the serial port hardware. */
	if( ( pxPort->xRxedChars != serINVALID_QUEUE ) && ( pxPort->xCharsForTx != serINVALID_QUEUE ) )
	{
		switch(ePort) {
		case serCOM1:
			irq = UART1_IRQn;
			instance = SP_UART1;
			break;
		case serCOM2:
			irq = UART2_IRQn;
			instance = SP_UART2;
			break;
		case serCOM3:
			irq = UART3_IRQn;
			instance = SP_UART3;
			break;
		default:
			trace_error("[SerCOM%d] Init failed!\n", ePort);

		}

		/* Set pin ctrl */
		HAL_PINMUX_Cfg(PINMUX_UART0 + ePort, 1);

		/* Enable clk */
		HAL_Module_Clock_enable(UART0 + ePort, 1);
		HAL_Module_Clock_gate(UART0 + ePort, 1);
		HAL_Module_Reset(UART0 + ePort, 0);

		pxPort->xUart.Instance = instance;
		pxPort->xUart.Init.BaudRate = ulWantedBaud;
		pxPort->xUart.uart_idx = ePort;

		pxPort->xUart.Init.LcrInit.LCR_Init = UART_INIT_USE_DEFAULT_VALUE;
		pxPort->xUart.Init.McrInit.MCR_Init = UART_INIT_USE_DEFAULT_VALUE;
		pxPort->xUart.txdma = NULL;
		pxPort->xUart.txgdma = NULL;
		pxPort->xUart.rxdma = NULL;

		/* Set IRQ Handler */
		NVIC_SetVector(irq, (uint32_t)vUARTInterruptHandler);
		NVIC_SetPriority(irq, 0xE0);
		NVIC_EnableIRQ(irq);

		HAL_UART_Init(&pxPort->xUart);

		HAL_UART_ITConfig( &pxPort->xUart, UART_IT_RX, ENABLE);

		trace_info("[SerCOM%d] Init done!\n", ePort);
	}
	else
	{
		return NULL;
	}

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return pxPort;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( pxPort->xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/
#ifdef TX_MEMCPY
void vSerialPutStringQuick( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
	signed char *pxNext;
	unsigned short usByte;
	int loop, i, copysize;

	loop = usStringLength / TX_ARRARY_SIZE;
	if (usStringLength % TX_ARRARY_SIZE)
		loop ++;

	pxNext = ( signed char * ) pcString;

	for ( i = 0; i < loop; i ++ )
	{
		/* last data copy size */
		if( i == (loop - 1) )
			copysize = (usStringLength % TX_ARRARY_SIZE) ? (usStringLength % TX_ARRARY_SIZE) : TX_ARRARY_SIZE;
		else
			copysize = TX_ARRARY_SIZE;

		memcpy(pxPort->cTxBuf, pxNext, copysize);
		pxNext += copysize;
		pxPort->ulTxCnt = copysize;

		HAL_UART_ITConfig( &pxPort->xUart, UART_IT_TX, ENABLE);
	}
}

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	vSerialPutString(pxPort, &cOutChar, 1);

	return pdPASS;
}

#else /*Normal put string*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
	signed char *pxNext;
	unsigned short usByte;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	for( usByte = 0; usByte < usStringLength; usByte ++ )
	{
		//xSerialPutChar( pxPort, *pxNext, portMAX_DELAY );
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/
signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	signed portBASE_TYPE xReturn = pdFAIL;

	if( xQueueSend( pxPort->xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
	{

		HAL_UART_ITConfig( &pxPort->xUart, UART_IT_TX, ENABLE);
		xReturn = pdPASS;

	}
	else
	{
		//Enable interrupt
		xReturn = pdFAIL;
	}

	return xReturn;
}
#endif
/*-----------------------------------------------------------*/
void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/
int debug_cnt2 = 0;
int debug_cntkk = 0;
void vUARTInterruptHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	char cChar;
	unsigned long ulCurrentInterrupt, ulTempCnt;
	eCOMPort ePort;
	xComPortHandle pxPort;
	xUartHandle pxhuart;
#ifdef TX_MEMCPY
	unsigned long ulTempCnt;
#endif

	__asm volatile ( "mrs %0, ipsr" : "=r" ( ulCurrentInterrupt )::"memory" );

	switch(ulCurrentInterrupt - 16) {
	case UART1_IRQn:
		ePort = serCOM1;
		break;
	case UART2_IRQn:
		ePort = serCOM2;
		break;
	case UART3_IRQn:
		ePort = serCOM3;
		break;
	default:
		trace_error("[serCOM%d] Invalid serCOM!\n", ePort);
	}

	pxPort = &xPorts[ePort];
	pxhuart = &pxPort->xUart;

#ifdef TX_MEMCPY
		while( ulTempCnt < pxPort->ulTxCnt )
		{
			debug_cntkk ++;
			cChar = *( pxPort->cTxBuf + ulTempCnt);
			HAL_UART_SendData( pxhuart, cChar );

			if ( ulTempCnt == ( pxPort->ulTxCnt - 1 ) )
			{
				ulTempCnt = 0;

				HAL_UART_ITConfig( pxhuart, UART_IT_TX, DISABLE);
				while( !HAL_UART_IsTxFifoEmpty( pxhuart ) );
				break;
			}
			ulTempCnt ++;
		}
#else
	if( HAL_UART_GetITStatus( pxhuart, UART_IT_TX ) == SET )
	{

		/* The interrupt was caused by FIFO empty. */
		do
		{
			if (xQueueReceiveFromISR( pxPort->xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE)
			{
				/* A character was retrieved from the queue so can be sent to the
				fifo now. */
				HAL_UART_SendData( pxhuart, cChar );
			}
		} while( !HAL_UART_IsTxFifoEmpty( pxhuart ) );

		//Disable tx irq
		HAL_UART_ITConfig( pxhuart, UART_IT_TX, DISABLE);
#endif
	}

	if( HAL_UART_GetITStatus( pxhuart, UART_IT_RX ) == SET )
	{
		while( !HAL_UART_IsRxFifoEmpty( pxhuart ) ) {
			cChar = HAL_UART_ReceiveData( pxhuart );
			xQueueSendFromISR( pxPort->xRxedChars, &cChar, &xHigherPriorityTaskWoken );
		}
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

