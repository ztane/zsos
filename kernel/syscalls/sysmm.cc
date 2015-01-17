#include <syscall.hh>
#include <iostream>
#include <scheduler.hh>
#include "kernel/memory.hh"

extern Scheduler scheduler;
SYSCALL(brk)
{
        Task *task = scheduler.getCurrentTask();
	void *rv = task->setBrk((void*)r.arg0);
        SYSCALL_RETURN((uint32_t)rv);
}

SYSCALL(set_thread_area)
{
        Task *task = scheduler.getCurrentTask();
	UserDesc *desc = (UserDesc*)r.arg0;
	int rc = 0;

	if (desc->getEntryNumber() == -1) {
		int i = 0;
		for (i = 0; i < N_TLS_ENTRY; i++) {
			if (task->tlsUserDescriptors[i].getEntryNumber() == -1) {
				break;
			}
		}
		if (i == N_TLS_ENTRY) {
			rc = -ESRCH;
			goto exit;
		}

		desc->setEntryNumber(TLS_ENTRY_MIN + i);
	}

	if (desc->getEntryNumber() < TLS_ENTRY_MIN || desc->getEntryNumber() > TLS_ENTRY_MAX) {
		rc = -EINVAL;
		goto exit;
	}

	task->tlsUserDescriptors[desc->getEntryNumber() - TLS_ENTRY_MIN] = *desc;
	task->tlsDescriptors[desc->getEntryNumber() - TLS_ENTRY_MIN] = desc->toGDT();
	task->swapGDT();

exit:
        SYSCALL_RETURN((uint32_t)rc);
}
