#include <FreeRTOS.h>
#include <task.h>
#include "sp645_uart.h"
#include "sp645_hal_conf.h"

q645_uart_TypeDef sTemp_q645[UART_NUM];
UART_HandleTypeDef xUARTHandle[UART_NUM];

/**
	* @brief	Tx Transfer completed callback
	* @param	UartHandle pointer on the uart reference
	* @retval None
	*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t ucIndex = huart->uart_idx;
	q645_uart_TypeDef *sTemp = &sTemp_q645[ucIndex];
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t copy_size;

	trace_debug("[UART%d] HAL_UART_TxCpltCallback   %d\n", ucIndex, sTemp->tx_cnt);
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
	}
}

#ifdef TO_FIX
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint32_t offset, copy_size;
	volatile uint8_t value, ret;
	uint8_t ucIndex = huart->uart_idx;
	q645_uart_TypeDef *sTemp = &sTemp_q645[ucIndex];
	struct circ_buf *queue = &sTemp->queue;

#if 0
	offset = sTemp->rx_cnt;

	if (huart->rear + 1 < huart->front) {

		copy_size = ARRAY_SIZE - huart->front;
		memcpy(sTemp->usr_buf + offset, sTemp->fixed_rxbuf + huart->front, copy_size);

		offset += copy_size;
		copy_size = huart->rear + 1;
		memcpy(sTemp->usr_buf + offset, sTemp->fixed_rxbuf, copy_size);

	} else {
		copy_size = huart->rear + 1 - huart->front;
		memcpy(sTemp->usr_buf + offset, sTemp->fixed_rxbuf + huart->front, copy_size);
	}
	huart->front = huart->rear + 1;//empty
	sTemp->rx_cnt += huart->rx_index;
#else
	while(1) {
		trace_debug("0x%x  0x%x\n", huart->pRxBuffPtr[huart->rx_queue.head], huart->pRxBuffPtr[huart->rx_queue.tail]);

		ret = _delete(&huart->rx_queue, (char *)&value);
		if (ret)
			break;
		sTemp->usr_buf[sTemp->rx_cnt] = value;

		trace_debug("%d  0x%x\n", sTemp->rx_cnt, sTemp->usr_buf[sTemp->rx_cnt]);
		sTemp->rx_cnt ++;
	}
#endif
}
#endif

//static TaskHandle_t xTaskRx = NULL;

uint8_t serial_rx_active(UART_HandleTypeDef *huart)
{
	return ((HAL_UART_GetState(huart) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX);
}
int cnt111 = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t c;
	HAL_StatusTypeDef ret = HAL_OK;
	//BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t ucIndex = huart->uart_idx;
	q645_uart_TypeDef *sTemp = &sTemp_q645[ucIndex];
	struct circ_buf *queue = &sTemp->queue;

	cnt111 ++;

	do {
		if (serial_rx_active(huart)) {
			trace_error("[UART%d] Data Reception process is ongoing!\n", ucIndex);
			break;
		}

		trace_debug("%d xxxxxxxx tail %d head %d \n", cnt111, queue->tail, queue->head);

		c = sTemp->recv;
		_insert(queue, c);

		/* Restart RX irq */
		ret = HAL_UART_Receive_IT(huart, &sTemp->recv, 1);
		if (ret) {
			trace_error("[UART%d] Rx isn't ready!\n", ucIndex);
			break;
		}

		//vTaskNotifyGiveFromISR(xTaskRx, &xHigherPriorityTaskWoken);
		//portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		//TEST
#if 0
		_delete(queue,&c);
		sTemp->usr_buf[sTemp->rx_cnt] = c;
		sTemp->rx_cnt++;
#endif
	} while(0);

}

void vUartCallback(void)
{
	uint32_t ulCurrentInterrupt;
	uint8_t ucIndex;

	//trace_info("entry vUartCallback.\n");
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

	trace_info("[UART%d] Init done!\n", ucIndex);
}

#if 0 // notify not work
void vTaskRxDataPrint(void *pArg)
{
	uint32_t i, j;

	UNUSED(pArg);

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		trace_info("get %c", vUartOutput());
	}

}
#endif

void vUartStartTx(uint8_t ucIndex, uint8_t *pucBuffer, uint32_t ulSize)
{
	sTemp_q645[ucIndex].tx_cnt = 0;
	sTemp_q645[ucIndex].data_size = ulSize;
	sTemp_q645[ucIndex].usr_buf = pucBuffer;

	HAL_UART_TxCpltCallback(&xUARTHandle[ucIndex]);
}

void vUartStartRx(uint8_t ucIndex, uint8_t *pucBuffer, uint32_t ulSize)
{
	struct circ_buf *queue = &sTemp_q645[ucIndex].queue;

	/* Init the queue */
	queue->tail = 0;
	queue->head = 1;
	queue->buf = sTemp_q645[ucIndex].fixed_rxbuf;
	queue->size = ARRAY_SIZE;

	sTemp_q645[ucIndex].rx_cnt = 0;
	sTemp_q645[ucIndex].usr_buf = pucBuffer;
#ifdef TO_FIX
	HAL_UART_Receive_IT(&xUARTHandle[ucIndex], sTemp_q645[ucIndex].fixed_rxbuf, UART_BUFFER_SIZE);
#endif
	HAL_UART_Receive_IT(&xUARTHandle[ucIndex], &sTemp_q645[ucIndex].recv, 1);

	//xTaskCreate(vTaskRxDataPrint, "RxDataPrint", 128, NULL, tskIDLE_PRIORITY+1, &xTaskRx);

	trace_info("[UART%d] RX start!\n", ucIndex);
}
int cnt = 0;
int vUartOutput(uint8_t ucIndex)
{
	uint8_t c, ret;
	struct circ_buf *queue = &sTemp_q645[ucIndex].queue;

	trace_debug("!!! tail %d head %d \n", queue->tail, queue->head);

	ret = _delete(queue, &c);

	if(!ret) {
		cnt++;
		trace_debug("[UART%d] %d vUartOutput %c\n", ucIndex, cnt, c);
		return c;
	} else {
		return -1;
	}
}
