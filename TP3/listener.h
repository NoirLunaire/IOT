#ifndef LISTENER_H_
#define LISTENER_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ring.h"
#include "uart.h"

typedef uint8_t bool_t;

struct cookie {
	uint32_t uartno;
	char line[MAX_CHARS];
	uint32_t head, tail;
	bool_t processing;
};
struct cookie cookie;

void read_listener (void* cookie);
void write_listener (void* cookie);
void write_amap(struct cookie* cookie);

void app_start () {
	uart_init(UART0, &read_listener, &write_listener, &cookie);
}

#endif
