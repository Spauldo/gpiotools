/* gpiolist.c
 *
 * A small utility for retrieving GPIO information on Linux.
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


#define _POSIX_C_SOURCE 200809L

#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "gpiotools.h"

/* Prints usage information to stderr */
void print_usage();

/* Prints the GPIO chip information.
 * Takes an open file descriptor to a GPIO device.
 * Returns true on success, false if ioctl() failed. */
int print_chip_info(int gpio_fd);

/* Prints the GPIO line information.
 * Takes an open file descriptor to a GPIO device and a line number.  If a
 * negative line number is given, prints information for all lines.
 * Returns true on success, false if ioctl() failed. */
int print_line_info(int gpio_fd, int line_num);

/* Prints the info for a single GPIO line.
 * Takes a struct gpioline_info pointer that has been filled by an ioctl() call.
 */
void print_single_line_info(struct gpioline_info *line_info);


static const char gpio_dev_prefix[] = "/dev/gpiochip"; /* GPIO character device
							* names */
char *progname;                                        /* Name of this
							* program */

int
main(int argc, char **argv)
{
	char gpio_dev_name[sizeof(gpio_dev_prefix) + 2]; /* character device
							  * filename */
	char gpio_num[3] = {'\0', '\0', '\0'};           /* character device
							    number */

	int gpio_fd;             /* file descriptor */
	int line_num;            /* line number (-1 for all lines) */
	int ci_flag, li_flag;    /* Are we requesting chip or line info?*/

	int c;                   /* getopt() character */

	progname = argv[0];
	ci_flag = li_flag = 0;

	while ((c = getopt(argc, argv, "hcl:d:")) != -1) {
		switch(c) {
		case 'd':
			snprintf(gpio_num, sizeof(gpio_num),
				 "%d", (int) strtol(optarg, NULL, 10));
			break;
		case 'c':
			ci_flag = 1;
			break;
		case 'l':
			li_flag = 1;
			if (strcmp(optarg, "all") == 0) {
				line_num = -1;
			} else {
				line_num = (int) strtol(optarg, NULL, 10);
			}
			break;
		case '?':
		default:
			print_usage();
			return EXIT_FAILURE;
		}
	}

	if (! (ci_flag || li_flag)) {
		fprintf(stderr, "You must specify at least one of -c or -l.\n");
		print_usage();
		return EXIT_FAILURE;
	}

	/* Generate filename for character device */
	strcpy(gpio_dev_name, gpio_dev_prefix);
	strcat(gpio_dev_name, gpio_num);

	if ((gpio_fd = open(gpio_dev_name, O_RDWR | O_CLOEXEC)) < 0) {
		perror("Cannot open gpio device");
		fprintf(stderr, "Did you specify a number (i.e. '3') for the ");
		fprintf(stderr, "device?");
		print_usage();
		return EXIT_FAILURE;
	}

	if (ci_flag)
		print_chip_info(gpio_fd);

	if (li_flag)
		print_line_info(gpio_fd, line_num);

	close(gpio_fd);

	return EXIT_SUCCESS;
}


int
print_chip_info(int gpio_fd)
{
	struct gpiochip_info chip_info;

	if (! get_chip_info(gpio_fd, &chip_info)) {
		perror("Failed to retrieve GPIO chip info");
		return 0;
	}

	printf("GPIO %s\n", chip_info.name);
	if (chip_info.label[0])
		printf("  Label: %s\n", chip_info.label);
	printf("  Lines: %d\n", chip_info.lines);

	return 1;
}

void
print_single_line_info(struct gpioline_info *line_info)
{
	int flag = 0; /* Have we printed anything about this line? */

	printf("Line %2d\n", line_info->line_offset);

	if (line_info->name[0]) {
		flag = 1;
		printf("  Name: %s\n", line_info->name);
	}

	if (line_info->consumer[0]) {
		flag = 1;
		printf("  Used by: %s\n", line_info->consumer);
	}

	if (line_info->flags) {
		flag = 1;
		printf("  Flags: ");
		if (line_info->flags & GPIOHANDLE_REQUEST_INPUT)
			printf("INPUT ");
		if (line_info->flags & GPIOHANDLE_REQUEST_OUTPUT)
			printf("OUTPUT ");
		if (line_info->flags & GPIOHANDLE_REQUEST_ACTIVE_LOW)
			printf("ACTIVE_LOW ");
		if (line_info->flags & GPIOHANDLE_REQUEST_OPEN_DRAIN)
			printf("OPEN_DRAIN ");
		if (line_info->flags & GPIOHANDLE_REQUEST_OPEN_SOURCE)
			printf("OPEN_SOURCE ");
		printf("\n");
	}

	if (! flag)
		printf("  Not configured.\n");

}

int
print_line_info(int gpio_fd, int line_num)
{
	struct gpiochip_info chip_info;
	struct gpioline_info line_info;

	if (! get_chip_info(gpio_fd, &chip_info)) {
		perror("Failed to retrieve GPIO chip info");
		return 0;
	}

	if (line_num >= (int) chip_info.lines) {
		fprintf(stderr, "Invalid line number: %d\n", line_num);
		fprintf(stderr, "This chip has %d lines.\n", chip_info.lines);
		print_usage();
		return 0;
	}

	if (line_num >= 0) {
		/* Print info for single line */
		if (! get_line_info(gpio_fd, line_num, &line_info)) {
			perror("Failed to retrieve GPIO line info");
			return 0;
		}

		print_single_line_info(&line_info);
	} else {
		/* Print info for all lines */
		for (int i = 0; i < chip_info.lines; i++) {
			if (! get_line_info(gpio_fd, i, &line_info)) {
				fprintf(stderr, "On GPIO line %d:\n", i);
				perror("Failed to retieve GPIO line info");
			} else {
				print_single_line_info(&line_info);
			}
		}
	}

	return 1;
}

void
print_usage()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "%s [-c] [-l<all|line#>] -d<gpio device number>\n",
		progname);
}
