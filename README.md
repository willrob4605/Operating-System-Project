#xv6 system call tracing

##Team members
-Will Roberts - Kernel and Testing Lead
-Anish Singh - Documentation Lead

##Prerequisites
-GCC (32-bit support required)
-QEMU (qemu-system-i386)
-MAKE
##To install on Ubuntu
sudo apt-get install gcc-multilib qemu-system-x86 make

#Build instructions
clone https://github.com/willrob4605/Operating-System-Project
cd Operating-system-project
make clean
make qemu QEMU=qemu-system-i386

#Testing Instructions
Once xv6 boots and it shows $ prompt in the terminal run:
test_trace
