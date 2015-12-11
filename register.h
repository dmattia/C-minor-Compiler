#ifndef REGS_H
#define REGS_H

static int regs[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const char* register_name(int r);
int register_alloc();
void register_free(int r);
char* num_to_arg(int);
void free_all();

#endif
