#include <iostream>
#include "kernel/arch/current/ide.hh"
#include "kernel/ktasks/softirq.hh"
#include <kernel/interrupt.hh>

namespace ide {

IdeController::IdeController() : Init(Init::EARLY)
{

}

void if0_softirq_handler(int vector);
void if1_softirq_handler(int vector);

int IdeController::init()
{
	ifs[0] = new IdeInterface(0x1F0, 14);
	ifs[1] = new IdeInterface(0x170, 15);

	int i;
	for (i = 0; !registerSoftIrq(i, if0_softirq_handler) && i < numSoftIrqVectors; i ++)
		;
	softirq_vectors[0] = i;
	// There is probably no need to scan from 0 again, but fix later
	for (i = 0; !registerSoftIrq(i, if1_softirq_handler) && i < numSoftIrqVectors; i ++)
		;
	softirq_vectors[1] = i;

	return 0;
}




/* IdeInterface - an IDE controller has 2 interfaces
 * Has own IRQ and port base
 */
IdeInterface::IdeInterface(int base, int irq)
{
	this->base = base;
        this->irq  = irq;
	floating_bus = inb(base + 7) == 0xff;

	// SKIP CONSTRUCTION, THIS BUS HAS NO DRIVES
	if (floating_bus) {
		return;
        }

	requests = new RingBuffer<struct ide_request_t>(MAX_IDE_REQUESTS);
	for (int i = 0; i < MAX_DRIVES; i ++) {
		drives[i] = new IdeDisk(base, i);
	}

 	int drives_exist = 0;

	// Issues SOFTWARE RESET, clears control register and selects drive0
	drives[0]->__ata_soft_reset();

	for (int i = 0; i < MAX_DRIVES; i ++) {
		__select_drive(i);

		drives[i]->__identify_drive();
		if (drives[i]->get_exists()) {
			drives_exist = 1;
			drives[i]->__set_geometry();
                }
	}

	if (drives_exist) {
		enableIrq(irq);
	}
}

void IdeInterface::__select_drive(int num)
{
	drives[num]->__select();
	selected_drive = num;
}

/* QUICK AND DIRTY */
void IdeInterface::add_request(int _rw, int _drive, void *_data, uint64_t _block, size_t _count)
{
	struct ide_request_t request;

	request.drive = _drive;
	request.block = _block;
	request.count = _count;
	request.data  = _data;

	if (_rw == 0) {
		request.type = ide_request_t::READ;
        }
	else {
		request.type = ide_request_t::WRITE;
        }

//	wq.addCurrentTask();
	requests->put(request);

	if (selected_drive != request.drive) {
		__select_drive(request.drive);
	}

	drives[request.drive]->command(request);
}

void IdeInterface::softirq_handler()
{
	struct ide_request_t request;
	requests->get(request);

	if (request.type == ide_request_t::READ) {
		drives[request.drive]->read(request.data, request.count);
	} else {
		drives[request.drive]->write(request.data, request.count);
	}
}

IdeController controller; // driver instance

void irq_handler(int ifnum)
{
	kout << "Juuhuu! ide" << ifnum << " got an IRQ!" << endl;
	triggerSoftIrq(controller.softirq_vectors[ifnum]);
	return;
}

void if0_softirq_handler(int vector)
{
	controller.ifs[0]->softirq_handler();
}

void if1_softirq_handler(int vector)
{
	controller.ifs[1]->softirq_handler();
}

};
