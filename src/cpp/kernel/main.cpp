#include "../../../include/bear/gdt.hpp"
#include "../../../include/bear/serial.hpp"
#include "../../../include/bear/shell.hpp"

namespace bear {
    void kernel_main() {
        init_gdt();
        Serial::init();
        Serial::writeln("bear kernel initialized");

        Shell shell;
        shell.init();
        shell.run();

        asm volatile ("hlt");
    }
}

extern "C" void kmain() {
    bear::kernel_main();
}
