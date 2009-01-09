/*
 *	direct VGA buffer access "driver" for x86
 *	use only for debugging and such
 *
 *	for this to work the screen needs to
 *	be 80 x 25 characters
 */

#include <stdlib.h>
#include <string.h>
#include <printk.h>
#include <kernel/arch/current/port.h>

static unsigned char * const VGA_TEXT_BASE = (unsigned char * const) 0xC00B8000;

#define VGA_TEXT_WIDTH  80
#define VGA_TEXT_HEIGHT 25
#define VGA_TEXT_LENGTH (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2)

#define ATTR_BLINK      1
#define ATTR_HI_INT     2
#define ATTR_REV        4

static unsigned int   cursor_offset = 0;
static unsigned char *charmem;

static unsigned int   saved_cursor  = 0;

#define MAX_CONTROL_STRING_CHARS  32

#define execute_command             while (0) printk

static char current_attributes = 7;

static int current_foreground  = 7;
static int current_background  = 0;
static int current_attr_flags  = 0;

void   init_vga_buffer();
size_t vga_buf_write(const void *, size_t);

static void scroll_buffer();

void init_vga_buffer()
{
	outb(0x3d4, 14);
	cursor_offset = inb(0x3d5) << 8;
	outb(0x3d4, 15);
	cursor_offset |= inb(0x3d5);
       	charmem = VGA_TEXT_BASE + cursor_offset * 2;
		
}

void update_cursor_position(int cursor_offset) {
	outb(0x3d4, 15);
	outb(0x3d5, (unsigned char) cursor_offset);
	outb(0x3d4, 14);
	outb(0x3d5, (unsigned char) (cursor_offset >> 8));
}

void scroll_if_necessary() {
	while (cursor_offset >= (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT))
	{
		scroll_buffer();
		cursor_offset -= VGA_TEXT_WIDTH;
		charmem       -= VGA_TEXT_WIDTH * 2;
	}
}

void vga_buf_write_char(char ch) {
	scroll_if_necessary();

	if (ch == '\n')
	{
		unsigned int offset  = VGA_TEXT_WIDTH - (cursor_offset % VGA_TEXT_WIDTH);
		charmem             += offset * 2;
		cursor_offset       += offset;
	}

	else if (ch == '\t')
	{
		unsigned int offset  = (cursor_offset + 8) & 0xfffffff8;
		charmem             += (offset - cursor_offset) * 2;
		cursor_offset        = offset;
	}

	else
	{
		*charmem++ = ch;
		*charmem++ = current_attributes;
		cursor_offset++;
	}

	scroll_if_necessary();
}

// strsep with one delimiter char -- e.g. empty fields returned
char *strsepch(register char **stringp, int delim) {
        if (! *stringp)
                return NULL;

        register char *ptr = strchr(*stringp, delim);
        register char *orig = *stringp;

        if (ptr) {
                *ptr = 0;
                ptr ++;
                *stringp = ptr;
                return orig;
        }

        *stringp = NULL;
        return orig;
}

int get_param(char **buf, int when_empty, int when_invalid) {
        char *component = strsepch(buf, ';');
        char ch;

        if (! component || ! *component) {
                return when_empty;
        }

        int val = 0;
        while ((ch = *component)) {
                if (ch < '0' || ch > '9')
                        return when_invalid;

                val *= 10;
                val += ch - '0';
                component ++;
        }
        return val;
}

// ansi to vga palette...
static int colors[] = {	0, 4, 2, 6, 1, 5, 3, 7, 7, 7 };

void do_attrs() {
	int fg = current_foreground;
	int bg = current_background;

	if (current_attr_flags & ATTR_REV) {
		fg = current_background;
		bg = current_foreground;
	}
	if (current_attr_flags & ATTR_HI_INT)
		fg |= 0x8;

	if (current_attr_flags & ATTR_BLINK)
		bg |= 0x8;

	current_attributes = (bg << 4) | fg;
}

void change_attrs(int cmd) {
	if (cmd > 47)
		return;

	if (cmd == 0) {
		current_foreground  = 7;
		current_background  = 0;
		current_attr_flags  = 0;
	}
	else if (cmd == 1) {
		current_attr_flags |=  ATTR_HI_INT;
	}
	else if (cmd == 5 || cmd == 6) {
		current_attr_flags |=  ATTR_BLINK;
	}
	else if (cmd == 7) {
		current_attr_flags |=  ATTR_REV;
	}
	else if (cmd == 27) {
		current_attr_flags &= ~ATTR_REV;
	}
	else if (cmd >= 30) {
		cmd -= 30;
		if (cmd > 10) {
			current_background = colors[cmd - 10];
		}
		else {
			current_foreground = colors[cmd];
		}
	}

	do_attrs();
}

void set_cursor(int col, int row) {
	if (row < 0) {
		row = 0;
	}

	if (col < 0) {
		col = 0;
	}

	if (row >= VGA_TEXT_HEIGHT) {
		row = VGA_TEXT_HEIGHT - 1;
	}

	if (col >= VGA_TEXT_WIDTH) {
		col = VGA_TEXT_WIDTH - 1;
	}

	cursor_offset = row * VGA_TEXT_WIDTH + col;
	charmem = VGA_TEXT_BASE + 2 * cursor_offset;
}

void cursor_move_rel(int cols, int rows) {
	int current_col = cursor_offset % VGA_TEXT_WIDTH + cols;
	int current_row = cursor_offset / VGA_TEXT_WIDTH + rows;

	set_cursor(current_col, current_row);
}

void interpret_command(char command, char *parambuf) {
        int param1, param2;
        switch (command) {
        case 'A':
                param1 = get_param(&parambuf, 1, 0);
		cursor_move_rel(0, -param1);
                break;

        case 'B':
                param1 = get_param(&parambuf, 1, 0);
		cursor_move_rel(0,  param1);
                break;

        case 'C':
                param1 = get_param(&parambuf, 1, 0);
		cursor_move_rel(param1, 0);
                break;

        case 'D':
                param1 = get_param(&parambuf, 1, 0);
		cursor_move_rel(-param1, 0);
                break;

        case 'E':
                param1 = get_param(&parambuf, 1, 1);
		// beginning of next line
		cursor_move_rel(param1, -1000);
                break;

        case 'F':
                param1 = get_param(&parambuf, 1, 1);
		// beginning of prev line
		cursor_move_rel(-param1, -1000);
                break;

        case 'G':
                param1 = get_param(&parambuf, 1, 1);
		set_cursor(param1, cursor_offset / VGA_TEXT_WIDTH);
                break;

        case 'H':
        case 'f':
		// order: 1 = row, 2 = col
                param1 = get_param(&parambuf, 1, 1);
                param2 = get_param(&parambuf, 1, 1);

		// 1-based to 0-based
		set_cursor(param2 - 1, param1 - 1);
                break;

        case 'J':
                param1 = get_param(&parambuf, 0, 0);
                execute_command("<Clear screen: %d>\n", param1);
                break;

        case 'K':
                param1 = get_param(&parambuf, 0, 0);
                execute_command("<Erase line: %d>\n", param1);
                break;

        case 'm':
                while (parambuf) {
                        param1 = get_param(&parambuf, 0, -1);
                        if (param1 != -1)
				change_attrs(param1);
                }
                break;

        case 's':
		saved_cursor = cursor_offset;
                break;

        case 'u':
		cursor_offset = saved_cursor;
		charmem = VGA_TEXT_BASE + 2 * cursor_offset;
                break;
        }
}

size_t vga_buf_write(const void *vbuf, size_t len)
{
	const unsigned char *buf = (const unsigned char *)vbuf;
	char ch;
	static int escape_state = 0;
	static int parambuf_count = 0;
	static char parambuf[34];
	static char command;

	while (len-- > 0)
	{
		ch = *buf;
                if (escape_state == 0) {
                        if (ch == '\033')
                                escape_state = 1;
                        else if (ch == (char)'\233')
                                escape_state = 2;
                        else
                                vga_buf_write_char(ch);
                }

                // wait for CSI start...
                else if (escape_state == 1) {
                        if (ch == '[') {
                                escape_state = 2;
                                parambuf_count = 0;
                        }
                        else {
                                vga_buf_write_char('\033');
                                vga_buf_write_char(ch);
                                escape_state = 0;
                        }
                }

                // collect max 32 chars in our string...
                else if (escape_state == 2) {
                        if ((ch & 0xF0) == 0x20)
                                // intermediate bytes start
                                escape_state = 3;

                        else if ((ch & 0xF0) != 0x30) {
                                // command byte! Decode and act!
                                command = ch;
                                escape_state = 4;
                        }
                        else if (parambuf_count < MAX_CONTROL_STRING_CHARS) {
                                parambuf[parambuf_count ++] = ch;
                        }
                        // abandon char!
                }

                else if (escape_state == 3) {
                        // consume intermediate bytes
                        // we do not recognize yet any
                        // commands with intermediate bytes
                        if ((ch & 0xF0) != 0x20) {
                                escape_state = 5;
                        }
                }

                if (escape_state == 4) {
                        // aah... we have a command now!
                        // terminate parambuf
                        if (parambuf_count == MAX_CONTROL_STRING_CHARS) {
                                parambuf[parambuf_count] = '@';
                        }

                        parambuf[parambuf_count] = 0;

                        interpret_command(command, parambuf);
                        escape_state = 5;
                }

                // fall back to 0
                if (escape_state == 5) {
                        parambuf_count = 0;
                        escape_state = 0;
                }

                buf ++;
	}

	update_cursor_position(cursor_offset);
	return 0;
}

static void scroll_buffer()
{
	/*
	for every character in buffer except first row:
		move back 1 position with color data
	*/
	
	memmove(VGA_TEXT_BASE, VGA_TEXT_BASE + VGA_TEXT_WIDTH * 2, VGA_TEXT_LENGTH - VGA_TEXT_WIDTH * 2);
	_memsetd((VGA_TEXT_BASE + VGA_TEXT_LENGTH - VGA_TEXT_WIDTH * 2), 0x07200720, VGA_TEXT_WIDTH / 2);
}


