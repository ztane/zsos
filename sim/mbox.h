#ifndef __MBOX_H__
#define __MBOX_H__

#include <stdlib.h>

int mbox_create();
int mbox_delete(int handle);
int mbox_send(int box, const char *message, size_t message_len,
	unsigned int priority);
int mbox_receive(int box, char *message, size_t messagelen, 
	unsigned int milliseconds);

#endif
