#ifndef __UNSTDLIB_H__
#define __UNSTDLIB_H__


#ifdef __cplusplus 
extern "C" { 
#endif

int itostr(char *buf, size_t n, int radix, int i);
int uitostr(char *buf, size_t n, int radix, unsigned int l);
int intoa(char *buf, size_t n, int i);
int uintoa(char *buf, size_t n, int i);
int intoxa(char *buf, size_t n, unsigned int i);
int uintoxa(char *buf, size_t n, unsigned int i);

////
// wchar versions

#include "wchar.h"

int itowstr  (wchar_t *buf, size_t n, int radix, int i);
int uitowstr (wchar_t *buf, size_t n, int radix, unsigned int l);
int intowcs  (wchar_t *buf, size_t n, int i);
int uintowcs (wchar_t *buf, size_t n, int i);
int intoxwcs (wchar_t *buf, size_t n, unsigned int i);
int uintoxwcs(wchar_t *buf, size_t n, unsigned int i);

#ifdef __cplusplus 
};
#endif

#endif
