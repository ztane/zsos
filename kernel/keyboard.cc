#include "kernel/arch/current/port.h"
#include "kernel/ktasks/softirq.hh"
#include "kernel/atomic.hh"

#include <iostream>
#include "kernel/panic.hh"

#define KB_INPUT          0x60
#define KB_OUTPUT         0x60
#define KB_STATUS         0x64

#define KB_CMD_RESET      0xFF

#define KB_ACK            0xFA

#define KB_DATA_AVAILABLE 0x01
#define KB_BUF2KBD_FULL   0x02

#define KB_CMD_SET_RATE_DELAY 0xF3

#define KB_AUX_READ_CMD_BYTE  0x20
#define KB_AUX_WRITE_CMD_BYTE 0x60

#define KB_ENABLE_INTERRUPT   0x01 // in Command Byte

static inline int readKb() {
    return inb(KB_INPUT);
}

static inline int readKbWait() {
    while (! (inb(KB_STATUS) & KB_DATA_AVAILABLE));

    return readKb();
}

static void acknowledge() {
    unsigned char status = inb(0x61);
    outb(0x61, status | 0x80);
    outb(0x61, status);
}

static inline void emptyKbBuffer() {
    acknowledge();
    while (inb(KB_STATUS) & KB_DATA_AVAILABLE) {
        readKb();
        acknowledge();
    }
}

static inline void writeAuxCommand(unsigned char byte) {
    while (inb(KB_STATUS) & KB_BUF2KBD_FULL);
    outb(KB_STATUS, byte);
}

static inline void writeKb(unsigned char byte) {
    // wait for output buffer to become empty!

    while (inb(KB_STATUS) & KB_BUF2KBD_FULL);
    outb(KB_OUTPUT, byte);
}

static inline int writeKbResult(unsigned char byte) {
    writeKb(byte);
    return readKbWait();
}

static volatile Atomic keybuf_pointer = 0;
static volatile bool     keyboard_state[512];
static volatile int      char_buffer[128];

static const int EXTENDED = 1;
static const int BREAK    = 2;
static const int PAUSE    = 4;

static int pause_state = 0;
static volatile Atomic current_state = 0;

static const int KEY_PAUSE        = 0xE1;
static const int KEY_PRINT_SCREEN = 0xFF;

static void queueKeyDown(int key) {
    kout << "Key down: " << key << "\n";
}

static void queueKeyUp(int key) {
    kout << "Key up: " << key << "\n";
}

static void keyboardRoutine(int vector) {
    while (inb(KB_STATUS) & KB_DATA_AVAILABLE) {
        uint8_t byte = readKb();
        acknowledge();

        // stupid idiocy - handle pause!
        if (current_state & PAUSE) {
            if (pause_state == 0 && byte == 0x1d) {
                pause_state = 1;
            }
            else if (pause_state == 1 && byte == 0x45) {
                pause_state = 2;
            }
            else if (pause_state == 2 && byte == 0xe1) {
                pause_state = 3;
            }
            else if (pause_state == 3 && byte == 0x9d) {
                pause_state = 4;
            }
            else if (pause_state == 4 && byte == 0xc5) {
                pause_state = 0;
                queueKeyDown(KEY_PAUSE);
                queueKeyUp(KEY_PAUSE);
            }
            else {
                current_state &= ~PAUSE;
            }
        }
        else if (byte == 0xE0) {
            current_state |= EXTENDED;
	}
        else if (byte == 0xE1) {
            pause_state = 0;
            current_state |= PAUSE;
        }
        else {
            if (byte & 0x80) {
                current_state |= BREAK;

                // clear topmost bit
                byte &= 0x7F;
            }
            int key = current_state & EXTENDED ? 0x80 | byte : byte;

            if (current_state & BREAK)
               queueKeyUp(key);
            else
               queueKeyDown(key);

            current_state = 0;
        }
    }    
}

// kbWrite:
// WaitLoop:    in     al, 64h     ; Read Status byte
//             and    al, 01b     ; Test OBF flag (Status<0>)
//             jnz    WaitLoop    ; Wait for OBF = 0
//             out    60h, cl     ; Write data to output buffer 
// }

bool initKeyboard() {
        int result;
        
	emptyKbBuffer();

	if ((result = writeKbResult(KB_CMD_RESET)) != KB_ACK) {
		kout << "Keyboard did not accept reset command: " << result << endl;
		return false;
	}

	if (readKbWait() != 0xAA) {
		kout << "Keyboard reset not successful" << endl;
		return false;
	}

        if ((result = writeKbResult(KB_CMD_SET_RATE_DELAY)) != KB_ACK) {
   	        kout << "Set typematic rate/delay not acknowledged!" << result << endl;
		return false;
        }

        if (writeKbResult(0x00) != KB_ACK) {
		kout << "Set typematic rate/delay arg not acknowledged!" << endl;
		return false;
        }

        if (writeKbResult(0xF0) != KB_ACK) {
		kout << "Set scancode set not acknowledged!" << endl;
		return false;
        }

        if (writeKbResult(0x02) != KB_ACK) {
		kout << "Set scancode set arg not acknowledged!" << endl;
		return false;
        }

        writeAuxCommand(KB_AUX_READ_CMD_BYTE);
        unsigned char cmdbyte = readKbWait();
        writeAuxCommand(KB_AUX_WRITE_CMD_BYTE);
        writeKb(cmdbyte | KB_ENABLE_INTERRUPT);

	registerSoftIrq(2, keyboardRoutine);
        return true;
}

