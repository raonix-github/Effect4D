#ifndef _HA200_H_
#define _HA200_H_

#define HA200_DEV_MAJOR_NUM		248

/*
 * ioctl commands
 */
#define	HA200_IOCTL_GPIO			_IO(HA200_DEV_MAJOR_NUM, 100)
#define	HA200_IOCTL_WM8731_I2C_WR	_IO(HA200_DEV_MAJOR_NUM, 200)
#define	HA200_IOCTL_ENVSENSOR		_IO(HA200_DEV_MAJOR_NUM, 300)
#define	HA200_IOCTL_INTERPHONE		_IO(HA200_DEV_MAJOR_NUM, 400)
#define HA200_IOCTL_HWWATCHDOG		_IO(HA200_DEV_MAJOR_NUM, 500)

// GPIO
#define TCC_GPIO_CFG			0x01
#define TCC_GPIO_SET_DIR_IN		0x02
#define TCC_GPIO_SET_DIR_OUT	0x03
#define	TCC_GPIO_READ			0x04
#define	TCC_GPIO_WRITE			0x05
#define	TCC_GPIO_SET			0x06
#define	TCC_GPIO_CLEAR			0x07
#define TCC_GPIO_REG_EXINT		0x08

// H200 HW Watchdog
#define HWWATCHDOG_GET_EN       0x01
#define HWWATCHDOG_SET_EN       0x11
#define HWWATCHDOG_SET_REFRESH  0x12

typedef struct _ioctl_gpio_t
{
	uint32_t cmd;
	uint32_t idx;
	uint32_t val;
} ioctl_gpio_t;

typedef struct _ioctl_hwwatchdog_t
{
	uint32_t cmd;
	uint32_t val;
} ioctl_hwwatchdog_t;

typedef struct  _ioctl_msg_t
{
	int rc;

	union
	{
		ioctl_gpio_t		gpio;
		ioctl_hwwatchdog_t	hwwatchdog;
	};
} ioctl_msg_t;

#define		RET_SUCCESS		(0)
#define		RET_FAILED		(1)
#define		RET_MEM_ERR		(2)
#define		RET_UNKNOWN_CMD	(3)

#endif
