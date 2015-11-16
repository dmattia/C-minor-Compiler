#include "expr.h"
#include "stdlib.h"
#include "scope.h"
#include "symbol.h"
#include "error.h"

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = kind;
	e->left = left;
	e->right = right;
	return e;
}

struct expr * expr_create_name( const char *n ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = EXPR_NAME;
	e->left = e->right = 0;
	e->name = n;
	return e;
}

struct expr * expr_create_boolean_literal( int c ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = EXPR_BOOLEAN;
	e->left = e->right = 0;
	e->literal_value = c;
	return e;
}

struct expr * expr_create_integer_literal( int c ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = EXPR_INT;
	e->left = e->right = 0;
	e->literal_value = c;
	return e;
}

struct expr * expr_create_character_literal( int c) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = EXPR_CHAR;
	e->left = e->right = 0;
	e->literal_value = c;
	return e;
}

struct expr * expr_create_string_literal( const char *str ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = EXPR_STRING;
	e->left = e->right = 0;
	e->string_literal = str;
	return e;
}

void expr_print( struct expr *e ) {
	if(!e) return;
	if(e->kind != EXPR_LIST && e->kind != EXPR_FUNCTION && e->kind != EXPR_ARRAY && e->kind != EXPR_NAME && e->kind != EXPR_ASSIGNMENT) printf("(");
	expr_print(e->left);
	switch(e->kind) {
		case EXPR_LIST:
			printf(", ");
			break;
		case EXPR_ASSIGNMENT:
			printf(" = ");
			break;
		case EXPR_OR:
			printf(" || ");
			break;
		case EXPR_AND:
			printf(" && ");
			break;
		case EXPR_LT:
			printf(" < ");
			break;
		case EXPR_GT:
			printf(" > ");
			break;
		case EXPR_LE:
			printf(" <= ");
			break;
		case EXPR_GE:
			printf(" >= ");
			break;
		case EXPR_NOT_EQUALS:
			printf(" != ");
			break;
		case EXPR_EQUALS:
			printf(" == ");
			break;
		case EXPR_ADD:
			printf("+");
			break;
		case EXPR_MINUS:
			printf("-");
			break;
		case EXPR_TIMES:
			printf("*");
			break;
		case EXPR_DIVIDES:
			printf("/");
			break;
		case EXPR_MOD:
			printf("%c", '%');
			break;
		case EXPR_POWER:
			printf("^");
			break;
		case EXPR_NEGATIVE:
			printf("-");
			break;
		case EXPR_NOT:
			printf("!");
			break;
		case EXPR_PRE_INCREMENT:
			printf("++");
			break;
		case EXPR_PRE_DECREMENT:
			printf("--");
			break;
		case EXPR_POST_INCREMENT:
			printf("++");
			break;
		case EXPR_POST_DECREMENT:
			printf("--");
			break;
		case EXPR_FUNCTION:
			printf("(");
			break;
		case EXPR_BOOLEAN:
			/* fall through */
		case EXPR_INT:
			printf("%d", e->literal_value);
			break;
		case EXPR_CHAR:
			printf("'%c'", e->literal_value);
			break;
		case EXPR_STRING:
			printf("%s", e->string_literal);
			break;
		case EXPR_NAME:
			printf("%s", e->name);
			break;
		case EXPR_ARRAY:
			printf("[");
			break;
	}
	expr_print(e->right);
	if(e->kind != EXPR_LIST && e->kind != EXPR_FUNCTION && e->kind != EXPR_ARRAY && e->kind != EXPR_NAME && e->kind != EXPR_ASSIGNMENT) printf(")");
	if(e->kind == EXPR_FUNCTION) printf(")");
	if(e->kind == EXPR_ARRAY) printf("]");
}

void expr_resolve (struct expr *e) {
	if(!e) return;
	expr_resolve(e->left);
	expr_resolve(e->right);
	if(e->kind == EXPR_NAME) {
		struct symbol *s = scope_lookup(e->name);
		if(s) {
			e->symbol = s;
			printf("Found use of symbol %s\n", e->name);
		} else {
			error err;
			err.errorType = ERROR_UNFOUND_IDENTIFIER;
			sprintf(err.description, "%s has not been declared", e->name);
			err.lineNum = -1;
			throw_error(err);
		}
	}
}

struct type *expr_typecheck(struct expr *e) {
	if(!e) return type_create(TYPE_VOID, 0, 0, 0);
	struct type *left;
	struct type *right;
	switch(e->kind) {
		case EXPR_LIST:
			return type_create(TYPE_STRING, 0, 0, 0); // Could be any non void type
			break;
		case EXPR_ASSIGNMENT:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(type_equal(left, right) && left->kind != TYPE_FUNCTION) {
				return type_copy(left);
			} else {
				printf("Cannot assign ");
				type_print(right);
				printf(" to ");
				type_print(left);
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_NOT_EQUALS:
		case EXPR_EQUALS:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(type_equal(left, right) && left->kind != TYPE_FUNCTION && left->kind != TYPE_ARRAY) {
				return type_copy(left);
			} else {
				printf("Cannot perform logical equals operation on ");
				type_print(left);
				printf(" and ");
				type_print(right);
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
		case EXPR_ADD:
		case EXPR_MINUS:
		case EXPR_TIMES:
		case EXPR_DIVIDES:
		case EXPR_MOD:
		case EXPR_POWER:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(left->kind == TYPE_INTEGER && right->kind == TYPE_INTEGER) {
				return type_create(TYPE_INTEGER, 0, 0, 0);
			} else {
				printf("Cannot perform arithmetic operations on ");
				type_print(left);
				printf(" and ");
				type_print(right);
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_NEGATIVE:
			right = expr_typecheck(e->right);
			if(right->kind == TYPE_INTEGER) {
				return type_create(TYPE_INTEGER, 0, 0, 0);
			} else {
				printf("Cannot take the negative of ");
				type_print(right);	
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_OR:
		case EXPR_AND:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(left->kind == TYPE_BOOLEAN && right->kind == TYPE_BOOLEAN) {
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			} else {
				printf("Cannot perform logical operations on ");
				type_print(left);
				printf(" and ");
				type_print(right);
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_NOT:
			right = expr_typecheck(e->right);
			if(right->kind == TYPE_BOOLEAN) {
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			} else {
				printf("Cannot perform a logical not on ");
				type_print(right);	
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_PRE_INCREMENT:
		case EXPR_PRE_DECREMENT:
			right = expr_typecheck(e->right);
			if(right->kind == TYPE_INTEGER) {
				return type_create(TYPE_INTEGER, 0, 0, 0);
			} else {
				printf("Cannot perform integer operations on ");
				type_print(right);	
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_POST_INCREMENT:
		case EXPR_POST_DECREMENT:
			left = expr_typecheck(e->left);
			if(left->kind == TYPE_INTEGER) {
				return type_create(TYPE_INTEGER, 0, 0, 0);
			} else {
				printf("Cannot perform integer operations on ");
				type_print(left);	
				printf("\n");
				exit(1);
			}
			break;
		case EXPR_FUNCTION:
			return scope_lookup(e->name)->type;
			break;
		case EXPR_BOOLEAN:
			return type_create(TYPE_BOOLEAN, 0, 0, 0);
			break;
		case EXPR_INT:
			return type_create(TYPE_INTEGER, 0, 0, 0);
			break;
		case EXPR_CHAR:
			return type_create(TYPE_CHARACTER, 0, 0, 0);
			break;
		case EXPR_STRING:
			return type_create(TYPE_STRING, 0, 0, 0);
			break;
		case EXPR_NAME:
			return e->symbol->type;
			break;
		case EXPR_ARRAY:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(right->kind == TYPE_INTEGER) {
				return type_copy(left);
			} else {
				printf("Can not use ");
				type_print(right);
				printf(" as an array index. Must use an integer");
				exit(1);
			}
			break;
	}
}
