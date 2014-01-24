#include <stdio.h>             
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "log.h"
#include "gpio.h"
#include "ha210.h"

#define GPIO_DEV	"/dev/ha210"
#define VDEC_DEV	"/dev/video1"

int gpio_cfg(int gpio, int func)
{
	ioctl_msg_t msg;
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	msg.gpio.cmd = TCC_GPIO_CFG;
	msg.gpio.idx = gpio;
	msg.gpio.val = func;
	if (ioctl(fd, HA200_IOCTL_GPIO, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int gpio_set_output(int gpio, int val)
{
	ioctl_msg_t msg;
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.gpio.cmd = TCC_GPIO_SET_DIR_OUT;
	msg.gpio.idx = gpio;
	msg.gpio.val = val;
	if (ioctl(fd, HA200_IOCTL_GPIO, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int gpio_set_input(int gpio)
{
	ioctl_msg_t msg;
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.gpio.cmd = TCC_GPIO_SET_DIR_IN;
	msg.gpio.idx = gpio;
	if (ioctl(fd, HA200_IOCTL_GPIO, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int gpio_read(int gpio, int *val)
{
	ioctl_msg_t msg;
	int fd, req;

	*val = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.gpio.cmd = TCC_GPIO_READ;
	msg.gpio.idx = gpio;
	if (ioctl(fd, HA200_IOCTL_GPIO, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	*val = msg.gpio.val;

	return 0;
}

int gpio_write(int gpio, int val)
{
	ioctl_msg_t msg;
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.gpio.cmd = TCC_GPIO_WRITE;
	msg.gpio.idx = gpio;
	msg.gpio.val = val;
	if (ioctl(fd, HA200_IOCTL_GPIO, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

/*
 * System
 */

int hwwatchdog_get(int cmd, int *val)
{
	ioctl_msg_t msg;
	int fd, req;

	*val = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.hwwatchdog.cmd = cmd;
	if (ioctl(fd, HA200_IOCTL_HWWATCHDOG, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	*val = msg.hwwatchdog.val;

	return 0;
}

int hwwatchdog_set(int cmd, int val)
{
	ioctl_msg_t msg;
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}

	msg.hwwatchdog.cmd = cmd;
	msg.hwwatchdog.val = val;
	if (ioctl(fd, HA200_IOCTL_HWWATCHDOG, &msg) < 0)
	{
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int system_enable_hwwatchdog(int on)
{
	return hwwatchdog_set(HWWATCHDOG_SET_EN, on);
}

int system_refresh_hwwatchdog()
{
	hwwatchdog_set(HWWATCHDOG_SET_REFRESH, 1);
	hwwatchdog_set(HWWATCHDOG_SET_REFRESH, 0);
	return 0;
}
