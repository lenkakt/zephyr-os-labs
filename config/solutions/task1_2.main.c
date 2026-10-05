/*
 * Config lab, TASK1.2: using Kconfig options in the application code.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

int main(void)
{
	printf("Hello World! System has booted and it is running. To stop the emulation, press Ctrl+A, X.\n");

	/* An int option is always defined in autoconf.h, even when it is 0. */
	printf("Boot delay: %d ms\n", CONFIG_BOOT_DELAY);

	/*
	 * A bool option set to n is NOT defined in autoconf.h at all, and neither are
	 * the options that depend on it (BOOT_BANNER_STRING depends on BOOT_BANNER).
	 * #if removes the code before the compiler sees it, so the missing
	 * CONFIG_BOOT_BANNER_STRING does not matter.
	 */
#if defined(CONFIG_BOOT_BANNER)
	printf("Boot banner is on, text: \"%s\"\n", CONFIG_BOOT_BANNER_STRING);
#else
	printf("Boot banner is off.\n");
#endif

	/*
	 * IS_ENABLED() is an ordinary C condition: the code is always compiled
	 * (so it must be valid), the compiler only removes the dead branch.
	 */
	if (IS_ENABLED(CONFIG_BOOT_BANNER)) {
		printf("IS_ENABLED: the banner code is in this build.\n");
	} else {
		printf("IS_ENABLED: the banner code is not in this build.\n");
	}

	return 0;
}
