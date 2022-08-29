#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include "led.h"

int main() {
	bi_decl(bi_program_description("template raspberry pi pico/w c++ project"));

	stdio_init_all();

	init_led();
	uint32_t count = 1;

	while (true) {
		set_led(count & 1);
		printf("%d\n", count);
		sleep_ms(100);
		count++;
	}
}
