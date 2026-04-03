#pragma once

void fs_init();
int  fs_create(const char* name);
int  fs_mkdir(const char* name);
int  fs_write(const char* name, const char* data);
int  fs_read(const char* name, char* out, int maxlen);
void fs_list();
int  fs_delete(const char* name);
int  fs_getsize(const char* name);
int  fs_isdir(const char* name);
