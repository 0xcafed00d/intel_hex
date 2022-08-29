#include <pico/stdlib.h>

#if defined(RASPBERRYPI_PICO_W)
	#include <pico/cyw43_arch.h>
#endif

#if defined(PICO_DEFAULT_LED_PIN)
void init_led() {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void set_led(bool state) {
	gpio_put(PICO_DEFAULT_LED_PIN, state);
}
#elif defined(RASPBERRYPI_PICO_W)
void init_led() {
	cyw43_arch_init();
}

void set_led(bool state) {
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
}
#else
void init_led() {
}

void set_led(bool state) {
}
#endif
