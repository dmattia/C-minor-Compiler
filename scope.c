#include "scope.h"
#include "list.h"
#include "stdio.h"
#include "error.h"

void scope_enter() {
	struct hash_table *h;	
	h = hash_table_create(0,0);
	push_front(h);
	printf("Entered new scope. New Level: %d\n", scope_level());
}

void scope_leave() {
	pop_front();
	printf("Leaving scope. New Level: %d\n", scope_level());
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

void scope_bind(const char *name, struct symbol *sym) {
	if(head) {
		hash_table_insert(head->hash_table, name, sym);
	}
	printf("%s resolves to something\n", name);
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
