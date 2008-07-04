#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <inttypes.h>
#include <stdarg.h>

#ifdef __cplusplus

# define NULL 0

#else

# define NULL ((void *) 0)
  typedef  int32_t wchar_t;

#endif

typedef unsigned int size_t;
typedef          int ssize_t;
typedef  int32_t ptrdiff_t;
typedef uint32_t wint_t;

#endif
