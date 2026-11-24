#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

/* The device tree node with the label dummy0 */
#define DUMMY_NODE DT_NODELABEL(dummy0)

/* Configuration of the device: values taken from the device tree */
struct dummy_config {
	int foo;
};

/* The kernel calls this function once at startup to initialize the device */
static int dummy_init(const struct device *dev)
{
	const struct dummy_config *cfg = dev->config;

	printk("dummy: init, foo = %d\n", cfg->foo);
	return 0;
}

static const struct dummy_config dummy_config0 = {
	/* The value is taken from the device tree when the program is built */
	.foo = DT_PROP(DUMMY_NODE, foo),
};

/* Creates the struct device for the node dummy0 and registers dummy_init */
DEVICE_DT_DEFINE(DUMMY_NODE, dummy_init, NULL, NULL, &dummy_config0,
		 POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, NULL);
