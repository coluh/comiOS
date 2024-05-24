#include "defs.h"
#include <stdarg.h>

void debug_print_int(int d) {
	if (d < 0) {
		debug_print_char('-');
		d *= -1;
	}
	if (d < 10) {
		debug_print_char('0' + d);
		return;
	}
	debug_print_int(d / 10);
	debug_print_char('0' + d % 10);
}

int debug_printf(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	for (char *p = fmt; *p != 0; p++) {
		if (*p != '%') {
			debug_print_char(*p);
			continue;
		}
		// *p == '%'
		p++;
		if (*p == 0) {
			panic("printf: %?");
		}
		switch(*p) {
		case 'd':
			debug_print_int(va_arg(args, int));
			break;
		case 'x':
			panic("printf: %x not support");
			break;
		case 's':
			debug_print(va_arg(args, char*));
			break;
		case 'p':
			panic("printf: %p not support");
			break;
		case 'f':
			panic("printf: %f not support");
			break;
		case '%':
			debug_print_char('%');
			break;
		default:
			debug_print("what is %");
			debug_print_char(*p);
			panic("printf");
			break;
		}
	}

	va_end(args);
	return 0;
}

void panic(char *s) {
	debug_print("\npanic: ");
	debug_print(s);
	debug_print("...\n");
	for(;;)
		;
}

