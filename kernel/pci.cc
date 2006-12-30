#include <pci.hh>
#include <port.h>
#include <inttypes.h>
#include <printk.h>

const int PCI_CONFIG_ADDRESS = 0xCF8;
const int PCI_CONFIG_DATA    = 0xCFC;

namespace PCI {

inline uint32_t pci_config_read_long(int bus, int slot, int func, int offset)
{
     uint32_t address;
 
     /* create configuration address as per Figure 1 */
     address = (bus << 16) | (slot << 11) |
             (func << 8) | (offset & 0xfc) | 0x80000000;
 
     /* write out the address */
     outl(PCI_CONFIG_ADDRESS, address);
     /* read in the data */
     return inl(PCI_CONFIG_DATA);
}

inline uint16_t pci_config_read_short(int bus, int slot, int func, int offset) {
	uint32_t temp = pci_config_read_long(bus, slot, func, offset);
	return temp >> ((offset & 2) << 3) & 0xFFFF;
}

inline uint8_t pci_config_read_byte(int bus, int slot, int func, int offset) {
	uint32_t temp = pci_config_read_long(bus, slot, func, offset);
	return temp >> ((offset & 3) << 3) & 0xFF;
}

static void pci_list_devices() {
	for (int bus = 0; bus < 256; bus ++) {
		for (int slot = 0; slot < 32; slot++) {
		    for (int fun = 0; fun < 16; fun++) {
			// assumes first fun is defined if second & so...
			uint16_t vendor = pci_config_read_short(bus, slot, fun, 0);
			if (vendor != 0xFFFF) {
				uint16_t device = pci_config_read_short(bus, slot, fun, 2);
				printk("%d:%d:%d: vendor %04x, device %04x\n",
					bus, slot, fun, vendor, device);

				uint16_t clazz = pci_config_read_short(bus, slot, fun, 10);
				printk("  class code %04x", clazz);
				if (clazz == 0x0101) {
					printk(" - an IDE controller; bus mastering ");
					uint8_t reserved = 
						pci_config_read_byte(bus, slot, fun, 0x09);
					printk("is %ssupported", reserved & 0x80 ? "" : "not ");
				}		
				printk("\n");
			}
			else {
				break;
			}
		    }
		}
	}
}

void initialize() {
	pci_list_devices();
}

};
