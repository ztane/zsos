#include <iostream>
#include <cstdlib>
#include <cstring>

#include "messagequeue.hh"


template <class T>
MessageQueue<T>::MessageQueue(const char *_name, size_t _size, int _attr) :
	flags(_attr),
	maxmsgs(_size),
	curmsgs(0)
{
	size_t nlen = strlen(_name) + 1;
	name = new char[nlen];
	strncpy(name, _name, nlen);

	// initialize the ringbuffer
}

template <class T>
MessageQueue<T>::~MessageQueue()
{
	// destroy the ringbuffer
	delete[] name;
}

template <class T>
void MessageQueue<T>::acquire()
{
	++ rcount;
}

template <class T>
void MessageQueue<T>::release()
{
	if (-- rcount && (flags & 0x80000000))
		; // do suicide
}

template <class T>
void MessageQueue<T>::destroy()
{
	flags |= 0x80000000;
}

template <class T>
const char * MessageQueue<T>::getName() const
{
	return (const char *) name;
}
