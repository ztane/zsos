#include <iostream>
#include <cstdlib>
#include <cstring>

#include "messagequeue.hh"

Mutex mQueuePoolLock;
MessageQueue *messageQueuePool = NULL;

MessageQueue::MessageQueue(const char *mqname) :
	flags(0),
	maxMsgCount(0),
	maxMsgSize(0),
	currMsgCount(0),
	next(NULL)
{
	size_t nLen = strlen(mqname) + 1;
	name = new char[nLen];
	strncpy(name, mqname, nLen);

	// initialize the ringbuffer
}

MessageQueue::~MessageQueue()
{
	// destroy the ringbuffer
	delete[] name;
}

void MessageQueue::setNext(MessageQueue *nmq)
{
	next = nmq;
}

void MessageQueue::setPrev(MessageQueue *pmq)
{
	next = pmq;
}

MessageQueue *MessageQueue::getNext() const
{
	return next;
}

MessageQueue *MessageQueue::getPrev() const
{
	return prev;
}

void MessageQueue::acquire()
{
	++ rCount;
}

void MessageQueue::release()
{
	if (-- rCount && flags)
		; // do suicide
}

MessageQueueDesc::MessageQueueDesc(const char *name, int attr) :
	flags(attr)
{
	mq = messageQueuePool;

	mQueuePoolLock.lock();
	if (mq == NULL) {
		messageQueuePool = new MessageQueue(name); // FIXME! check for NULL
		mq = messageQueuePool;
		mq->setNext(NULL);
		mq->setPrev(NULL);
	} else {
		int rv = 0;
		size_t nlen = strlen(name);

		while ((rv = strncmp(name, mq->getName(), nlen)) && mq->getNext() != NULL)
			mq = mq->getNext();
		if (rv && mq->getNext() == NULL) {
			// no match
			MessageQueue *tmq = new MessageQueue(name);
			tmq->setNext(NULL);
			tmq->setPrev(mq);
			mq->setNext(tmq);
			mq = tmq;
		}
		// correct match (or new) and mq set
	}
	mQueuePoolLock.unlock();

	mq->acquire();
}

MessageQueueDesc::~MessageQueueDesc()
{
	mq->release();
}
