#include "symbol.h"
#include "stdlib.h"

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name ) {
	struct symbol *s;
	s = (struct symbol*)malloc(sizeof(struct symbol));
	s->kind = kind;
	s->type = type;
	s->name = name;
	s->which = -1;
	return s;
}

// Returns the register value for a symbol
char* symbol_code( struct symbol *s ) {
	int stack_diff;
	char *result = (char*)malloc(11);
	if(!s) {
		printf("No symbol given\n");
		exit(1);
	}
	stack_diff = 8 * s->which;
	sprintf(result, "-%d(%rbp)", stack_diff);
	return result;
}
