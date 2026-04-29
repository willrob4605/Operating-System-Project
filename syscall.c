#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
enum argtype {
  ARG_END = 0,  // no more arguments
  ARG_INT = 1,  // integer
  ARG_STR = 2,  // string pointer
  ARG_PTR = 3   // other pointer
};

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_trace(void); //add system trace call

static int syscall_argtypes[][4] = {
[SYS_fork]   = {ARG_END},
[SYS_exit]   = {ARG_END},
[SYS_wait]   = {ARG_END},
[SYS_pipe]   = {ARG_PTR, ARG_END},
[SYS_read]   = {ARG_INT, ARG_PTR, ARG_INT, ARG_END},
[SYS_kill]   = {ARG_INT, ARG_END},
[SYS_exec]   = {ARG_STR, ARG_PTR, ARG_END},
[SYS_fstat]  = {ARG_INT, ARG_PTR, ARG_END},
[SYS_chdir]  = {ARG_STR, ARG_END},
[SYS_dup]    = {ARG_INT, ARG_END},
[SYS_getpid] = {ARG_END},
[SYS_sbrk]   = {ARG_INT, ARG_END},
[SYS_sleep]  = {ARG_INT, ARG_END},
[SYS_uptime] = {ARG_END},
[SYS_open]   = {ARG_STR, ARG_INT, ARG_END},
[SYS_write]  = {ARG_INT, ARG_PTR, ARG_INT, ARG_END},
[SYS_mknod]  = {ARG_STR, ARG_INT, ARG_INT, ARG_END},
[SYS_unlink] = {ARG_STR, ARG_END},
[SYS_link]   = {ARG_STR, ARG_STR, ARG_END},
[SYS_mkdir]  = {ARG_STR, ARG_END},
[SYS_close]  = {ARG_INT, ARG_END},
[SYS_trace]  = {ARG_INT, ARG_END},
};

//names array for when printing syscall names
static char *syscallnames[] = {
[SYS_fork]    "fork",
[SYS_exit]    "exit",
[SYS_wait]    "wait",
[SYS_pipe]    "pipe",
[SYS_read]    "read",
[SYS_kill]    "kill",
[SYS_exec]    "exec",
[SYS_fstat]   "fstat",
[SYS_chdir]   "chdir",
[SYS_dup]     "dup",
[SYS_getpid]  "getpid",
[SYS_sbrk]    "sbrk",
[SYS_sleep]   "sleep",
[SYS_uptime]  "uptime",
[SYS_open]    "open",
[SYS_write]   "write",
[SYS_mknod]   "mknod",
[SYS_unlink]  "unlink",
[SYS_link]    "link",
[SYS_mkdir]   "mkdir",
[SYS_close]   "close",
[SYS_trace]   "trace",
};

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace, //add to array of syscalls
};

// prints syscall arguments
static void
print_syscall_args(int num)
{
  int i, val;
  char *str;

  cprintf("(");

  for(i = 0; i < 3; i++){
    if(syscall_argtypes[num][i] == ARG_END)
      break;

    if(i > 0)
      cprintf(", ");

    if(syscall_argtypes[num][i] == ARG_INT){
      argint(i, &val);
      cprintf("%d", val);
    }
    else if(syscall_argtypes[num][i] == ARG_STR){
      if(argstr(i, &str) < 0)
        cprintf("?");
      else
        cprintf("\"%s\"", str);
    }
    else if(syscall_argtypes[num][i] == ARG_PTR){
      argint(i, &val);
      cprintf("%p", (void*)val);
    }
  }

  cprintf(")");
}
void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->eax = syscalls[num]();

    // if this syscall's bit is set in tracemask, print the trace
    if(curproc->tracemask & (1 << num)){
      cprintf("%d: syscall %s", curproc->pid, syscallnames[num]);
      print_syscall_args(num);
      cprintf(" -> %d\n", curproc->tf->eax);
    }

  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
