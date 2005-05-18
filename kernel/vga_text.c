/* 7.12.2004 - Ilja Everilä
 *
 * initial console - direct access to vga buffer
 *
 * 07/01/2005
 */
#include <stdlib.h>
#include <string.h>
#include "port.h"
#include "chardev.h"

#define VGA_TEXT_MEMORY   0xC00B8000
#define VGA_TEXT_WIDTH    80
#define VGA_TEXT_HEIGHT   25
#define VGA_TEXT_MEM_SIZE VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2

size_t vga_write(const void *, size_t);
void   init_vga_buffer();
static void           commit_vga_buffer();
static void           moveup_vga_buffer();
static unsigned short _newline(unsigned char **, unsigned short);

char_device_iface vga = {
	.read  = 0,
	.write = vga_write,
};

static unsigned char  vga_buffer[VGA_TEXT_MEM_SIZE];
static unsigned short vga_offset = 0;

size_t vga_write(const void *vbuf, size_t len)
{
	const unsigned char *buf = (const unsigned char *) vbuf;
	unsigned char *vidmem = vga_buffer;
	int i = 0;

	vidmem += vga_offset * 2;

	while (len-- > 0 && buf[i])
	{
		if (buf[i] == '\n')
		{
			vga_offset = _newline(&vidmem, vga_offset);
			i++;
			continue;
		}
		if (vga_offset > (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT - 1))
        	{
                	moveup_vga_buffer();
        	        vga_offset -= VGA_TEXT_WIDTH;
			vidmem -= VGA_TEXT_WIDTH * 2;
	        }
		*vidmem = buf[i++];
		vidmem += 2;
		vga_offset++;
	}

	commit_vga_buffer();

	return i;
}

void init_vga_buffer()
{
	memcpy(vga_buffer, (const void *) VGA_TEXT_MEMORY, sizeof(vga_buffer));

	outb(0x3D4, 14);
	vga_offset = inb(0x3D5) << 8;
	outb(0x3D4, 15);
	vga_offset |= inb(0x3D5);
}

static void commit_vga_buffer()
{
	memcpy((void *) VGA_TEXT_MEMORY, vga_buffer, sizeof(vga_buffer));

	outb(0x3d4, 15);
	outb(0x3d5, (unsigned char) vga_offset);
	outb(0x3d4, 14);
	outb(0x3d5, (unsigned char) (vga_offset >> 8));
}

static void moveup_vga_buffer()
{
	int i;
	unsigned char *clear_line = (vga_buffer + VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1) * 2);

	/* siirretään bufferia rivi ylöspäin */
	memcpy(vga_buffer, (vga_buffer + VGA_TEXT_WIDTH * 2), sizeof(vga_buffer) - VGA_TEXT_WIDTH * 2);
	/* korjaa tämä käyttämään jotain fiksumpaa joskus */
	for (i = 0; i < VGA_TEXT_WIDTH; i++)
	{
		*clear_line = 0;
		clear_line += 2;
	}
}

static unsigned short _newline(unsigned char **vidmem,
					      unsigned short offset)
{
	offset += VGA_TEXT_WIDTH - (offset % VGA_TEXT_WIDTH);

	if (offset > (VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT - 1))
	{
		moveup_vga_buffer();
		offset -= VGA_TEXT_WIDTH;
	}

	*vidmem = vga_buffer;
	*vidmem += offset * 2;
	outb(0x3d4, 15);
	outb(0x3d5, (unsigned char) offset);
	outb(0x3d4, 14);
	outb(0x3d5, (unsigned char) (offset >> 8));
	return offset;
}

