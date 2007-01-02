#ifndef __WAITQUEUE_HH__
#define __WAITQUEUE_HH__

#include "tasking.hh"

class WaitQueue {
private:
	Process *head;
	Process *end;
public:
	WaitQueue() {
		head = end = NULL;
	}
	
	void addLast(Process *p) {
		p->setPrevious(end);
		if (end != NULL) {
			end->setNext(p);
		}
		else {
			head = p;
		}
		p->setNext(NULL);
		end = p;
	}

	void addFirst(Process *p) {
		p->setNext(head);
		if (head != NULL) {
			head->setPrevious(p);
		}
		else {
			end = p;
		}
		p->setPrevious(NULL);
		head = p;
	}

	Process *extractFirst() {
		Process *rv = head;
		head = rv->getNext();

		if (head == NULL) {
			end = NULL;
		}
		else {
			head->setPrevious(NULL);
		}

		rv->setNext(NULL);
		rv->setPrevious(NULL);
		return rv;
	}

	bool remove(Process *p) {
		if (p == head) {
			extractFirst();
			return true;
		}
		else if (p == end) {
			end = p->getPrevious();
		}
		else {
			p->getNext()->setPrevious(p->getPrevious());
		}
		// valid for both...
		p->getPrevious()->setNext(p->getNext());

		p->setNext(NULL);
		p->setPrevious(NULL);
		return true;
	}

	bool isEmpty() const {
		return head == NULL;
	}
};

#endif

