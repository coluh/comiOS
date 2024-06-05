#include "basic_types.h"

// Type: Initial
void		init_kernel_freememory();
void		init_kernel_pagetable();
void		init_kernel_trap();
void		plicinit();
void		plicinithart();
void		init_processes();
void		init_file();
void		init_disk();
void		init_userprocess();

// filesystem/diskio
void disk_read_block(void *dst, int blockid);
void disk_write_block(void *src, int blockid);
// filesystem/inode
struct inode;
void uinode_read(uint64 *upt, uint64 uaddr, struct inode *inode, uint64 off, uint n);
void kinode_read(uint64 vaddr, struct inode *inode, uint64 off, uint n);
int getinode(struct inode *buf, int i);

// filesystem/file
int getinode_frompath(struct inode *inode, char *path);

// memory/k
uint64 *	walk(uint64 *pagetable, uint64 v_addr, int alloc);
uint64		walkpaalign(uint64 *pagetable, uint64 v_addr);
void*		kalloc(void);
void		kfree(void *p_addr);
int		mappage(uint64 *pagetable, uint64 v_addr, uint64 p_addr,
			uint64 size, int perm);
// memory/u
void		ugrow_memory(uint64 *upt, uint64 sz, uint64 sz1, int perm);
int		copyin(void *ka, uint64 *upt, void *ua, uint n);
int		copyin_string(char *ka, uint max, uint64 *upt, char *s);
int		copyout(void *ka, uint64 *upt, void *ua, uint n);

// process/create
struct proc *	current_proc();
struct proc *	allocate_proc();

// util
extern void	debug_print_char(char c);
extern void	debug_print(char *s);
void		debug_print_int(long d, int jinzhi);
void		debug_print_addr(uint64 p);
int		debug_printf(char *fmt, ...);
#define dpf1(s, x) debug_printf(s, x);
#define dpf2(s, x, y) debug_printf(s, x, y);
void		debug_println(char *s);
#define dpln(s) debug_println(s);
void		backtrace();
void		panic(char *s);
void*		memset(void *dst, char c, uint n);
void*		memcpy(void *dst, void *src, uint n);
int		strcmp(char *a, char *b);
int		cpuid();

