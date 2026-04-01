#pragma once
#include <cstdint>

namespace bear {
    /**
    * global descriptor table entry structure
    */
    struct GDTEntry {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
    } __attribute__((packed));

    /**
    * gdt descriptor for lgdt instruction
    */
    struct GDTDescriptor {
        uint16_t size;
        uint64_t offset;
    } __attribute__((packed));

    extern "C" void setup_gdt();
    extern "C" void flush_gdt();

    void init_gdt();
}
