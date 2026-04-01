#pragma once

namespace bear {
    /**
    * serial port operations for i/o
    */
    class Serial {
    public:
        static void init();
        static void putchar(char c);
        static char getchar();
        static void write(const char* str);
        static void writeln(const char* str);

    private:
        static const uint16_t PORT = 0x3F8;
        static const uint16_t THR = 0;
        static const uint16_t RBR = 0;
        static const uint16_t LSR = 5;
        static const uint8_t LSR_READY = 0x20;
        static const uint8_t LSR_DATA = 0x01;
    };
}
