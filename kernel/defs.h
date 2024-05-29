#include "basic_types.h"

// Type: Initial
void		init_kernel_freememory();
void		init_kernel_pagetable();
void		init_kernel_trap();
void		plicinit();
void		plicinithart();

// memory/kmemory
void*		kalloc(void);
void		kfree(void *p_addr);
int		mappage(uint64 *pagetable, uint64 v_addr, uint64 p_addr,
			uint64 size, int perm);

// process/create
struct proc *	current_proc();

// util
extern void	debug_print_char(char c);
extern void	debug_print(char *s);
void		debug_print_int(int d);
int		debug_printf(char *fmt, ...);
#define dpf1(s, x) debug_printf(s, x);
void		debug_println(char *s);
#define dpln(s) debug_println(s)
void		panic(char *s);
void*		memset(void *dst, char c, uint n);
void*		memcpy(void *dst, void *src, uint n);
int		cpuid();

