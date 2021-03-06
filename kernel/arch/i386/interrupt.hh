// -*- C++ -*-

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define INTR_PRESENT	    0x80

#define INTR_ACCESS_MASK    0x60
#define INTR_ACCESS_RING_0  0x00
#define INTR_ACCESS_RING_1  0x20
#define INTR_ACCESS_RING_2  0x40
#define INTR_ACCESS_RING_3  0x60

#define INTR_TYPE_TASK_GATE     0x05 /* 16/32 bit op */
#define INTR_TYPE_386_CALL_GATE 0x0c /* 32 bit operations */
#define INTR_TYPE_386_INTR_GATE 0x0e /* 32 bit operations */
#define INTR_TYPE_386_TRAP_GATE 0x0f /* 32 bit operations */
#define INTR_TYPE_MASK          0x0f

/* a list of some well known interrupts
 * interrupts 0-31 as whole are CPU interrupts
 * interrupts 19-31 are reserved by Intel
 * interrupts 32-255 are available for soft and hard interrupts
 */
#define INTR_DIVIDE_BY_ZERO  0
#define INTR_DEBUG_EXCEPTION 1
#define INTR_NMI             2
#define INTR_BREAKPOINT      3
#define INTR_OVERFLOW        4
#define INTR_BOUND_EXCEPTION 5
#define INTR_INVALID_OPCODE  6
#define INTR_FPU_NOT_AVAIL   7
#define INTR_DOUBLE_FAULT    8
#define INTR_COP_SEGMENT_OVERRUN 9
#define INTR_INVALID_TSS     10
#define INTR_SEGMENT_NOT_PRESENT 11
#define INTR_STACK_EXCEPTION 12
#define INTR_GENERAL_PROT_FAULT  13
#define INTR_PAGE_FAULT      14
#define INTR_FLP_ERROR       16
#define INTR_ALIGN_CHECK     17
#define INTR_MACHINE_CHECK   18

#include <inttypes.h>

#define C_ISR(name)                                                     \
        void __ISR_ ## name(volatile Registers r,                       \
                unsigned int eip, unsigned int cs, unsigned int eflags)

#define C_ISR_W_ECODE(name)                                             \
        void __ISR_ ## name(Registers r, unsigned int errorcode,        \
                unsigned int eip, unsigned int cs, unsigned int eflags)


typedef struct Registers {
    union {
        volatile uint32_t edi;
        volatile uint32_t arg4;
    };

    union {
        volatile uint32_t esi;
        volatile uint32_t arg3;
    };

    union {
        volatile uint32_t ebp;
        volatile uint32_t arg5;
    };

    volatile uint32_t esp;

    union {
        volatile uint32_t ebx;
        volatile uint32_t arg0;
    };

    union {
        volatile uint32_t edx;
        volatile uint32_t arg2;
    };

    union {
        volatile uint32_t ecx;
        volatile uint32_t arg1;
    };

    union {
        uint32_t syscallNumber;
        uint32_t syscallRetval;
        uint32_t eax;
    };
    volatile uint32_t gs, fs, es, ds;
} REGS;

extern "C" {
	typedef void (* ISR_W_ECODE_TYPE)(Registers r, unsigned int errorcode,
		unsigned int eip, unsigned int cs, unsigned int eflags);

	typedef void (* ISR_TYPE)(Registers r, unsigned int eip, unsigned int cs,
		unsigned int eflags);
}

class ISRPointer {
private:
	void *pointer;
public:
	ISRPointer() 			{ pointer = 0;         }
	ISRPointer(void (&p)())		{ pointer = (void*)&p; }
	ISRPointer(void *p) 		{ pointer = p;         }
	void *get_value() const         { return pointer;      }
};

static unsigned short get_code_selector() { return 0x8; }

class InterruptDescriptor {
protected:
	unsigned char offset_7_0;
	unsigned char offset_15_8;
	unsigned char selector_7_0;
	unsigned char selector_15_8;
	unsigned char null;	/* set to zero for IDT entries */
	unsigned char access;
	unsigned char offset_23_16;
	unsigned char offset_31_24;
public:
	void set_handler(const ISRPointer pointer)
	{
		unsigned int offset = (unsigned int)pointer.get_value();

	        offset_7_0   =  offset & 0xff;
        	offset_15_8  = (offset >> 8)  & 0xff;
	        offset_23_16 = (offset >> 16) & 0xff;
        	offset_31_24 = (offset >> 24) & 0xff;

	        selector_7_0  =  get_code_selector() & 0xff;
        	selector_15_8 = (get_code_selector() >> 8) & 0xff;
	}

	void set_present(bool present) {
		access &= ~INTR_PRESENT;
		access |= present ? INTR_PRESENT : 0;
	}

	void set_access(int a) {
		access &= ~INTR_ACCESS_MASK;
		access |= a & INTR_ACCESS_MASK;
	}

	void set_type(int t) {
		access &= ~INTR_TYPE_MASK;
		access |= t & INTR_TYPE_MASK;
	}

	void set_all(const ISRPointer pointer, int type,
		int access = INTR_ACCESS_RING_0, bool present = true)
	{
		set_handler(pointer);
		set_present(present);
		set_access(access);
		set_type(type);
	}

	InterruptDescriptor()
	{
		null = 0x00;
		set_all((void *)0, INTR_TYPE_386_INTR_GATE,
			INTR_ACCESS_RING_0, false);
	}

	InterruptDescriptor(const ISRPointer handler, int type,
		int access = INTR_ACCESS_RING_0, bool present = true)
	{
		null = 0x00;
		set_all(handler, type, access, present);
	}

	bool is_present() const { return access & INTR_PRESENT;     }
	int get_access()  const { return access & INTR_ACCESS_MASK; }
	int get_type()    const { return access & INTR_TYPE_MASK;   }
};

void load_idt(InterruptDescriptor *start, int num_descs);

// void get_id_values(interrupt_descriptor *, const interrupt_info *);
// void set_id_values(const interrupt_info *, interrupt_descriptor *);

void initPic();

static __inline__ bool interruptsEnabled() {
        volatile uint32_t flags;
        asm volatile ("pushfl; popl %0": "=a"(flags) : );
        return (flags & 0x200);
}

static __inline__ bool disableInterrupts() {
	if (interruptsEnabled()) {
		asm volatile ("cli");
		return true;
	}
	return false;
}

static __inline__ void __enableInterrupts() {
	asm volatile ("sti");
}

static __inline__ void enableInterruptsIf(bool enable) {
	if (enable) {
		__enableInterrupts();
	}
}

void enableIrq(int number);
void disableIrq(int number);

#endif
