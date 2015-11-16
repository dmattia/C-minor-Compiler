#include "type.h"
#include <stdlib.h>

struct type * type_create( type_kind_t kind, struct expr *e, struct param_list *params, struct type *subtype ) {
	struct type *t;
	t = (struct type*)malloc(sizeof(*t));
	t->kind = kind;
	t->expr = e;
	t->params = params;
	t->subtype = subtype;
	return t;
}

void type_print( struct type *t) {
	switch(t->kind) {
		case TYPE_BOOLEAN:
			printf("boolean");
			break;
		case TYPE_CHARACTER:
			printf("char");
			break;
		case TYPE_INTEGER:
			printf("integer");
			break;
		case TYPE_STRING:
			printf("string");
			break;
		case TYPE_ARRAY:
			printf("array [");
			expr_print(t->expr);
			printf("] ");
			type_print(t->subtype);
			break;
		case TYPE_FUNCTION:
			printf("function ");
			type_print(t->subtype);
			printf(" (");
			param_list_print(t->params);
			printf(")");
			break;
		case TYPE_VOID:
			printf("void");
			break;
	}
}

struct type * type_copy( struct type *t ) {
	struct type *new = type_create(t->kind, t->expr, t->params, t->subtype);
	return new;
}

int type_equal(struct type *t1, struct type *t2) {
	return t1->kind == t2->kind;
}
