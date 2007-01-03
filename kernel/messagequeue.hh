// -*- C++ -*-

#ifndef MESSAGEQUE_INC
#define MESSAGEQUE_INC 1

#include <cstdlib>

#include "refcount.hh"
#include "mutex.hh"

const int MAX_MSGQUEUE_COUNT = 64;

class MessageQueue {
private:
	int flags;
	size_t maxMsgCount;
	size_t maxMsgSize;
	size_t currMsgCount;
	char *name;
	RefCount rCount;
	Mutex mqLock;
//	RingBuffer msgData;
	MessageQueue *next;
	MessageQueue *prev;
public:
	MessageQueue(const char *mqname);
	~MessageQueue();
	int push(void *src, size_t len); // FIFO
	int pop(void *dst, size_t len); // FIFO
	void setNext(MessageQueue *nmq);
	void setPrev(MessageQueue *pmq);
	void acquire();
	void release();

	MessageQueue *getNext() const;
	MessageQueue *getPrev() const;
	const char *getName() const;
};

class MessageQueueDesc {
private:
	int flags;
	MessageQueue *mq;
public:
	MessageQueueDesc(const char *name, int attr);
	~MessageQueueDesc();
	int send(const void *src, size_t len); // blocking
	int recv(void *dst, size_t len); // blocking
	int destroy(); // mark Queue for desctruction (when mqd.rCount reaches 0)
};

#endif
