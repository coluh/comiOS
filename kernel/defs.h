// Type: Initial
void init_kernel_memory();

// memory/kalloc
void *kalloc(void);
void kfree(void *p_addr);

// util
extern debug_print_char(char c);
extern debug_print(char *s);
void debug_print_int(int d);
int debug_printf(char *fmt, ...);
void panic(char *s);

