#include <stdarg.h>
#include "stdlib.h"
#include "string.h"
#include "unstdlib.h"

/*
   snprintf.
   Accepted modifiers are s, d, i, x and c.
   No short/long accepted (all arguments must be ints).
   Doesn't obey any width-modifiers etc Also, the return 
   value currently does not conform to C99.
*/
int vsnprintf(char *str, size_t n, const char *fmt, va_list ap)
{
        int d;
	unsigned int u;
	int pos = 0;

	// big enough to hold binary number and the trailing null
	char buf[sizeof(int) * 8 + 1];

	/* n - 1, to have space for the final 0 */
        while (*fmt && pos < n - 1) {
		int cmd = *fmt++;

		char *s = buf;
		if (cmd != '%') {
			buf[0] = cmd;
			buf[1] = 0;
			cmd = -1;
		}
		else {
			cmd = *fmt++;
		}
		if (! cmd) {
			break;
		}

                switch(cmd) {
			// just write 
			case -1: 
				break;

	                case 's':
        	       		s = va_arg(ap, char *);
                        	break;

			// binary!
	                case 'b':           
				u = va_arg(ap, unsigned int);
				uitostr(buf, sizeof(buf), 2, u);
				break;

	                case 'd':           
	                case 'i':           
  	    	                d = va_arg(ap, int);
				intoa(buf, sizeof(buf), d);
	                        break;
			case 'u':
				u = va_arg(ap, unsigned int);
				uintoa(buf, sizeof(buf), u);
				break;
	                case 'o':           
        	                u = va_arg(ap, unsigned int);
				uitostr(buf, sizeof(buf), 8, u);
	                        break;

			// NOTICE: upper case always! 
	                case 'x':           
	                case 'X':           
        	                u = va_arg(ap, unsigned int);
				uintoxa(buf, sizeof(buf), u);
	                        break;

        	        case 'c':        
        	                cmd = (char)va_arg(ap, int);
				// fall through 
			default:
				// for others, just print the following character
				buf[0] = cmd;
				buf[1] = 0;
        	}
	
		/* strncpy and advance... */
		strncpy(str + pos, s, n - pos - 1);
		pos += strlen(s);

	}
	
	if (pos > n - 1)
		pos = n - 1;

	str[pos] = 0;
	return pos;
}


int snprintf(char *str, size_t n, const char *fmt, ...)
{
	int rv;
	va_list ap;
        va_start(ap, fmt);
	rv = vsnprintf(str, n, fmt, ap);
	va_end(ap);
	return rv;
}


