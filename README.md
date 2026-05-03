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
sudo apt-get update
sudo apt-get install gcc-multilib qemu-system-x86 make
```

## Build Instructions
```bash
git clone -b feature/system-call-tracing https://github.com/willrob4605/Operating-System-Project
cd Operating-System-Project
make clean
make qemu QEMU=qemu-system-i386
```
**Note:** The `cd` command assumes the default cloned folder name. If the folder is renamed use that name instead.

## Testing Instructions
Once xv6 boots and you see the `$` prompt, run:
test_trace
**Note:** Test 2 traces both 'read' and 'write'. When the program pauses,
type something and press Enter to trigger the 'read' syscall and see it traced.
