#ifndef _TTA_RS485_PRIVATE_H_
#define _TTA_RS485_PRIVATE_H_

#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif

#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#endif

#define _TTA_RS485_PREHEADER_LENGTH	1
#define _TTA_RS485_HEADER_LENGTH		5
#define _TTA_RS485_CHECKSUM_LENGTH	2

/* Time waited beetween the RTS switch before transmit data or after transmit
   data before to read */
#define _TTA_RS485_TIME_BETWEEN_RTS_SWITCH 10000

#if defined(_WIN32)
#if !defined(ENOTSUP)
#define ENOTSUP WSAEOPNOTSUPP
#endif

/* WIN32: struct containing serial handle and a receive buffer */
#define PY_BUF_SIZE 512
struct win32_ser {
	/* File handle */
	HANDLE fd;
	/* Receive buffer */
	uint8_t buf[PY_BUF_SIZE];
	/* Received chars */
	DWORD n_bytes;
};
#endif /* _WIN32 */

typedef struct _tta_rs485 {
    /* Device: "/dev/ttyS0", "/dev/ttyUSB0" or "/dev/tty.USA19*" on Mac OS X for
       KeySpan USB<->Serial adapters this string had to be made bigger on OS X
       as the directory+file name was bigger than 19 bytes. Making it 67 bytes
       for now, but OS X does support 256 byte file names. May become a problem
       in the future. */
#if defined(__APPLE_CC__)
    char device[64];
#else
    char device[16];
#endif
    /* Bauds: 9600, 19200, 57600, 115200, etc */
    int baud;
    /* Data bit */
    uint8_t data_bit;
    /* Stop bit */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;
#if defined(_WIN32)
    struct win32_ser w_ser;
    DCB old_dcb;
#else
    /* Save old termios settings */
    struct termios old_tios;
#endif
#if HAVE_DECL_TIOCSRS485
    int serial_mode;
#endif
#if HAVE_DECL_TIOCM_RTS
    int rts;
#endif
    /* To handle many slaves on the same link */
    int confirmation_to_ignore;
} tta_rs485_t;

#endif /* _TTA_RS485_PRIVATE_H_ */
