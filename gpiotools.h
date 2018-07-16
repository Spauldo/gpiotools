/* gpiotools.h
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
 *
 * Utilities for manipulating GPIO via the /dev/gpiochip* devices.
 */

#ifndef _GPIOTOOLS_H
#define _GPIOTOOLS_H

#include <linux/gpio.h>
#include <sys/ioctl.h>

/* Retrieves the chip information for a GPIO chip.
 * Takes an open file descriptor to a gpio device and a pointer to gpiochip_info
 * struct, which it fills in.
 * Returns true if successful, false otherwise. */
__attribute__((always_inline)) int inline
get_chip_info(int gpio_fd, struct gpiochip_info *chip_info)
{
	if (ioctl(gpio_fd, GPIO_GET_CHIPINFO_IOCTL, chip_info) == -1)
		return 0;
	else
		return 1;
}

/* Retrieves the line info for a single GPIO line.
 * Takes an open file descriptor to a gpio device, the line number we are
 * interested in, and a pointer to a gpioline_info struct which it fills in.
 * Returns true if successful, false otherwise. */
__attribute__((always_inline)) int inline
get_line_info(int gpio_fd, unsigned int line, struct gpioline_info *line_info)
{
	line_info->line_offset = line;
	if (ioctl(gpio_fd, GPIO_GET_LINEINFO_IOCTL, line_info) == -1)
		return 0;
	else
		return 1;
}

/* Retrieves a file descriptor for a set of GPIO lines.
 * Takes an open file descriptor to a gpio device and a pointer to a populated
 * gpiohandle_request struct.
 * Returns true if successful, false otherwise. */
__attribute__((always_inline)) int inline
get_line_handle(int gpio_fd, struct gpiohandle_request *req)
{
	/* I thought about having this function set up the req struct, but
	 * it's actually easier and faster to just do it on the calling
	 * side.  So we just do the ioctl() here. */
	if (ioctl(gpio_fd, GPIO_GET_LINEHANDLE_IOCTL, req) == -1)
		return 0;
	else if (req->fd <= 0)
		/* We got a bad file descriptor. */
		return 0;
	else
		return 1;
}

/* Reads the data for a set of lines.
 * Takes an open line descriptor and a pointer to a gpiohandle_data struct
 * which it fills in with the retrieved data.
 * Returns true if successful, false otherwise. */
__attribute__((always_inline)) int inline
get_line_data(int line_fd, struct gpiohandle_data *data)
{
	if (ioctl(line_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, data) == -1)
		return 0;
	else
		return 1;
}

/* Writes the data to a set of lines.
 * Takes an open line descriptor and a pointer to a gpiohandle_data struct
 * which must contain the data to write.
 * Returns true if successful, false otherwise. */
__attribute__((always_inline)) int inline
set_line_data(int line_fd, struct gpiohandle_data *data)
{
	if (ioctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, data) == -1)
		return 0;
	else
		return 1;
}

/* Gets the file descriptor to a stream of event data for a line.
 * Takes an open gpio descriptor and a pointer to a prefilled gpioeven_request
 * struct, where it will place the file descriptor.
 * Returns true on success and false otherwise. */
__attribute__((always_inline)) int inline
get_gpioevent_handle(int gpio_fd, struct gpioevent_request *req)
{
	if (ioctl(gpio_fd, GPIO_GET_LINEEVENT_IOCTL, req) == -1)
		return 0;
	else
		return 1;
}


#endif /* _GPIOTOOLS_H */
