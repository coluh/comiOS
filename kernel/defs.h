#include "basic_types.h"

// Type: Initial
void		init_kernel_memory();

// memory/kalloc
void*		kalloc(void);
void		kfree(void *p_addr);

// pagetable/kpagetable
void		init_kernel_pagetable();

// util
extern void	debug_print_char(char c);
extern void	debug_print(char *s);
void		debug_print_int(int d);
int		debug_printf(char *fmt, ...);
void		panic(char *s);
void*		memset(void *dst, char c, uint n);

