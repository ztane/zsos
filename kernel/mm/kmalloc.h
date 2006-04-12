#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#ifdef __cplusplus 
extern "C" { 
#endif 

void *kmalloc(size_t size);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);
void  kfree(void *ptr);

int   kmalloc_init(void *ptr, size_t size); /* this should be called by init */

int   kbrk(void *end_data_segment);
void *ksbrk(intptr_t increment);

#ifdef __cplusplus
};
#endif

#endif