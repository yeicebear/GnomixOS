#include "shell.hpp"
#include "../vga/vga.hpp"
#include "../drivers/kb.hpp"
#include "../include/str.hpp"
#include "../include/fs.hpp"

#define MAX_ARG_SLOTS 20
#define MAX_LINE_BUF  256

typedef struct shell_stuff {
    char line[MAX_LINE_BUF];
    int  len;
    int  n_args;
    char* args[MAX_ARG_SLOTS];
} shell_stuff;

static shell_stuff state;

static void clear_state(shell_stuff* s)
{
    s->len = 0;
    s->n_args = 0;
    s->line[0] = 0;
}

static char get_key_blocking(void)
{
    char r;
    do {
        r = keyboard_getchar();
    } while (!r);
    return r;
}

static void shell_readline(shell_stuff* s)
{
    s->len = 0;

    while (1) {
        char x = get_key_blocking();

        if (x == '\n') {
            s->line[s->len] = 0;
            vga_putchar('\n');
            return;
        }

        if ((x == '\b') && (s->len > 0)) {
            s->len -= 1;
            vga_putchar('\b');
            continue;
        }

        if (s->len < MAX_LINE_BUF - 1) {
            s->line[s->len++] = x;
            vga_putchar(x);
        }
    }
}

static void shell_tokenize(shell_stuff* s)
{
    s->n_args = 0;
    int i = 0;

    while ((s->line[i] != 0) && (s->n_args < MAX_ARG_SLOTS)) {
        while (s->line[i] == ' ') {
            i++;
        }
        if (!s->line[i]) {
            break;
        }
        s->args[s->n_args++] = &s->line[i];
        while (s->line[i] && (s->line[i] != ' ')) {
            i++;
        }

        if (s->line[i] == ' ') {
            s->line[i++] = 0;
        }
    }
}

static const char* shell_arg(shell_stuff* s, int idx)
{
    if ((idx < 0) || (idx >= s->n_args)) {
        return "";
    }
    return s->args[idx];
}

static int dispatch_clear(shell_stuff* sh)
{
    (void)sh;
    vga_clear();
    return 0;
}

static int dispatch_echo(shell_stuff* sh)
{
    for (int it = 1; it < sh->n_args; it++) {
        vga_print(shell_arg(sh, it));
        if (it + 1 < sh->n_args) {
            vga_putchar(' ');
        }
    }
    vga_putchar('\n');
    return 0;
}

static int dispatch_touch(shell_stuff* sh)
{
    const char* x = shell_arg(sh, 1);
    if (k_strcmp(x, "") == 0) {
        vga_println("touch: missing argument");
        return -1;
    }
    if (fs_create(x) < 0) {
        vga_println("touch: failed");
        return -1;
    }
    return 0;
}

static int dispatch_ls(shell_stuff* sh)
{
    (void)sh;
    fs_list();
    return 0;
}

static int dispatch_cat(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    if (k_strcmp(p, "") == 0) {
        vga_println("cat: missing argument");
        return -1;
    }
    char buf[512];
    int r = fs_read(p, buf, (int)sizeof(buf));
    if (r < 0) {
        vga_println("cat: not found");
        return -1;
    }
    vga_println(buf);
    return 0;
}

static int dispatch_mkdir(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    if (k_strcmp(p, "") == 0) {
        vga_println("mkdir: missing argument");
        return -1;
    }
    if (fs_mkdir(p) < 0) {
        vga_println("mkdir: failed");
        return -1;
    }
    return 0;
}

static int dispatch_rm(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    if (k_strcmp(p, "") == 0) {
        vga_println("rm: missing argument");
        return -1;
    }
    if (fs_delete(p) < 0) {
        vga_println("rm: not found");
        return -1;
    }
    return 0;
}

static int dispatch_cd(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    if (k_strcmp(p, "") == 0) {
        vga_println("cd: missing argument");
        return -1;
    }
    if (fs_isdir(p) > 0) {
        vga_println("cd: ok");
    } else {
        vga_println("cd: not a directory");
    }
    return 0;
}

static int dispatch_stat(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    if (k_strcmp(p, "") == 0) {
        vga_println("stat: missing argument");
        return -1;
    }
    if (fs_isdir(p) > 0) {
        vga_print(p);
        vga_println(" (directory)");
        return 0;
    }
    int size = fs_getsize(p);
    if (size < 0) {
        vga_println("stat: not found");
        return -1;
    }
    vga_print(p);
    vga_print(" (file, ");
    if (size < 10) {
        vga_putchar('0');
    }
    if (size < 100) {
        vga_putchar('0' + (size / 10) % 10);
    }
    vga_putchar('0' + size % 10);
    vga_println("b)");
    return 0;
}

static int dispatch_cp(shell_stuff* sh)
{
    const char* s = shell_arg(sh, 1);
    const char* d = shell_arg(sh, 2);
    if (k_strcmp(s, "") == 0 || k_strcmp(d, "") == 0) {
        vga_println("cp: missing arguments");
        return -1;
    }
    char buf[512];
    if (fs_read(s, buf, sizeof(buf)) < 0) {
        vga_println("cp: source not found");
        return -1;
    }
    if (fs_create(d) < 0) {
        vga_println("cp: destination exists");
        return -1;
    }
    if (fs_write(d, buf) < 0) {
        vga_println("cp: write failed");
        return -1;
    }
    return 0;
}

static int dispatch_write(shell_stuff* sh)
{
    const char* p = shell_arg(sh, 1);
    const char* d = shell_arg(sh, 2);
    if (k_strcmp(p, "") == 0 || k_strcmp(d, "") == 0) {
        vga_println("write: missing arguments");
        return -1;
    }

    if (fs_isdir(p) > 0) {
        vga_println("write: target is a directory");
        return -1;
    }

    if (fs_getsize(p) < 0) {
        if (fs_create(p) < 0) {
            vga_println("write: failed to create file");
            return -1;
        }
    }

    if (fs_write(p, d) < 0) {
        vga_println("write: failed");
        return -1;
    }

    return 0;
}

static int dispatch_uname(shell_stuff* sh)
{
    (void)sh;
    vga_println("BearNugget 0.1 - bare metal os");
    return 0;
}

static int dispatch_whoami(shell_stuff* sh)
{
    (void)sh;
    vga_println("kernel");
    return 0;
}

static int dispatch_help(shell_stuff* sh)
{
    (void)sh;
    vga_println("available commands:");
    vga_println("  echo <text>");
    vga_println("  cat <file>");
    vga_println("  ls");
    vga_println("  pwd");
    vga_println("  cd <dir>");
    vga_println("  mkdir <dir>");
    vga_println("  touch <file>");
    vga_println("  write <f> <data>");
    vga_println("  rm <file>");
    vga_println("  stat <file>");
    vga_println("  cp <src> <dst>");
    vga_println("  whoami");
    vga_println("  clear");
    vga_println("  uname");
    vga_println("  exit");
    return 0;
}

static int dispatch_exit(shell_stuff* sh)
{
    (void)sh;
    vga_println("goodbye");
    while (true) {
        asm volatile("hlt");
    }
    return 0;
}

typedef struct dispatch_
{
    const char* name;
    int (*fn)(shell_stuff*);
} dispatch;

static dispatch dispatch_table[] =
{
    {"help", dispatch_help},
    {"echo", dispatch_echo},
    {"clear", dispatch_clear},
    {"ls", dispatch_ls},
    {"pwd", dispatch_whoami},
    {"uname", dispatch_uname},
    {"whoami", dispatch_whoami},
    {"cd", dispatch_cd},
    {"mkdir", dispatch_mkdir},
    {"touch", dispatch_touch},
    {"rm", dispatch_rm},
    {"cat", dispatch_cat},
    {"write", dispatch_write},
    {"stat", dispatch_stat},
    {"cp", dispatch_cp},
    {"exit", dispatch_exit},
};

static int run_dispatch(shell_stuff* sh)
{
    if (sh->n_args == 0) {
        return -1;
    }

    const char* cmd = shell_arg(sh, 0);
    for (int i = 0; i < (int)(sizeof(dispatch_table) / sizeof(dispatch_table[0])); i++) {
        if (k_strcmp(cmd, dispatch_table[i].name) == 0) {
            return dispatch_table[i].fn(sh);
        }
    }

    vga_print(cmd);
    vga_println(": command not found");
    return -1;
}

void shell_run()
{
    vga_clear();
    vga_println("BearNugget 0.1");
    vga_println("type 'help' for commands");
    vga_println("(shell starting)");
    vga_putchar('\n');

    while (1) {
        vga_print("bear> ");
        clear_state(&state);
        shell_readline(&state);
        shell_tokenize(&state);
        run_dispatch(&state);
    }
}
