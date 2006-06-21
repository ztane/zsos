#include "mbox.h"

#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>

const unsigned int INFINITE = 0xFFFFFFFFul;

int mbox_create()
{
	mqd_t box;
	static int mbox_no = 0;
	char namebuf[256];

	while (1) {
		sprintf(namebuf, "/zsos_sim_%d", mbox_no);
	
		errno = 0;
		box = mq_open(namebuf, O_CREAT|O_RDWR|O_EXCL);
		
		if (box != -1) {
			return box;
		}

		if (errno != EEXIST) {
			return -1;
		}
		
		mbox_no ++;
	};	
}
	
int mbox_delete(int handle) {
	return mq_close(handle);
}

int mbox_send(int box, const char *message, size_t message_len,
	unsigned int priority)
{	
	return mq_send(box, message, message_len, priority);
}

int mbox_receive(int box, char *message, size_t messagelen, 
	unsigned int milliseconds)
{
	unsigned int dummy;
	if (milliseconds == INFINITE) {
		return mq_receive(box, message, messagelen,
			&dummy);
	}
	else {
		struct timespec wait;
		struct timeval current;
		unsigned long long total_millis;

		gettimeofday(&current, NULL);
		total_millis = milliseconds + current.tv_usec / 1000;
		wait.tv_sec  = current.tv_sec + total_millis / 1000;
		wait.tv_nsec = (total_millis % 1000) * 1000000;

		return mq_timedreceive(box, message, 
			messagelen, &dummy, &wait);
	}
}
