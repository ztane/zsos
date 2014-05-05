#include <kernel/arch/current/atags.hh>
#include <printk.h>

struct tag_cmdline {
        char        cmdline[1];         /* this is the minimum size */
};

void atag_scanner(void *start_address) {
        uint32_t tag_value;
        uint32_t *atags;
        struct tag_cmdline *cmdline;

        for (atags = (uint32_t *)start_address; atags < (uint32_t *)0x8000; atags +=1) {
                switch (atags[1]) {
                         case ATAG_CORE:
                                printk("[ATAG_CORE] flags: %x, pagesize: %x, rootdev: %x\n", atags[2], atags[3], atags[4]);
                                break;

                         case ATAG_MEM:
                                printk("[ATAG_MEM] size: %x, start: %x\n", atags[2], atags[3]);
                                break;

                         case ATAG_VIDEOTEXT:
                                printk("[ATAG_VIDEOTEXT] x,y,video: %x, mode,cols, ega: %x, lines, vga, points: %x\n", atags[2], atags[3], atags[4]);
                                break;

                         case ATAG_RAMDISK:
                                printk("[ATAG_RAMDISK] flags: %x, size: %x, start: %x\n", atags[2], atags[3], atags[4]);
                                break;

                         case ATAG_INITRD2:
                                printk("[ATAG_INITRD2] size: %x, start: %x\n", atags[3], atags[2]);
                                break;

                         case ATAG_SERIAL:
                                printk("[ATAG_SERIAL#] low: %x, high: %x\n", atags[2], atags[3]);
                                break;

                         case ATAG_REVISION:
                                printk("[ATAG_REVISION] rev: %x\n", atags[2]);
                                break;

                         case ATAG_VIDEOLFB:
                                printk("[ATAG_VIDEOLFB] found\n");
                                break;

                         case ATAG_CMDLINE:
                                printk("[ATAG_CMDLINE] found: \n");
                                atags += 2;
                                cmdline = (struct tag_cmdline *)atags;
                                printk("%s\n", cmdline->cmdline);
                                break;

//                         case ATAG_ACORN:
//                                printk("[ATAG_ACORN] found\n");
//                                atags += atags[0] - 1;
//                                break;

//                         case ATAG_MEMCLK:
//                                printk("[ATAG_MEMCLK] memclck: %x\n", atags[2]);
//                                atags += atags[0] - 1;
//                                break;

                         case ATAG_NONE:
                                printk("[ATAG_NONE] List ends\n");
                                return;

                         default:
                                printk("Unknown ATAG: %x", atags[1]);
                                break;
                }
		atags += atags[0] - 1;
        }
}
