#include "decl.h"
#include "stmt.h"
#include "stdlib.h"
#include "error.h"
#include "scope.h"

extern int type_check_errors;

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
		if(d->type->kind == TYPE_FUNCTION) {
			printf("Cannot declare function %s. Functions must be declared globally\n", d->name);
			exit(1);
		} else {
			s = symbol_create(SYMBOL_LOCAL, d->type, d->name);
		}
	}
	if(scope_lookup_local(d->name)) {
		// variable already exists in this scope
		if(d->type->kind == TYPE_FUNCTION && scope_lookup_local(d->name)) { 
			// a function declaration exists, replace it
			scope_remove_local(d->name);
			s = symbol_create(SYMBOL_GLOBAL, d->type, d->name);
		} else {
			printf("%s cannot be declared twice in the same scope\n", d->name);
			exit(1);
		}
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
			printf("Type Mismatch: %s expects a ", d->name);
			type_print(d->type);
			printf(" but was given a ");
			type_print(expr_result);
			printf("\n");
			type_check_errors++;
		}
	} else if (d->code) {
		// decl is a non-empty function
		struct type *function_return = stmt_typecheck(d->code);
		if(function_return) {
			if(type_equal(d->type->subtype, function_return)) {
				result = type_create(d->type->subtype->kind, 0, 0, 0);
			} else {
				printf("Type Error: Function %s expects a ", d->name);
				type_print(d->type->subtype);
				printf(" but a ");
				type_print(function_return);
				printf(" was returned\n");
				type_check_errors++;
			}
		} else {
			if(d->type->subtype->kind == TYPE_VOID) {
				result = type_create(TYPE_VOID, 0, 0, 0);
			} else {
				printf("Type Error: Function %s expects a ", d->name);
				type_print(d->type->subtype);
				printf(" but no return value was found\n");
				type_check_errors++;
			}
		}
	} else if (d->isEmptyFunction) {
		// decl is an empty function
		if(d->type->subtype->kind == TYPE_VOID) {
			result = type_create(TYPE_VOID, 0, 0, 0);
		} else {
			printf("Type Error: Non void function %s cannot be empty\n", d->name);
			type_check_errors++;
		}
	} else {
		// decl is of type:
		// a : integer; 
		if (d->type->kind == TYPE_ARRAY) {
			if (d->type->expr) {
				if(d->type->expr->kind != EXPR_INT) {
					printf("Type Error: Declaration of array %s must have a fixed size\n", d->name);
					type_check_errors++;
				}
			} else {
				printf("Declaration of array %s must have a fixed size\n", d->name);
				type_check_errors++;
			}
		}
		result = type_create(d->type->kind, 0, 0, 0);
	}
	decl_typecheck(d->next);
	return result;
}

void decl_codegen(struct decl *d, FILE *file) {
	if(!d) return;
	switch(d->type->kind) {
		case TYPE_BOOLEAN:
			break;
		case TYPE_CHARACTER:
			break;
		case TYPE_INTEGER:
			fprintf(file, "%s:\n", d->name);
			if(d->value) {
				fprintf(file, "\t.quad %d\n", d->value->literal_value);
			}
			break;
		case TYPE_STRING:
			fprintf(file, "%s:\n", d->name);
			if(d->value) {
				fprintf(file, "\t.string %s\n", d->value->string_literal);
			}
			break;
		case TYPE_ARRAY:
			printf("No array support\n");
			exit(1);
		case TYPE_FUNCTION:
			fprintf(file, "\n.global %s\n.type %s, @function\n%s:\n", d->name, d->name, d->name);
			break;
		case TYPE_VOID:
			break;
	}
	decl_codegen(d->next, file);
	/*
	fprintf(file, ".data\n");
	fprintf(file, ".text\n");
	fprintf(file, ".global main\n");
	fprintf(file, "main:\n");
	*/
}
