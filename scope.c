#include "scope.h"
#include "list.h"
#include "stdio.h"
#include "error.h"

void scope_enter(int quiet) {
	struct hash_table *h;	
	h = hash_table_create(0,0);
	push_front(h);
	if(!quiet) printf("Entered new scope. New Level: %d\n", scope_level());
}

void scope_leave(int quiet) {
	pop_front();
	if(!quiet) printf("Leaving scope. New Level: %d\n", scope_level());
}

int scope_level() {
	int level = 0;
	struct node* n = head;
	while(n) {
		++level;
		n = n->next;
	}
	return level;
}

void scope_bind(const char *name, struct symbol *sym, int quiet) {
	if(head) {
		hash_table_insert(head->hash_table, name, sym);
	}
	switch(sym->kind) {
		case SYMBOL_LOCAL:
			if(!quiet) printf("%s resolves to local %d\n", name, hash_table_size(head->hash_table) - head->params);
			break;
		case SYMBOL_PARAM:
			if(!quiet) printf("%s resolves to param %d\n", name, hash_table_size(head->hash_table));
			break;
		case SYMBOL_GLOBAL:
			if(!quiet) printf("%s resolves to global %s\n", name, name);
			break;
	}
}

struct symbol *scope_lookup(const char *name) {
	struct node *n = head;
	struct symbol *s;
	while(n) {
		if( (s = hash_table_lookup(n->hash_table, name)) ) {
			return s;
		}
		n = n->next;
	}
	return 0;
}

struct symbol *scope_lookup_local(const char *name) {
	if(!head) return 0;
	return hash_table_lookup(head->hash_table, name);
}
