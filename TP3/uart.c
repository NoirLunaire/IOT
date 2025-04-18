/*
 * Copyright: Olivier Gruber (olivier dot gruber at acm dot org)
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "main.h"
#include "uart.h"
#include "uart-mmio.h"

struct uart {
  uint8_t uartno; // the UART numéro
  void* bar;      // base address register for this UART
//  void (*rl)(uint8_t no);
//  void (*wl)(uint8_t no);
//  void* cookie;
};

static
struct uart uarts[NUARTS];

static
void uart_init(uint32_t uartno, void* bar) {
  struct uart*uart = &uarts[uartno];
  uart->uartno = uartno;
  uart->bar = bar;
  // no hardware initialization necessary
  // when running on QEMU, the UARTs are
  // already initialized, as long as we
  // do not rely on interrupts.
}


void uarts_init() {
  uart_init(UART0,UART0_BASE_ADDRESS);
  uart_init(UART1,UART1_BASE_ADDRESS);
  uart_init(UART2,UART2_BASE_ADDRESS);
}

void uart_enable(uint32_t uartno) {
  struct uart*uart = &uarts[uartno];
  // nothing to do here, as long as
  // we do not rely on interrupts 
  // mmio_set(uart->bar, UART_IMSC, UART_IMSC_TXIM); 
  mmio_set(uart->bar, UART_IMSC, UART_IMSC_RXIM);
}

void uart_disable(uint32_t uartno) {
  struct uart*uart = &uarts[uartno];
  // nothing to do here, as long as
  // we do not rely on interrupts
  mmio_clear(uart->bar, UART_IMSC, UART_IMSC_RXIM);
  mmio_clear(uart->bar, UART_IMSC, UART_IMSC_TXIM);
}

char uart_receive(uint8_t uartno) {
  struct uart*uart = &uarts[uartno];
  if (mmio_read8(uart->bar, UART_FR) & UART_FR_RXFE) {
	return 0;
  }
  return (char)mmio_read8(uart->bar, UART_DR);
}

/**
 * Sends a character through the given uart, this is a blocking call
 * until the character has been sent.
 */
void uart_send(uint8_t uartno, char s) {
  struct uart* uart = &uarts[uartno];
  if (mmio_read8(uart->bar, UART_FR) & UART_FR_TXFF) { return; }
  mmio_write8(uart->bar, UART_DR, s);
}

/**
 * This is a wrapper function, provided for simplicity,
 * it sends a C string through the given uart.
 */
void uart_send_string(uint8_t uartno, const char *s) {
  while (*s != '\0' || *s != '\n') {
    uart_send(uartno, *s);
    s++;
  }
}

