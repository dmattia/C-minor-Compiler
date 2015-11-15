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

struct expr * expr_create_name( const char *n) {
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
