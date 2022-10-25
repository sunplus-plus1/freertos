#ifndef __SP645_UART_H_
#define __SP645_UART_H_
#include "sp645_hal_def.h"
#include "sp645_cm4.h"
#include "sp64xx.h"

#define UART_BUFFER_SIZE	64
#define USED_SIZE	UART_BUFFER_SIZE
#define ARRAY_SIZE	(UART_BUFFER_SIZE + 1)

typedef enum {
	UART0_INDEX=0,
	UART1_INDEX,
	UART2_INDEX,
	UART3_INDEX,
	UART_NUM
} uart_index_t;

typedef struct __q645_uart
{
	uint32_t tx_cnt;
	uint32_t rx_cnt;
	uint32_t data_size;
	uint8_t *usr_buf; //user data
	uint8_t fixed_txbuf[ARRAY_SIZE];
	uint8_t fixed_rxbuf[ARRAY_SIZE];
	uint32_t loop;
	struct circ_buf queue;
	uint8_t recv; //1 byte space to store the data temporary
} q645_uart_TypeDef;

void vUartInit(uint8_t ucIndex, uint32_t ulBaudRate, uint8_t ucPriority);

int vUartOutput(uint8_t ucIndex);

void vUartStartTx(uint8_t ucIndex, uint8_t *pucBuffer, uint32_t ulSize);
void vUartStartRx(uint8_t ucIndex, uint8_t *pucBuffer, uint32_t ulSize);


#endif
