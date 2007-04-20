#include <iostream>
#include <cstring>

#include "kernel/task.hh"
#include "kernel/usertask.hh"
#include "kernel/memory.hh"
#include "kernel/printk.h"
#include "kernel/exe/bits.hh"
#include "kernel/paging.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/mm/memarea.hh"
#include "kernel/panic.hh"

Task::Task(const char *_name, State state, int priority) {
	esp = 0;
	strncpy(name, _name, sizeof(name));
	isNew = true;
	setCurrentState(READY);
	setCurrentPriority(priority);
	timeslice = 0;
}

bool Task::handlePageFault(PageFaultInfo& f) {
        kout << "Process " << process_id << " had a pfault at " 
		<< (uint32_t)f.address << endl;

	// map code...
	if (f.address == 1048740) {
		char *text_address = (char*)&_binary_example_zsx_start 
			+ _binary_example_zsx_start.textPhys;
		
		pageaddr_t p = pageaddr_t::fromVirtual(text_address);
	
		PageFlags fl;
		fl.setPresent(true);
		fl.setRW(false);
		fl.setUser(true);

		mapPage(page_directory, f.address, 
			&(page_frames.getByFrame(p)), fl);
	}
	// map stack...
	else if ((f.address & 0xF0000000) == 0xB0000000) {
                PageFlags fl;
                fl.setPresent(true);
                fl.setRW(true);
                fl.setUser(true);

                PageAllocation ac;
                NormalMemory.allocatePages(1, ac);
                if (! ac) {
                        kernelPanic("Could not allocate a frame for stack\n");
	        }

                mapPage(page_directory, f.address,
                        &(page_frames.getByFrame(ac.getAddress())), fl);
	}
	// map data (* should use COW *)...
	else if ((f.address & 0xF0000000) == 0x80000000) {
		// bss.. ;)
		pageaddr_t p;

		// bss...
		if (f.address & 0xF000) {
	                PageAllocation ac;
        	        NormalMemory.allocatePages(1, ac);
                	if (! ac) {
                        	kernelPanic("Could not allocate data for bss\n");
	        	}

			void *addr = ac.getAddress().toVirtual();
			memset(addr, 0, 4096);
			kout << "bss..." << endl;
		}
		else {
			char *data_address = (char*)&_binary_example_zsx_start 
				+ _binary_example_zsx_start.dataPhys;
		
			p = pageaddr_t::fromVirtual(data_address);
		}

		PageFlags fl;
		fl.setPresent(true);
		fl.setRW(true);
		fl.setUser(true);

		mapPage(page_directory, f.address, 
			&(page_frames.getByFrame(p)), fl);
	}
	else {
        	__asm__ __volatile__ ("cli; hlt");
	}
        return true;
}

void Task::switchContexts(uint32_t *saved_esp) {
        __asm__ __volatile__ (
                "call 1f\n\t"
                "jmp 2f\n"
                "1:\n\t"
                "pushal\n\t"
                "mov %%esp, (%1)\n\t"
                "mov %0, %%esp\n\t"
                "popal\n\t"
                "ret\n"
                "2:\n\t"
                : : "a"(esp), "b"(saved_esp));
}
