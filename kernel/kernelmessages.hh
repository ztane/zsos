#ifndef __KERNEL_MESSAGES_HH__
#define __KERNEL_MESSAGES_HH__

#include <stddef.h>

typedef size_t (*CONSOLE_OUTPUT_FUNCP)(const void *vbuf, size_t len);
extern CONSOLE_OUTPUT_FUNCP writeToConsole;

#ifdef __cplusplus
extern "C"
#endif
void initConsoleDriver();

#endif // __KERNEL_MESSAGES_HH__
