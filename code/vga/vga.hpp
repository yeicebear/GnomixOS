#pragma once

#include <stdint.h>
void vga_clear();
void vga_putchar(char c);
void vga_print(const char* s);
void vga_println(const char* s);
