#ifndef __WAITQUEUE_HH__
#define __WAITQUEUE_HH__

#include "task.hh"
#include "scheduler.hh"

class WaitQueue {
private:
	Task *head;
	Task *end;
public:
	WaitQueue() {
		head = end = NULL;
	}

protected:	
	void addLast(Task *p) volatile {
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

	void addFirst(Task *p) volatile {
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

	Task *extractFirst() volatile {
		Task *rv = head;
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

	bool remove(Task *p) volatile {
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

public:
	bool isEmpty() const volatile {
		return head == NULL;
	}

	void addCurrentTask() volatile {
		Task *task = scheduler.getCurrentTask();
		scheduler.removeTask(task);
		task->setCurrentState(Task::BLOCKED);
		addLast(task);
	}
	
	void resumeFirst() volatile {
               if (! isEmpty()) {
                        Task *task = extractFirst();
                        task->setCurrentState(Task::READY);
                        scheduler.addTask(task);
               }
	}
	
	void resumeAll() volatile {
		while (! isEmpty()) {
			resumeFirst();
		}
	}
};

#endif

