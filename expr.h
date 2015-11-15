#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
	EXPR_LIST,
	EXPR_ASSIGNMENT,
	EXPR_OR,
	EXPR_AND,
	EXPR_LT,
	EXPR_GT,
	EXPR_LE,
	EXPR_GE,
	EXPR_NOT_EQUALS,
	EXPR_EQUALS,
	EXPR_ADD,
	EXPR_MINUS,
	EXPR_TIMES,
	EXPR_DIVIDES,
	EXPR_MOD,
	EXPR_POWER,
	EXPR_NEGATIVE,
	EXPR_NOT,
	EXPR_PRE_INCREMENT,
	EXPR_PRE_DECREMENT,
	EXPR_POST_INCREMENT,
	EXPR_POST_DECREMENT,
	EXPR_FUNCTION,
	EXPR_BOOLEAN,
	EXPR_INT,
	EXPR_CHAR,
	EXPR_STRING,
	EXPR_NAME,
	EXPR_ARRAY
} expr_t;

struct expr {
	/* used by all expr types */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by leaf expr types */
	const char *name;
	struct symbol *symbol;
	int literal_value;
	const char * string_literal;
};

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

struct expr * expr_create_name( const char *n );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_character_literal( int c );
struct expr * expr_create_string_literal( const char *str );

void expr_print( struct expr *e );
void expr_resolve( struct expr *e );

#endif
