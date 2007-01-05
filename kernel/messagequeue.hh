// -*- C++ -*-

#ifndef MESSAGEQUE_INC
#define MESSAGEQUE_INC 1

#include <cstdlib>

#include "refcount.hh"
#include "mutex.hh"
#include "ringbuffer.hh"

const int MAX_MSGQUEUE_COUNT = 64;

template <class T>
class MessageQueue {
private:
	int flags;
	size_t maxmsgs;
	size_t curmsgs;
	char *name;
	RefCount rcount;
	Mutex lock;
	RingBuffer<T> msgs;
public:
	MessageQueue(const char *_name, size_t _size, int _attr);
	~MessageQueue();
	int add(const T &src); // FIFO
	int get(T &dst); // FIFO
	void acquire();
	void release();
	void destroy();

	const char *getName() const;
};

/* ADD IF NEEDED (for example for privilege separation) */
/*
template <class T>
class MessageQueueDesc {
private:
	int flags;
	MessageQueue<T> &mq;
public:
	MessageQueueDesc(const char *name, int attr);
	~MessageQueueDesc();
	int send(const T &src); // blocking
	int recv(T &dst); // blocking
	int destroy(); // mark Queue for desctruction (when mqd.rCount reaches 0)
};
*/

#endif
