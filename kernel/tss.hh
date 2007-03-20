#ifndef TSS_H_INCLUDED
#define TSS_H_INCLUDED

class TssContents {
public:
        unsigned short backlink, __blh;
        unsigned int   esp0;
        unsigned short ss0, __ss0h;
        unsigned int   esp1;
        unsigned short ss1, __ss1h;
        unsigned int   esp2;
        unsigned short ss2, __ss2h;
        unsigned int   cr3;
        unsigned int   eip;
        unsigned int   eflags;
        unsigned int   eax, ecx, edx, ebx;
        unsigned int   esp, ebp, esi, edi;
        unsigned short es, __esh;
        unsigned short cs, __csh;
        unsigned short ss, __ssh;
        unsigned short ds, __dsh;
        unsigned short fs, __fsh;
        unsigned short gs, __gsh;
        unsigned short ldt, __ldth;
        unsigned short trace, bitmap;
        void setup();
} __attribute__((packed));

#endif
