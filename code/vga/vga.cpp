#include "vga.hpp"
static volatile uint16_t* buf = (volatile uint16_t*)0xB8000;
static int col = 0;
static int row = 0;
static const int COLS = 80;
static const int ROWS = 25;
static const uint8_t COLOR = 0x0F;

static void scroll()
{
    for (int r = 1; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            buf[(r-1)*COLS+c] = buf[r*COLS+c];
    for (int c = 0; c < COLS; c++)
        buf[(ROWS-1)*COLS+c] = (COLOR << 8) | ' ';
    row = ROWS - 1;
}

void vga_clear()
{
    for (int i = 0; i < ROWS*COLS; i++)
        buf[i] = (COLOR << 8) | ' ';
    col = row = 0;
}

void vga_putchar(char c)
{
    if (c == '\n') {
        col = 0;
        row++;
    } else if (c == '\b') {
        if (col > 0) col--;
        buf[row*COLS+col] = (COLOR << 8) | ' ';
    } else {
        buf[row*COLS+col] = ((uint16_t)COLOR << 8) | c;
        col++;
        if (col >= COLS) { col = 0; row++; }
    }
    if (row >= ROWS) scroll();
}

void vga_print(const char* s)
{
    while (*s) vga_putchar(*s++);
}

void vga_println(const char* s)
{
    vga_print(s);
    vga_putchar('\n');
}
