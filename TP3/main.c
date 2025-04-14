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
#include "ring.h"
#include "isr.h"

extern uint32_t irq_stack_top;
extern uint32_t stack_top;

char line[MAX_CHARS];
uint32_t nchars = 0;

void check_stacks() {
  void *memsize = (void*)MEMORY;
  void *addr;
  addr = &stack_top;
  if (addr >= memsize)
    panic();
  addr = &irq_stack_top;
  if (addr >= memsize)
    panic();
}

void rx_handler (uint32_t irq, void* cookie) { 
  uint32_t irqs = mmio_read32((void*)UART0_BASE_ADDRESS, UART_MIS);
  if (irqs & UART_IMSC_RXIM) {
    char code = uart_receive(UART0);
    while (code) {
      if (ring_full()) { panic(); }
      ring_put(code);
      code = uart_receive(UART0);
    }
    mmio_set((void*)UART0_BASE_ADDRESS, UART_ICR, UART_IMSC_RXIM);
    return;	
  }

  panic();
}

void process_ring () {
	uint8_t code;
	while (!ring_empty()) {
		code = ring_get();
		line[nchars++] = (char)code;
		uart_send(UART0, code);
		if (code == '\n') { nchars = 0; }
	}
}

/**
 * This is the C entry point,
 * upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */
void _start(void) {
  check_stacks();
  uarts_init();
  uart_enable(UART0);
  vic_setup_irqs();
  vic_enable_irq(UART0_IRQ, &rx_handler, NULL);
  core_enable_irqs();

  for (;;) {
	process_ring();
	core_disable_irqs();
	if (ring_empty()) {
		core_enable_irqs(); // obligatoire
		core_halt();
	}
  }

  core_disable_irqs();
}

void panic() {
  for(;;)
    ;
}

