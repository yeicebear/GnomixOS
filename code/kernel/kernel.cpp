#include "../vga/vga.hpp"
#include "../drivers/pic.hpp"
#include "../include/fs.hpp"
#include "../shell/shell.hpp"

extern "C" void kmain()
{
    vga_clear();
    pic_init();
    vga_println("PIC initialized");
    vga_println("BearNugget 0.1");
    vga_println("type 'help' for commands");
    vga_putchar('\n');

    fs_init();
    vga_println("fs init ok");

    vga_println("entering shell_run");
    shell_run();
    vga_println("returned from shell_run");

    while (1) {
        asm volatile("hlt");
    }
}