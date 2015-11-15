#include "decl.h"
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

void decl_resolve(struct decl *d) {
	if(!d) return;
	struct symbol *s = symbol_create(SYMBOL_LOCAL, d->type, d->name);
	if(scope_lookup_local(d->name)) {
		// variable already exists in this scope
		error e;
		e.errorType = ERROR_MULTIPLE_DECLARATION;
		sprintf(e.description, "%s cannot be declared twice in the same scope", d->name);
		e.lineNum = -1;
		throw_error(e);
	}
	scope_bind(s->name, s);
	expr_resolve(d->value);
	if(d->code) {
		scope_enter();
		param_list_resolve(d->type->params);
		stmt_resolve(d->code);
		scope_leave();
	}
	decl_resolve(d->next);
}
