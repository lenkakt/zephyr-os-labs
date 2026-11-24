#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>

/* The device tree node with the label uart0 */
#define UART_NODE DT_NODELABEL(uart0)

/* The device the build created for the node uart0 */
static const struct device *const uart = DEVICE_DT_GET(UART_NODE);

int main(void)
{
	/* The address comes from the property reg of the node */
	printk("uart0 registers start at 0x%08lx\n", (unsigned long)DT_REG_ADDR(UART_NODE));

	if (!device_is_ready(uart)) {
		printk("uart0 is not ready\n");
		return 0;
	}
	printk("uart0 device name: %s\n", uart->name);

	for (const char *c = "Hello from the UART driver!\n"; *c != '\0'; c++) {
		/* The driver writes to the registers, the application does not */
		uart_poll_out(uart, *c);
	}
	return 0;
}
