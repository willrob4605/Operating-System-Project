#include "types.h"
#include "user.h"
#include "syscall.h"

int
main(void)
{
  // Test 1: trace only write
  printf(1, "Test 1: trace write only \n");
  trace(1 << SYS_write);
  write(1, "Hello World!\n", 13);
  trace(0);

  // Test 2: trace read and write
  printf(1, "--- Test 2: trace read and write ---\n");
  trace(1 << SYS_read | 1 << SYS_write);
  write(1, "tracing both\n", 13);
  char buf[20];
  read(0, buf, sizeof(buf)); 
  trace(0);

  // Test 3: child inherits tracemask from parent
  printf(1, "Test 3: fork inherits trace \n");
  trace(1 << SYS_write);
  if(fork() == 0){
    write(1, "child is traced too\n", 20);
    exit();
  }
  wait();
  trace(0);

  // Test 4: invalid mask (negative) should return -1
  printf(1, "Test 4: invalid mask\n");
  if(trace(-1) < 0)
    printf(1, "trace(-1) correctly returned -1\n");

  // Test 5: trace getpid
  printf(1, "Test 5: trace getpid\n");
  trace(1 << SYS_getpid);
  getpid();
  trace(0);

  printf(1, "all tests done\n");
  exit();
}
