#include "../../include/bear/shell.hpp"
#include "../../include/bear/serial.hpp"
#include "../../include/bear/stdint.hpp"

namespace bear {
    void Shell::init() {
        Serial::writeln("shell initialized");
    }

    void Shell::run() {
        while (true) {
            print_prompt();
            char buffer[256];
            read_line(buffer, 256);
            parse_and_execute(buffer);
        }
    }

    void Shell::print_prompt() {
        Serial::write("> ");
    }

    void Shell::read_line(char* buffer, int max_len) {
        int i = 0;
        while (i < max_len - 1) {
            char c = Serial::getchar();

            if (c == '\r' || c == '\n') {
                Serial::writeln("");
                buffer[i] = '\0';
                break;
            }

            if (c == '\b' && i > 0) {
                i--;
                Serial::write("\b \b");
                continue;
            }

            if (c >= 32 && c <= 126) {
                buffer[i++] = c;
                Serial::putchar(c);
            }
        }
    }

    void Shell::parse_and_execute(const char* cmd) {
        if (cmd[0] == '\0') return;

        if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o') {
            handle_echo(cmd + 5);
        } else if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p') {
            handle_help();
        } else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r') {
            handle_clear();
        } else {
            Serial::writeln("unknown command");
        }
    }

    void Shell::handle_echo(const char* args) {
        Serial::writeln(args);
    }

    void Shell::handle_help() {
        Serial::writeln("available commands: echo, help, clear");
    }

    void Shell::handle_clear() {
        for (int i = 0; i < 50; i++) {
            Serial::putchar('\n');
        }
    }
}
