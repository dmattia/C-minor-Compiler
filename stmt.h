#ifndef STMT_H
#define STMT_H

#include "decl.h"

typedef enum {
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_WHILE,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK
} stmt_kind_t;

struct stmt {
	stmt_kind_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
};

struct stmt * stmt_create( stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body );
void stmt_print( struct stmt *s, int indent );
void stmt_resolve( struct stmt *s, int quiet );
struct type * stmt_typecheck( struct stmt *s );
void stmt_codegen( struct stmt*, FILE* );
int stmt_count_local_variables( struct stmt* );

#endif
