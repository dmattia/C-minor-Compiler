#include "decl.h"
#include "stdlib.h"

struct decl * decl_create( char *name, struct type *t, struct expr *v, struct stmt *c, struct decl *next ) {
	struct decl * d;
	d = (struct decl *)malloc(sizeof(*d));
	d->name = name;
	d->type = t;
	d->value = v;
	d->code = c;
	d->next = next;
	return d;
}

void decl_print( struct decl *d, int indent ) {
	if(!d) return;
	for(; indent>0; --indent) {
		printf("\t");
	}
	pretty_print(d);
	type_print(d->type);
	expr_print(d->value);
	stmt_print(d->code, indent+1);
	decl_print(d->next, indent);
	printf("\n");
}

void pretty_print( struct decl *d ) {
	if(!d) return;
	printf("%s : ", d->name);
}
