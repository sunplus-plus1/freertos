#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "trace.h"

typedef unsigned int u32;
typedef volatile u32 * reg;

#define REG(g, i)	((reg)0xf8000000)[g * 32 + i]
#define STAMP		REG(0, 0)
#define UART0		REG(18, 0)
#define UART1		REG(19, 0)
#define MBOX7		REG(255, 31)	// cpu0 to cpu2 direct reg07

typedef struct UART_t {
    unsigned int dr;  /* data register */
    unsigned int lsr; /* line status register */
    unsigned int msr; /* modem status register */
    unsigned int lcr; /* line control register */
    unsigned int mcr; /* modem control register */
    unsigned int div_l;
    unsigned int div_h;
    unsigned int isc; /* interrupt status/control */
    unsigned int txr; /* tx residue */
    unsigned int rxr; /* rx residue */
    unsigned int thr; /* rx threshold */
	unsigned int clk_src;
} UART_t;
#define UART_REG ((volatile UART_t *)&UART1)

#define UART_LSR_TX_RDY     (1 << 0)
#define UART_LSR_RX_RDY     (1 << 1)
#define UART_TX_EMPTY       (1 << 6)

#define UART_tx_rdy()       (UART_REG->lsr & UART_LSR_TX_RDY)
#define UART_rx_rdy()       (UART_REG->lsr & UART_LSR_RX_RDY)
#define UART_tx_empty()     (UART_REG->lsr & UART_TX_EMPTY)
#define UART_putc_nw(c)     (UART_REG->dr = (c))
#define UART_getc()         (UART_REG->dr)

#define UART_wait()         do { while (!UART_tx_rdy()) ;} while (0)
#define UART_flush()        do { while (!UART_tx_empty()) ;} while (0)
#define UART_putc(c)        do { UART_wait(); UART_putc_nw(c); } while (0)
#define UART_putc_nl(c)     do { UART_putc(c); if (c == 0x0a) UART_putc(0x0d);} while (0)
#define UART_puts(s) \
do { \
	int __c; \
	const char *__s = (const char *)(s); \
	while ((__c = *__s++)) { \
		UART_putc_nl(__c); \
	} \
} while (0)

#endif // __PLATFORM_H__
