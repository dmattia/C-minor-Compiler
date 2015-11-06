#include "param_list.h"
#include <stdlib.h>

struct param_list * param_list_create( char *name, struct type *type, struct param_list *next) {
	struct param_list *p;
	p = (struct param_list*)malloc(sizeof(*p));
	p->name = name;
	p->type = type;
	p->symbol = 0;
	p->next = next;
	return p;
}

void param_list_print( struct param_list *a ) {
	if(!a) return;
	printf("%s: ", a->name);
	type_print(a->type);
	if(a->next) {
		printf(", ");
		param_list_print(a->next);
	}
}
