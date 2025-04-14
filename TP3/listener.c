#include "listener.h"

/*
	uint32_t uartno;
	char line[MAX_CHARS];
	uint32_t head, tail;
	bool_t processing;
*/

bool_t ring_empty (void* addr) {
	struct cookie* cookie = addr;
	return cookie->head == cookie->tail;
}

bool_t ring_full (void* addr) {
	struct cookie* cookie = addr;
	int next = (cookie->head + 1) % MAX_CHARS;
	return next == cookie->tail;
}

void read_listener (void* addr) {
	struct cookie* cookie = addr;
	uint8_t code;
	while (!cookie->processing &&
		(code = uart_receive(cookie->uartno))) {
		cookie->line[cookie->head++] = (char)code;
		cookie->processing = (code == '\n');
		write_amap(cookie);
	}

	bool_t dropped = false;
	while (cookie->processing &&
		(code = uart_receive(cookie->uartno))) {
		dropped = true;
	}
	if (dropped) {
		// mmh
	}
}

void write_listener (void* addr) {
	struct cookie* cookie = addr;
	write_amap(cookie);
}

void write_amap(struct cookie* cookie) {
	while (cookie->tail < cookie->head) {
		uint8_t code = cookie->line[cookie->tail];
		uart_send(cookie->uartno, code);
		// if (!uart_send(cookie->uartno, code)) { return; }
		cookie->tail++;
		if (code == '\n') {
			//uart_send_string(cookie->uartno, cookie->line);
			cookie->tail = cookie->head = 0;
			cookie->processing = false;
		}
	}
}

