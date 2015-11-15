#include <stdlib.h>
#include "param_list.h"
#include "scope.h"
#include "error.h"
#include "list.h"

extern struct node *head;

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

void param_list_resolve(struct param_list *p) {
	if(!p) return;
	struct symbol *s = symbol_create(SYMBOL_PARAM, p->type, p->name);
	if(scope_lookup_local(p->name)) {
		error e;
		e.errorType = ERROR_MULTIPLE_DECLARATION;
		sprintf(e.description, "%s cannot be declared twice in a param list", p->name);
		e.lineNum = -1;
		throw_error(e);
	}
	scope_bind(s->name, s);
	head->params = head->params + 1;
	param_list_resolve(p->next);
}
