#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <inttypes.h>

#ifdef __cplusplus

# define NULL 0

#else

# define NULL ((void *) 0)
  typedef  int32_t wchar_t;

#endif

typedef uint32_t size_t;
typedef  int32_t ptrdiff_t;
typedef uint32_t wint_t;

#endif
