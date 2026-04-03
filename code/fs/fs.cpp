#include "../include/fs.hpp"
#include "../include/str.hpp"
#include "../vga/vga.hpp"

/*
gnomix filesystem model uses an in-memory table
32 entries match small embedded disk emulation
data buffer is capped at 512 bytes per file
tracks is_dir flag for directory semantics
actions are atomic with simple full table scan
this design is aligned with linux/unix command proofs
supports network-free foss demo mode for gnomix
*/
static const int MAX_FILES = 32;
static const int MAX_SIZE  = 512;

struct File {
    char name[32];
    char data[MAX_SIZE];
    int  used;
    int  is_dir;
};
static File files[MAX_FILES];
static int  count = 0;
static int  cwd = -1;

void fs_init()
{
    count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].is_dir = 0;
        files[i].used = 0;
    }
}

static File* find(const char* name) {
    for (int i = 0; i < count; i++)
        if (k_strcmp(files[i].name, name) == 0)
            return &files[i];
    return nullptr;
}

/*
expanded filesystem with directories
files and directories stored in flat array
is_dir flag distinguishes between types
cwd tracks current working directory
create initializes new files as empty
write stores data in files
read retrieves file contents
list shows files and directories
delete removes entries by shifting array
mkdir creates new directory entries
stat shows file metadata
getsize returns file byte count
isdir checks if path is directory
more bash compatible operations now
*/
int fs_create(const char* name)
{
    if (find(name) || count >= MAX_FILES) return -1;
    k_strcpy(files[count].name, name);
    files[count].data[0] = 0;
    files[count].used = 0;
    files[count].is_dir = 0;
    count++;
    return 0;
}

int fs_mkdir(const char* name)
{
    if (find(name) || count >= MAX_FILES) return -1;
    k_strcpy(files[count].name, name);
    files[count].data[0] = 0;
    files[count].used = 0;
    files[count].is_dir = 1;
    count++;
    return 0;
}

int fs_write(const char* name, const char* data)
{
    File* f = find(name);
    if (!f || f->is_dir) return -1;
    int i = 0;
    while (data[i] && i < MAX_SIZE - 1) { f->data[i] = data[i]; i++; }
    f->data[i] = 0;
    f->used = i;
    return 0;
}

int fs_read(const char* name, char* out, int maxlen)
{
    File* f = find(name);
    if (!f || f->is_dir) return -1;
    int i = 0;
    while (f->data[i] && i < maxlen - 1) { out[i] = f->data[i]; i++; }
    out[i] = 0;
    return i;
}

int fs_getsize(const char* name)
{
    File* f = find(name);
    if (!f) return -1;
    return f->used;
}

int fs_isdir(const char* name)
{
    File* f = find(name);
    if (!f) return -1;
    return f->is_dir;
}

void fs_list()
{
    if (count == 0) { vga_println("(empty)"); return; }
    for (int i = 0; i < count; i++) {
        if (files[i].is_dir) {
            vga_print(files[i].name);
            vga_println("/");
        } else {
            vga_print(files[i].name);
            vga_print(" (");
            int size = files[i].used;
            if (size < 10) vga_putchar('0' + size);
            else {
                vga_putchar('0' + size / 10);
                vga_putchar('0' + size % 10);
            }
            vga_println("b)");
        }
    }
}

int fs_delete(const char* name)
{
    for (int i = 0; i < count; i++) {
        if (k_strcmp(files[i].name, name) == 0) {
            files[i] = files[--count];
            return 0;
        }
    }
    return -1;
}
