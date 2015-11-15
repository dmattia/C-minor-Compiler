#include <stdlib.h>
#include "stmt.h"
#include "scope.h"

struct stmt * stmt_create( stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body ) {
	struct stmt *s;
	s = (struct stmt*)malloc(sizeof(*s));	
	s->kind = kind;
	s->decl = d;
	s->init_expr = init_expr;
	s->expr = e;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->next = 0;
	return s;
}

void stmt_print( struct stmt * s, int indent ) {
	int i = indent;
	if(!s) return;
	switch(s->kind) {
		case STMT_DECL:
			decl_print(s->decl, indent);
			break;
		case STMT_EXPR:
			for(; i > 0; --i) printf("\t");
			expr_print(s->expr);
			printf(";");
			if(s->next) printf("\n");
			break;
		case STMT_IF_ELSE:
			for(; i > 0; --i) printf("\t");
			printf("if (");
			expr_print(s->expr);
			printf(") ");
			stmt_print(s->body, indent);
			if(s->else_body) {
				printf(" else ");
				stmt_print(s->else_body, indent);
			}
			break;
		case STMT_FOR:
			for(; i > 0; --i) printf("\t");
			printf("for (");
			expr_print(s->init_expr);
			printf("; ");
			expr_print(s->expr);
			printf("; ");
			expr_print(s->next_expr);
			printf(") ");
			stmt_print(s->body, indent);
			printf("\n");
			break;
		case STMT_WHILE:
			printf("NO WHILE LOOPS MAN.");
			break;
		case STMT_PRINT:
			for(; i > 0; --i) printf("\t");
			printf("print ");
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_RETURN:
			for(; i > 0; --i) printf("\t");
			printf("return ");
			expr_print(s->expr);
			printf(";");
			break;
		case STMT_BLOCK:
			printf("{\n");
			stmt_print(s->body, indent + 1);
			printf("\n");
			for(i=0; i < indent; ++i) printf("\t");
			printf("}");
			break;
	}
	stmt_print(s->next, indent);
}

void stmt_resolve(struct stmt *s) {
	if(!s) return;
	switch(s->kind) {
		case STMT_DECL:
			decl_resolve(s->decl);
			break;
		case STMT_EXPR:
			expr_resolve(s->expr);
			break;
		case STMT_IF_ELSE:
			expr_resolve(s->expr);
			stmt_resolve(s->body);
			stmt_resolve(s->else_body);
			break;
		case STMT_FOR:
			expr_resolve(s->init_expr);
			expr_resolve(s->expr);
			expr_resolve(s->next_expr);
			stmt_resolve(s->body);
			break;
		case STMT_WHILE:
			// No while loops allowed
			break;
		case STMT_PRINT:
			expr_resolve(s->expr);
			break;
		case STMT_RETURN:
			expr_resolve(s->expr);
			break;
		case STMT_BLOCK:
			scope_enter();
			stmt_resolve(s->body);
			scope_leave();
			break;
	}
	stmt_resolve(s->next);
}
