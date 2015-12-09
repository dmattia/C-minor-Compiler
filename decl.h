#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "stmt.h"
#include "expr.h"
#include <stdio.h>

struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
	int isEmptyFunction;
};

struct decl * decl_create( char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next, int isEmptyFunction );
void decl_print( struct decl *d, int indent );
void decl_resolve(struct decl*, int quiet);
struct type * decl_typecheck(struct decl*);
void decl_codegen(struct decl*, FILE*);
void decl_global_data_codegen(struct decl*, FILE*);
void decl_global_functions_codegen(struct decl*, FILE*);

#endif
