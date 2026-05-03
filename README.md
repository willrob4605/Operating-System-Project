# xv6 System Call Tracing

## Team Members
- Will Roberts - Kernel and Testing Lead
- Anish Singh - Documentation Lead

## Prerequisites
- GCC (32-bit support required)
- QEMU (qemu-system-i386)
- Make

## To Install on Ubuntu
```bash
sudo apt-get install gcc-multilib qemu-system-x86 make
```

## Build Instructions
```bash
git clone https://github.com/willrob4605/Operating-System-Project
cd Operating-System-Project
git checkout feature/system-call-tracing
make clean
make qemu QEMU=qemu-system-i386
```

## Testing Instructions
Once xv6 boots and you see the `$` prompt, run:
```bash
test_trace
```
## Test Programs
See the `tests/` directory for `test_trace.c`
