/* gpiotools.c
 *
 * Copyright (C) 2018 Jeff Spaulding <sarnet@gmail.com>
 *
 * This is the ISC License.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "gpiotools.h"

/*
int
get_chip_info(int gpio_fd, struct gpiochip_info *chip_info)
{
	if (ioctl(gpio_fd, GPIO_GET_CHIPINFO_IOCTL, chip_info) == -1)
		return 0;
	else
		return 1;
}

int
get_line_info(int gpio_fd, unsigned int line, struct gpioline_info *line_info)
{
	line_info->line_offset = line;
	if (ioctl(gpio_fd, GPIO_GET_LINEINFO_IOCTL, line_info) == -1)
		return 0;
	else
		return 1;
}
*/
