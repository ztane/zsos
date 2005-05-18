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
		char pad_char = ' ';
		unsigned int pad_length = 0;
		unsigned int len;
		
		if (! cmd) {
			break;
		}
		if (cmd != '%') {
			buf[0] = cmd;
			buf[1] = 0;
			goto print_it;
		}
		else {
			cmd = *fmt++;
		}

		if (cmd >= '0' && cmd <= '9') {
			if (cmd == '0') {
				pad_char = '0';
				cmd = *fmt++;
			}
			while (cmd >='0' && cmd <= '9') {
				pad_length *= 10;
				pad_length += cmd - '0';
				cmd = *fmt ++;
			}
			
			if (! cmd) 
				break;
		}
		
                switch(cmd) {
	                case 's':
        	       		s = va_arg(ap, char *);
                        	break;

			/* binary! */
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

			/* NOTICE: upper case always! */
	                case 'x':           
	                case 'X':           
        	                u = va_arg(ap, unsigned int);
				uintoxa(buf, sizeof(buf), u);
	                        break;

        	        case 'c':        
        	                cmd = (char)va_arg(ap, int);
				/* fall through */ 
			default:
				/* for others, just print the following character */
				buf[0] = cmd;
				buf[1] = 0;
        	}

print_it:	
		len = strlen(s);
		if (pad_length > 0 && len < pad_length) {
			pad_length = pad_length - len;
			while (pad_length && pos <= n - 1) {
			        pad_length --;
				str[pos ++] = pad_char;
			}
		}
		/* strncpy and advance... */
		strncpy(str + pos, s, n - pos - 1);
		pos += len;

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


