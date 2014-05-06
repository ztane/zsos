#include <kernel/arch/current/pci.hh>
#include <kernel/arch/current/port.h>
#include <inttypes.h>
#include <printk.h>
#include <panic.hh>
#include <new>

const int PCI_CONFIG_ADDRESS = 0xCF8;
const int PCI_CONFIG_DATA    = 0xCFC;

namespace PCI {
	static PCIDevice devices[64];
	static const int nDevs = (sizeof (devices)) / (sizeof (PCIDevice));
	static int nextFree = 0;
	
	static void detectDevice(uint32_t bus, uint32_t slot, uint32_t fun)
	{
		if (nextFree == nDevs) {
			kernelPanic("Out of PCI device slots!");
		}

		PCIDevice *place = devices + nextFree;
		nextFree ++;

		PCIDevice& dev = *(new (place) PCIDevice(bus, slot, fun));
		PCIConfig& c = dev.config;		
		
		uint16_t clazz = c.classID();
		printk("%d:%d.%d - class %04x, vendor %04x - device %04x",
       			c.bus(), c.slot(),   c.func(), 
			clazz,   c.vendor(), c.deviceID());

		if (clazz == 0x0101) {
			printk("\n  * an IDE controller; bus mastering ");
			uint8_t reserved = dev.config[0x08] >> 8;
			printk("is %ssupported", reserved & 0x80 ? "" : "not ");
		}

		printk("\n");
	}

	void pciListDevices() {
		for (int bus = 0; bus < 256; bus ++) {
			for (int slot = 0; slot < 32; slot++) {
				for (int fun = 0; fun < 16; fun++) {
					PCIConfig cfg(bus, slot, fun);
					if (cfg[0] != 0xFFFFFFFF) {
						detectDevice(bus, slot, fun);
					}
					else {
						break;
					}
		 		}
			}
		}
	}

	

	void initialize() {
		pciListDevices();
	}
};
