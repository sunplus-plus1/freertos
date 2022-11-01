#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "sp645_uart.h"
#include "sp645_hal_conf.h"

q645_uart_TypeDef sTemp_q645[UART_NUM];
UART_HandleTypeDef xUARTHandle[UART_NUM];

uint8_t serial_rx_active(UART_HandleTypeDef *huart)
{
	return ((HAL_UART_GetState(huart) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX);
}

/*
 * @brief	Tx Transfer completed callback
 * @param	UartHandle pointer on the uart reference
 * @retval	None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t ucIndex = huart->uart_idx;
	q645_uart_TypeDef *sTemp = &sTemp_q645[ucIndex];
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t copy_size;

	trace_debug("[UART%d] %d\n", ucIndex, sTemp->tx_cnt);
	if (sTemp->tx_cnt < sTemp->data_size) {
		if (sTemp->loop = sTemp->data_size / UART_BUFFER_SIZE)
			copy_size = UART_BUFFER_SIZE;
		else
			copy_size = sTemp->data_size % UART_BUFFER_SIZE;

		memcpy(sTemp->fixed_txbuf, sTemp->usr_buf + sTemp->tx_cnt, copy_size);
		sTemp->tx_cnt += copy_size;
		ret = HAL_UART_Transmit_IT(huart, sTemp->fixed_txbuf, copy_size);
		if (ret)
			trace_error("[UART%d] Tx isn't ready!\n", ucIndex);
	} else {
		*sTemp->flag_end = 1;
	}
}

/*
 * @brief	rx Transfer completed callback
 * @param	UartHandle pointer on the uart reference
 * @retval	None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

/*
 * @brief	UART callback which is registered to NVIC
 * @param	UartHandle pointer on the uart reference
 * @retval	None
 */
void vUartCallback(void)
{
	uint32_t ulCurrentInterrupt;
	uint8_t ucIndex;

	__asm volatile ( "mrs %0, ipsr" : "=r" ( ulCurrentInterrupt )::"memory" );

	switch(ulCurrentInterrupt - 16) {
	case UART1_IRQn:
		ucIndex = UART1_INDEX;
		break;
	case UART2_IRQn:
		ucIndex = UART2_INDEX;
		break;
	case UART3_IRQn:
		ucIndex = UART3_INDEX;
		break;
	default:
		trace_error("[UART%d] Invalid UART!\n", ucIndex);
	}

	HAL_UART_IRQHandler(&xUARTHandle[ucIndex]);
}

/*
 * @brief	Initialize the UART
 * @param	Uart number in use
 * @param	Baud rate of UART
 * @param	Priority of UART interrupt
 * @retval	None
 */
void vUartInit(uint8_t ucIndex, uint32_t ulBaudRate, uint8_t ucPriority)
{
	IRQn_Type irq;
	UART_CTRL_Type *instance;

	switch(ucIndex) {
	case UART1_INDEX:
		irq = UART1_IRQn;
		instance = SP_UART1;
		break;
	case UART2_INDEX:
		irq = UART2_IRQn;
		instance = SP_UART2;
		break;
	case UART3_INDEX:
		irq = UART3_IRQn;
		instance = SP_UART3;
		break;
	default:
		trace_error("[UART%d] Invalid UART!\n", ucIndex);

	}

	/* set pin ctrl */
	HAL_PINMUX_Cfg(PINMUX_UART0 + ucIndex, 1);
	/* enable clk */
	HAL_Module_Clock_enable(UART0 + ucIndex, 1);
	HAL_Module_Clock_gate(UART0 + ucIndex, 1);
	HAL_Module_Reset(UART0 + ucIndex, 0);

	xUARTHandle[ucIndex].Instance = instance;
	xUARTHandle[ucIndex].Init.BaudRate = ulBaudRate;
	xUARTHandle[ucIndex].uart_idx = ucIndex;

	//xUARTHandle.Init.BaudRate = 115200;
	//xUARTHandle.Init.BaudRate = 680000;//TX PASS
	xUARTHandle[ucIndex].Init.LcrInit.LCR_Init = UART_INIT_USE_DEFAULT_VALUE;
	xUARTHandle[ucIndex].Init.McrInit.MCR_Init = UART_INIT_USE_DEFAULT_VALUE;
	xUARTHandle[ucIndex].txdma = NULL;
	xUARTHandle[ucIndex].txgdma = NULL;
	xUARTHandle[ucIndex].rxdma = NULL;

	NVIC_SetVector(irq, (uint32_t)vUartCallback); // Set IRQ Handler
	NVIC_SetPriority(irq, ucPriority); //why must set < 191, different from MAILBOX?
	NVIC_EnableIRQ(irq); // Enable Interrupt

	HAL_UART_Init(&xUARTHandle[ucIndex]);

#if 0 //DEBUG fifo size
	vTaskDelay(pdMS_TO_TICKS(5000));
	int debug_fifo = 0;
	while (xUARTHandle[ucIndex].Instance->lsr & SP_UART_LSR_RX) {
		debug_fifo ++;
		char ch = READ_REG(xUARTHandle[ucIndex].Instance->dr);
		trace_info("%d %c\n", debug_fifo, ch);
	}
#endif
	trace_info("[UART%d] Init done!\n", ucIndex);
}

/*
 * @brief	Start the UART transmit
 * @param	Uart number in use
 * @param	The buffer of source data
 * @param	The length of source data
 * @param	The flag of the end of transmit
 * @retval	None
 */
void vUartStartTx(uint8_t ucIndex, uint8_t *pucBuffer, uint32_t ulSize, uint8_t *pcFlag)
{
	sTemp_q645[ucIndex].tx_cnt = 0;
	sTemp_q645[ucIndex].data_size = ulSize;
	sTemp_q645[ucIndex].usr_buf = pucBuffer;
	sTemp_q645[ucIndex].flag_end = pcFlag;

	HAL_UART_TxCpltCallback(&xUARTHandle[ucIndex]);

	trace_info("[UART%d] TX start!\n", ucIndex);
}

/*
 * @brief	Start the UART receive
 * @param	Uart number in use
 * @retval	None
 */
void vUartStartRx(uint8_t ucIndex)
{
	HAL_StatusTypeDef ret = HAL_OK;

	ret = HAL_UART_Receive_IT(&xUARTHandle[ucIndex]);
	if (ret) {
		trace_error("[UART%d] RX isn't ready!\n", ucIndex);
	} else {
		trace_info("[UART%d] RX ready!\n", ucIndex);
	}
}

/*
 * @brief	End the UART transmit
 * @param	Uart number in use
 * @retval	None
 */
void vUartEndTx(uint8_t ucIndex)
{
	int ret;

	ret = HAL_UART_AbortTransmit(&xUARTHandle[ucIndex]);
	if (!ret)
		trace_info("[UART%d] TX end!\n", ucIndex);
}

/*
 * @brief	End the UART receive
 * @param	Uart number in use
 * @retval	None
 */
void vUartEndRx(uint8_t ucIndex)
{
	int ret;

	ret = HAL_UART_AbortReceive(&xUARTHandle[ucIndex]);
	if (!ret)
		trace_info("[UART%d] RX end!\n", ucIndex);
}

/*
 * @brief	Receive single byte data
 * @param	Uart number in use
 * @retval	character data received
 */
int debug_cnt3 = 0;
int vUartOutput(uint8_t ucIndex)
{
	uint8_t c;
	BaseType_t ret;

	/* Load data from circle buffer */
	ret = xQueueReceive(xUARTHandle[ucIndex].queue, &c, portMAX_DELAY);
	if(ret == pdFALSE)
		return -1;

	/* Just to simply verify that the data is not missing (receive 512 bytes) */
	debug_cnt3 ++;
	if (debug_cnt3 == 512) {
		trace_debug("%d !!! %c\n", debug_cnt3, c);
	}

	/* The CPU will always execute code faster than the UART data transfer,
	 * regardless of whether the fifo will be full. Maybe remove this function.
	 * e.x.
	 *	assume fifo size is 256 bytes, UART baud rate is 115200
	 *	when FIFO is full, the taken time is
	 *      256 * 8 / 115200 ≈ 17.78ms
	 *	Cortex M4 freq 200MHz, 1.25 DMISP/MHz
	 *	An instruction taken time is 4ns
	 */
#if 0
	if(xUARTHandle[ucIndex].rx_full) {
		xUARTHandle[ucIndex].rx_full = 0;
		HAL_UART_Start_RX_IRQ(&xUARTHandle[ucIndex]);
	}
#endif

	return c;
}
