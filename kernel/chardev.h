#ifndef __CHARDEV_H__
#define __CHARDEV_H__

#include "stdlib.h"

typedef struct _char_device_interface {
	size_t (*read)(void *, size_t);
	size_t (*write)(const void *, size_t);
} char_device_iface;

#endif
