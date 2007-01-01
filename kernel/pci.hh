#ifndef __PCI_HH_INCLUDED__
#define __PCI_HH_INCLUDED__

#include <inttypes.h>
#include <port.h>

namespace PCI {
	void initialize();

	class PCIDevice;
	class PCIRegister {
	private:
		uint32_t address;
		static const int PCI_CONFIG_ADDRESS = 0xCF8;
		static const int PCI_CONFIG_DATA    = 0xCFC;

		PCIRegister(uint32_t a) {
			address = a | 0x80000000;
		}
	public:
		inline uint32_t read() const {
			outl(PCI_CONFIG_ADDRESS, address);
			return inl(PCI_CONFIG_DATA);
		}
		inline operator uint32_t() const {
			return read();			
		}

		inline PCIRegister& operator =(uint32_t val) {
			outl(PCI_CONFIG_ADDRESS, address);
			outl(PCI_CONFIG_DATA,    val);
			return *this;
		}
		friend class PCIConfig;
	};
	
	class PCIConfig {
	private:
		uint32_t address;
		PCIConfig(uint32_t a) 
		{
			address = a;
		}
		PCIConfig(uint32_t bus, 
			uint32_t slot, uint32_t func)
		{
			address = ((bus  & 0xFF) << 16) 
				| ((slot & 0x1F) << 11)
				| ((func & 0x7)  << 8);
		}

	public:		
		PCIRegister operator [] (uint32_t reg) {
			return PCIRegister(address + reg);
		}	
		const PCIRegister operator [] (uint32_t reg) const {
			return PCIRegister(address + reg);
		}

		uint32_t bus() const {
			return (address >> 16) & 0xFF;
		}

		uint32_t slot() const {
			return (address >> 11) & 0x1F;
		}

		uint32_t func() const {
			return (address >> 8) & 0x7;
		}

		uint16_t vendor() const {
			return (*this)[0];
		}

		uint16_t deviceID() const {
			return (*this)[0] >> 16;
		}

		uint16_t classID() const {
			return (*this)[0x8] >> 16;
		}

	friend class PCIDevice;
	friend class PCIInit;
	friend void pciListDevices();
	};

	class PCIDevice {
	private:
	public:
		PCIConfig config;
		PCIDevice(uint32_t bus, uint32_t slot, uint32_t func) 
			: config(bus, slot, func)
		{
		}

		PCIDevice() : config(0) {
		}

		operator bool()       const {
			return config[0] != 0xFFFFFFFF;
		}
		
		bool     operator !() const {
			return config[0] == 0xFFFFFFFF;
		}
	};
};

#endif
