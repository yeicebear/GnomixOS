export PATH := $(HOME)/cross-bin:$(PATH)
AS  = nasm
CXX = i686-elf-g++
LD  = i686-elf-ld

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector \
           -nostdlib -O2 -I./include -B /usr/local/bin


OBJS = boot/boot.o \
       kernel/entry.o \
       kernel/kernel.o \
       vga/vga.o \
	drivers/kb.o \
	drivers/pic.o \
       kernel/str.o \
       fs/fs.o \
       shell/shell.o

all: bear.img

boot/boot.o: boot/boot.asm
	$(AS) -f bin $< -o $@

kernel/entry.o: kernel/entry.asm
	$(AS) -f elf32 $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

kernel.elf: $(filter-out boot/boot.o, $(OBJS)) linker.ld
	$(LD) -T linker.ld -o $@ $(filter-out boot/boot.o linker.ld, $^)

kernel.bin: kernel.elf
	i686-elf-objcopy -O binary kernel.elf kernel.bin

bear.img: boot/boot.o kernel.bin
	cat boot/boot.o kernel.bin > bear.img
	truncate -s 1474560 bear.img

run: bear.img
	qemu-system-i386 -drive format=raw,file=bear.img

clean:
	find . -name '*.o' -delete
	rm -f kernel.bin bear.img
