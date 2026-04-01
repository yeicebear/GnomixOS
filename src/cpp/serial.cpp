#include "../../include/bear/serial.hpp"
#include "../../include/bear/stdint.hpp"

namespace bear {
    void Serial::init() {
        asm volatile (
            "mov $0x3FB, %%dx \n\t"
            "mov $0x80, %%al \n\t"
            "out %%al, %%dx \n\t"
            : : : "dx", "al"
        );

        asm volatile (
            "mov $0x3F8, %%dx \n\t"
            "mov $0x01, %%al \n\t"
            "out %%al, %%dx \n\t"
            : : : "dx", "al"
        );

        asm volatile (
            "mov $0x3F9, %%dx \n\t"
            "mov $0x00, %%al \n\t"
            "out %%al, %%dx \n\t"
            : : : "dx", "al"
        );

        asm volatile (
            "mov $0x3FB, %%dx \n\t"
            "mov $0x03, %%al \n\t"
            "out %%al, %%dx \n\t"
            : : : "dx", "al"
        );
    }

    void Serial::putchar(char c) {
        uint8_t lsr;
        do {
            asm volatile (
                "mov $0x3FD, %%dx \n\t"
                "in %%dx, %%al \n\t"
                : "=a"(lsr) : : "dx"
            );
        } while (!(lsr & LSR_READY));

        asm volatile (
            "mov $0x3F8, %%dx \n\t"
            "out %%al, %%dx \n\t"
            : : "a"(c), "d"(0x3F8) : "dx"
        );
    }

    char Serial::getchar() {
        uint8_t lsr, data;
        do {
            asm volatile (
                "mov $0x3FD, %%dx \n\t"
                "in %%dx, %%al \n\t"
                : "=a"(lsr) : : "dx"
            );
        } while (!(lsr & LSR_DATA));

        asm volatile (
            "mov $0x3F8, %%dx \n\t"
            "in %%dx, %%al \n\t"
            : "=a"(data) : : "dx"
        );
        return data;
    }

    void Serial::write(const char* str) {
        for (int i = 0; str[i] != '\0'; i++) {
            putchar(str[i]);
        }
    }

    void Serial::writeln(const char* str) {
        write(str);
        putchar('\n');
        putchar('\r');
    }
}
