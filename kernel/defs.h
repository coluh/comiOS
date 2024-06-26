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

// filesystem/file
int fileadd(char *path, int flags);
int fileclose(int fd);
int fileread(int fd, uint64 uaddr, uint n);
int filewrite(int fd, uint64 uaddr, uint n);
int filedelete(char *path);
// filesystem/vnode
struct vnode;
int fillvnode(struct vnode *vp, char *path);
void kvnode_read(struct vnode *v, uint off, uint n, uint64 ka);

// memory/k
uint64 *	walk(uint64 *pagetable, uint64 v_addr, int alloc);
uint64		walkpaalign(uint64 *pagetable, uint64 v_addr);
void*		kalloc(void);
void		kfree(void *p_addr);
int		mappage(uint64 *pagetable, uint64 v_addr, uint64 p_addr,
			uint64 size, int perm);
// memory/u
void		ugrow_memory(uint64 *upt, uint64 sz, uint64 sz1, int perm);
int		copyin(uint64 ka, uint64 *upt, uint64 ua, uint n);
int		copyin_string(char *ka, uint max, uint64 *upt, char *s);
int		copyout(uint64 ka, uint64 *upt, uint64 ua, uint n);

// process/create
struct proc *	current_proc();

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
uint		strlen(char *s);
int		cpuid();

