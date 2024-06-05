#include "defs.h"
#include "basic.h"
#include <stdarg.h>

void debug_print_int(long d, int jinzhi) {
	char l[] = "0123456789abcdef";
	if (d < 0) {
		debug_print_char('-');
		d *= -1;
	}
	if (d < jinzhi) {
		debug_print_char(l[d]);
		return;
	}
	debug_print_int(d / jinzhi, jinzhi);
	debug_print_char(l[d % jinzhi]);
}

void debug_print_addr(uint64 p) {
	char l[] = "0123456789abcdef";
	debug_print("0x");
	for (int i = 64 - 4; i >= 0; i -= 4) {
		debug_print_char(l[(p >> i) & 0xf]);
	}
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
			debug_print_int(va_arg(args, long), 10);
			break;
		case 'x':
			debug_print("0x");
			debug_print_int(va_arg(args, long), 16);
			break;
		case 's':
			debug_print(va_arg(args, char*));
			break;
		case 'p':
			debug_print_addr((uint64)va_arg(args, long));
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


void debug_println(char *s) {
	debug_printf("%s\n", s);
}

void backtrace() {
	uint64 fp, ra;
	dpln("Backtrace:");
	fp = r_fp();
	while (1) {
		ra = *(uint64 *)(fp - 8);
		dpf1("\t%p\n", ra);
		fp = *(uint64 *)(fp - 16);
		if (fp == PGROUNDUP(fp)) {
			break;
		}
	}
}

void panic(char *s) {
	debug_print("\npanic: ");
	debug_print(s);
	debug_print("...\n");
	for(;;)
		;
}

void *memset(void *dst, char c, uint n) {
	for (int i = 0; i < n; i++) {
		((char *)dst)[i] = c;
	}
	return dst;
}

void *memcpy(void *dst, void *src, uint n) {
	for (int i = 0; i < n; i++) {
		((uint8 *)dst)[i] = ((uint8 *)src)[i];
	}
	return dst;
}

int cpuid() {
	int id = r_tp();
	return id;
}


