#ifndef SCOPE_H
#define SCOPE_H

#include "hash_table.h"
#include "symbol.h"
#include "decl.h"

void scope_enter();
void scope_leave();
int scope_level();
void scope_bind(const char*, struct symbol*);
struct symbol *scope_lookup(const char*);
struct symbol *scope_lookup_local(const char*);

#endif
