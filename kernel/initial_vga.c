/*
 *	direct VGA buffer access "driver" for x86
 *	use only for debugging and such
 *
 *	for this to work the screen needs to
 *	be 80 x 25 characters
 */

#include <stdlib.h>
#include <string.h>
#include <kernel/arch/current/port.h>

static unsigned char * const VGA_TEXT_BASE = (unsigned char * const) 0xC00B8000;
#define VGA_TEXT_WIDTH  80
#define VGA_TEXT_HEIGHT 25
#define VGA_TEXT_LENGTH (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2)

static unsigned int cursor_offset = 0;

void   init_vga_buffer();
size_t vga_buf_write(const void *, size_t);

static void scroll_buffer();

void init_vga_buffer()
{
	outb(0x3d4, 14);
	cursor_offset = inb(0x3d5) << 8;
	outb(0x3d4, 15);
	cursor_offset |= inb(0x3d5);
}

size_t vga_buf_write(const void *vbuf, size_t len)
{
	unsigned char *charmem = VGA_TEXT_BASE;
	const unsigned char *buf = (const unsigned char *)vbuf;
	
	charmem += cursor_offset * 2;
	
	while (len-- > 0)
	{
		while (cursor_offset >= (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT))
		{
			scroll_buffer();
			cursor_offset -= VGA_TEXT_WIDTH;
			charmem -= VGA_TEXT_WIDTH * 2;
		}
		
		if (*buf == '\n')
		{
			unsigned int offset = VGA_TEXT_WIDTH - (cursor_offset % VGA_TEXT_WIDTH);
			charmem += offset * 2;
			cursor_offset += offset;
			buf++;
		}
		else if (*buf == '\t')
		{
			unsigned int offset = (cursor_offset + 8) & 0xfffffff8;
			charmem += (offset - cursor_offset) * 2;
			cursor_offset = offset;
			buf++;
		}
		else
		{
			*charmem = *buf++;
			charmem += 2;
			cursor_offset++;
		}
	}
	
	outb(0x3d4, 15);
	outb(0x3d5, (unsigned char) cursor_offset);
	outb(0x3d4, 14);
	outb(0x3d5, (unsigned char) (cursor_offset >> 8));
	
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


