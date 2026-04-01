#pragma once

namespace bear {
    class Shell {
    public:
        void init();
        void run();

    private:
        void print_prompt();
        void read_line(char* buffer, int max_len);
        void parse_and_execute(const char* cmd);
        void handle_echo(const char* args);
        void handle_help();
        void handle_clear();
    };
}
