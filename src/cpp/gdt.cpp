#include "../../include/bear/gdt.hpp"
#include <cstdint>

namespace bear {
    GDTEntry gdt_entries[3];
    GDTDescriptor gdt_desc;

    void create_gdt_entry(int index, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t granularity) {
        gdt_entries[index].base_low = base & 0xFFFF;
        gdt_entries[index].base_mid = (base >> 16) & 0xFF;
        gdt_entries[index].base_high = (base >> 24) & 0xFF;

        gdt_entries[index].limit_low = limit & 0xFFFF;
        gdt_entries[index].granularity = (limit >> 16) & 0x0F;
        gdt_entries[index].granularity |= granularity & 0xF0;

        gdt_entries[index].access = access;
    }

    void init_gdt() {
        create_gdt_entry(0, 0, 0, 0, 0);
        create_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xA0);
        create_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xA0);

        gdt_desc.size = (sizeof(GDTEntry) * 3) - 1;
        gdt_desc.offset = (uint64_t)&gdt_entries[0];

        setup_gdt();
        flush_gdt();
    }
}
