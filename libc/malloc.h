#ifndef __MALLOC_H__
#define __MALLOC_H__

#ifdef __cplusplus 
extern "C" { 
#endif 

void *kmalloc(size_t);
void *kcalloc(size_t, size_t);
void *krealloc(void *, size_t);
void  kfree(void *);
int   kmalloc_init(void *, size_t); /* this should be called by init */

#ifdef __cplusplus
};
#endif

#endif
