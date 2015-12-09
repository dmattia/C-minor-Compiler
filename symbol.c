#include "symbol.h"
#include "stdlib.h"

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name ) {
	struct symbol *s;
	s = (struct symbol*)malloc(sizeof(struct symbol));
	s->kind = kind;
	s->type = type;
	s->name = name;
	return s;
}

// Returns the register value for a symbol
char* symbol_code( struct symbol *s) {
	return "SYM_CODE_NOT_DONE_YET";
}
