#ifndef __POKEPEEK_H_INCLUDED__
#define __POKEPEEK_H_INCLUDED__

static __inline__ void farpokeb(unsigned short sel, void *off, unsigned char v)
{
  asm("push %%fs; mov %0,%%fs;"
      "movb %2,%%fs:(%1);"
      "pop %%fs": :"r"(sel),"r"(off),"r"(v));
}

static __inline__ void farpokew(unsigned short sel, void *off, unsigned short v)
{
  asm("push %%fs; mov %0,%%fs;"
      "movw %2,%%fs:(%1);"
      "pop %%fs": :"r"(sel),"r"(off),"r"(v));
}

static __inline__ void farpokel(unsigned short sel, void *off, unsigned int v)
{
  asm("push %%fs; mov %0,%%fs;"
      "movl %2,%%fs:(%1);"
      "pop %%fs": :"r"(sel),"r"(off),"r"(v));
}




static __inline__ unsigned char farpeekb(unsigned short sel, void *off)
{
  unsigned char ret;
  asm("push %%fs;mov %1,%%fs;"
      "mov %%fs:(%2),%0;"
      "pop %%fs":"=r"(ret):"g"(sel),"r"(off));
  return ret;
}

static __inline__ unsigned short farpeekw(unsigned short sel, void *off)
{
  unsigned short ret;
  asm("push %%fs;mov %1,%%fs;"
      "mov %%fs:(%2),%0;"
      "pop %%fs":"=r"(ret):"g"(sel),"r"(off));
  return ret;
}

static __inline__ unsigned int farpeekl(unsigned short sel, void *off)
{
  unsigned int ret;
  asm("push %%fs;mov %1,%%fs;"
      "mov %%fs:(%2),%0;"
      "pop %%fs":"=r"(ret):"g"(sel),"r"(off));
  return ret;
}



#endif
