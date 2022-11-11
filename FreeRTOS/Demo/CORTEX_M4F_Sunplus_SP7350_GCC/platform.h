#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "trace.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef volatile u32 * reg;

#define REG_1(g, i)		((reg)0xf8000000)[g * 32 + i]
#define REG_AO_1(g, i)		((reg)0xf8800000)[g * 32 + i]
#define REG(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + 0xf8000000)
#define REG_AO(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + 0xf8800000)

#define STAMP		REG_AO_1(0, 0)
#define STAMP_1		REG_1(0, 0)
#define UART_0		REG_AO_1(50, 0)
#define UART_1		REG_AO_1(51, 0)
#define MBOX7		REG_1(258, 31)	// cpu0 to cpu2 direct reg07
#define MBOX6		REG_1(258, 30)	// cpu0 to cpu2 direct reg07

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
#define UART_REG ((volatile UART_t *)&UART_1)

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




struct pmc_regs_AO {
	unsigned int pmc_ctrl; 
	unsigned int pmc_timer; 
	unsigned int pmc_timer2; 
	unsigned int pmc_main_dowmain_pwd_1;
	unsigned int pmc_main_dowmain_pwd_11; 
	unsigned int pmc_reserve5;
	unsigned int pmc_reserve6;
	unsigned int pmc_xtal27m_pwd_h;
	unsigned int pmc_xtal27m_pwd_l;
	unsigned int pmc_wakeup_latch;
	unsigned int pmc_ivmx_reg;
	unsigned int pmc_reserve11;
	unsigned int pmc_reserve12;
	unsigned int pmc_reserve13;
	unsigned int pmc_reserve14; 
	unsigned int pmc_main_pwr_ctrl;
	unsigned int pmc_corepsw_pwd ;
	unsigned int pmc_corepsw_en;
	unsigned int pmc_coremem_sdpwd;
	unsigned int pmc_coremem_sden;
	unsigned int pmc_lvs_pwd;
	unsigned int pmc_lvs_disable;
	unsigned int pmc_iso_pwd; 
	unsigned int pmc_iso_en;
	unsigned int pmc_reserve24;
	unsigned int pmc_reserve25; 
	unsigned int pmc_reserve26; 
	unsigned int pmc_reset_vector;
	unsigned int pmc_ca55_reset_state; 
	unsigned int pmc_pctl_reg;
	unsigned int pmc_pctl_int_clr;
	unsigned int pmc_reserve31; 
};
#define PMC_REGS ((volatile struct pmc_regs_AO *) REG_AO(36, 0))

struct moon0_regs {
	unsigned int m0_sft_cfg[32];
};
#define MOON0_REG ((volatile struct moon0_regs *) REG_AO(0, 0))

struct moon3_regs {
	unsigned int m3_sft_cfg[32];
};
#define MOON3_REG ((volatile struct moon3_regs *) REG_AO(3, 0))


#endif // __PLATFORM_H__
