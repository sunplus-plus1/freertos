/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "platform.h"
#include "stub.h"
#include "weak_under_alias.h"

#if (TARGET == 143)
#define SP_UART_DATA          0x00
#define SP_UART_LSR           0x04
#define UART_REG_STATUS_RX    (1 << 1)  /* 1: receive fifo not empty */
#define UART_REG_STATUS_TX    (1 << 0)  /* 1: transmit fifo is not full */
#define UART_REG_STATUS_TXE   (1 << 6)  /* 1: transmit fifo is empty */
#if 1
#undef UART0_CTRL_ADDR
#define UART0_CTRL_ADDR _AC(0x9C000980,UL) // uart1
#endif
#define TX SP_UART_DATA
#define RX SP_UART_DATA
#define TX_READY ((UART0_REG(SP_UART_LSR) & UART_REG_STATUS_TXE) != 0)
#define RX_READY ((UART0_REG(SP_UART_LSR) & UART_REG_STATUS_RX) != 0)
#else
#define TX UART_REG_TXFIFO
#define RX UART_REG_RXFIFO
#define TX_READY ((UART0_REG(UART_REG_TXFIFO) & 0x80000000) == 0)
#define RX_READY (*(volatile uint8_t *)(UART0_CTRL_ADDR + UART_REG_RXCTRL + 2) != 0)
#endif

void platform_putchar(char ch)
{
  while (!TX_READY);
#if 0//(TARGET != 143)
  {
    static char cc = '\r';
    if (cc == '\r') UART0_REG(TX) = '@'; // prefix
    cc = ch;
  }
#endif
  UART0_REG(TX) = ch;
}

ssize_t __wrap_read(int fd, void* ptr, size_t len)
{
  uint8_t * current = (uint8_t *)ptr;
  ssize_t result = 0;

  if (isatty(fd)) {
    for (current = (uint8_t *)ptr;
        (current < ((uint8_t *)ptr) + len) && RX_READY;
        current ++) {
      *current = UART0_REG(RX);
      result++;
    }
    return result;
  }

  return _stub(EBADF);
}
weak_under_alias(read);
