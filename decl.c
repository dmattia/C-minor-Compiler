#include "decl.h"
#include "stmt.h"
#include "stdlib.h"
#include "error.h"
#include "scope.h"

struct decl * decl_create( char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next, int isEmptyFunction ) {
	struct decl * d;
	d = (struct decl *)malloc(sizeof(*d));
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->next = next;
	d->isEmptyFunction = isEmptyFunction;
	return d;
}

void decl_print( struct decl *d, int indent ) {
	int i = indent;
	if(!d) return;
	for(; i>0; --i) {
		printf("\t");
	}
	printf("%s : ", d->name);
	type_print(d->type);
	if(d->code) {
		printf(" = {\n");
		stmt_print(d->code, indent+1);
		printf("\n}\n");
	} else if (d->value) {
		printf(" = ");
		expr_print(d->value);
		printf(";\n");
	} else if(d->isEmptyFunction) {
		printf(" = { }\n");
	} else {
		printf(";\n");
	}
	decl_print(d->next, indent);
}

void decl_resolve(struct decl *d, int quiet) {
	if(!d) return;
	struct symbol *s;
	if(scope_level() == 1) {
		s = symbol_create(SYMBOL_GLOBAL, d->type, d->name);
	} else {
		s = symbol_create(SYMBOL_LOCAL, d->type, d->name);
	}
	if(scope_lookup_local(d->name)) {
		// variable already exists in this scope
		error e;
		e.errorType = ERROR_MULTIPLE_DECLARATION;
		sprintf(e.description, "%s cannot be declared twice in the same scope", d->name);
		e.lineNum = -1;
		throw_error(e);
	}
	scope_bind(s->name, s, quiet);
	expr_resolve(d->value, quiet);
	if(d->code) {
		scope_enter(quiet);
		param_list_resolve(d->type->params, quiet);
		stmt_resolve(d->code, quiet);
		scope_leave(quiet);
	}
	decl_resolve(d->next, quiet);
}

struct type *decl_typecheck(struct decl *d) {
	if(!d) return type_create(TYPE_VOID, 0, 0, 0);
	struct type *result;
	if (d->value) {
		// decl is of type:
		// a : integer = 5;
		struct type *expr_result = expr_typecheck(d->value);
		if(d->type->kind == expr_result->kind) {
			result = type_create(d->type->kind, 0, 0, 0);
		} else {
			printf("Type mismatch in the declaration:\n");
			decl_print(d, 0);
			printf("\n");
			exit(1);
		}
	} else if (d->code) {
		// decl is a non-empty function
		struct type *function_return = stmt_typecheck(d->code);
		if(d->type->subtype->kind == function_return->kind) {
			result = type_create(d->type->subtype->kind, 0, 0, 0);
		} else {
			printf("Type mismatch in the declaration:\n");
			decl_print(d, 0);
			printf("\n");
			exit(1);
		}
	} else if (d->isEmptyFunction) {
		// decl is an empty function
		if(d->type->subtype->kind == TYPE_VOID) {
			result = type_create(TYPE_VOID, 0, 0, 0);
		} else {
			printf("Non void funcion %s cannot be empty\n", d->name);
			exit(1);
		}
	} else {
		// decl is of type:
		// a : integer; 
		result = type_create(d->type->kind, 0, 0, 0);
	}
	decl_typecheck(d->next);
	return result;
}
