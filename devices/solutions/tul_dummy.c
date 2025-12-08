#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define DUMMY_NODE DT_NODELABEL(dummy0)

//

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DUMMY_NODE, tul_dummy), "dummy0 not tul,dummy");
BUILD_ASSERT(DT_NODE_HAS_STATUS(DUMMY_NODE, okay), "dummy0 not okay");

struct dummy_cfg { int foo; };

static int dummy_init(const struct device *dev)
{
    /* init work if any */
    return 0;
}

static const struct dummy_cfg dummy_cfg0 = {
    .foo = DT_PROP(DUMMY_NODE, foo),
};

DEVICE_DT_DEFINE(DUMMY_NODE, dummy_init, NULL, NULL, &dummy_cfg0,
                 POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, NULL);

/* optional: keep a handle around */
const struct device *const DUMMY0 = DEVICE_DT_GET(DUMMY_NODE);
