#include "kb.hpp"
#include <stdint.h>

static const char scancodes[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

char keyboard_getchar()
{
    static uint8_t last_sc = 0;

    if (!(inb(0x64) & 1)) {
        return 0;
    }

    uint8_t sc = inb(0x60);

    if (sc == last_sc) {
        return 0;
    }

    last_sc = sc;

    if (sc & 0x80) {
        return 0;
    }

    if (sc >= sizeof(scancodes) / sizeof(scancodes[0])) {
        return 0;
    }

    return scancodes[sc];
}

