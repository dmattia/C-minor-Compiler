#include <stdlib.h>
#include "stmt.h"
#include "scope.h"

extern int type_check_errors;

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

void stmt_resolve(struct stmt *s, int quiet) {
	if(!s) return;
	switch(s->kind) {
		case STMT_DECL:
			decl_resolve(s->decl, quiet);
			break;
		case STMT_EXPR:
			expr_resolve(s->expr, quiet);
			break;
		case STMT_IF_ELSE:
			expr_resolve(s->expr, quiet);

			scope_enter(quiet);
			stmt_resolve(s->body, quiet);
			scope_leave(quiet);

			scope_enter(quiet);
			stmt_resolve(s->else_body, quiet);
			scope_leave(quiet);
			break;
		case STMT_FOR:
			expr_resolve(s->init_expr, quiet);
			expr_resolve(s->expr, quiet);
			expr_resolve(s->next_expr, quiet);
			stmt_resolve(s->body, quiet);
			break;
		case STMT_WHILE:
			// No while loops allowed
			break;
		case STMT_PRINT:
			expr_resolve(s->expr, quiet);
			break;
		case STMT_RETURN:
			expr_resolve(s->expr, quiet);
			break;
		case STMT_BLOCK:
			scope_enter(quiet);
			stmt_resolve(s->body, quiet);
			scope_leave(quiet);
			break;
	}
	stmt_resolve(s->next, quiet);
}

struct type *stmt_typecheck(struct stmt *s) {
	if(!s) return 0;
	struct type *result = stmt_typecheck(s->next);

	decl_typecheck(s->decl);
	expr_typecheck(s->init_expr);
	expr_typecheck(s->expr);
	expr_typecheck(s->next_expr);

	struct type *body = stmt_typecheck(s->body);
	if(body) {
		if(!result || body->kind == result->kind) {
			result = body;
		} else {
			printf("You must return the same type consistently within a function\n");
			type_check_errors++;
		}
	}
	struct type *else_part = stmt_typecheck(s->else_body);
	if(else_part) {
		if(!result || else_part->kind == result->kind) {
			result = else_part;
		} else {
			printf("You must return the same type consistently within a function\n");
			type_check_errors++;
		}
	}
	if(else_part && body && else_part->kind != body->kind) {
		printf("You must return the same type consistently within a function\n");
		type_check_errors++;
	}
	if(s->kind == STMT_RETURN) {
		struct type *new_result = expr_typecheck(s->expr);
		if(!result || new_result->kind == result->kind) {
			result = new_result;
			while(result->subtype) result = result->subtype;
		} else {
			printf("You must return the same type consistently within a function\n");
			type_check_errors++;
		}
	} 
	if(s->kind == STMT_PRINT) {
		expr_typecheck(s->expr->left);
	}
	return result;
}

void stmt_codegen( struct stmt *s, FILE *file ) {
	struct expr *e;
	if(!s) return;
	switch(s->kind) {
		case STMT_DECL:
			decl_codegen(s->decl, file);
			break;
		case STMT_EXPR:
			expr_codegen(s->expr, file);
			break;
		case STMT_IF_ELSE:
			break;
		case STMT_FOR:
			break;
		case STMT_WHILE:
			printf("While loops not implemented\n");
			exit(1);
			break;
		case STMT_PRINT:
			fprintf(file, "\n");
			e = s->expr;
			while(e) {
				if(!e->left) break;
				expr_codegen(e->left, file);
				if(e->left->kind == EXPR_STRING) {
					// TODO: Get strings into data so they can be printed
					fprintf(file, "\tMOV %s, %rdi\t\t# Move String in for printing\n", register_name(e->left->reg)); 
				} else if(e->left->kind == EXPR_INT) {
					fprintf(file, "\tMOV $integer_string, %rdi\n");
					fprintf(file, "\tMOV %s, %rsi\n", register_name(e->left->reg));
				} else if(e->left->kind == EXPR_CHAR) {
					fprintf(file, "\tMOV $char_string, %rdi\n");
					fprintf(file, "\tMOV %s, %rsi\n", register_name(e->left->reg));
				} else if(e->left->kind == EXPR_BOOLEAN) {
					fprintf(file, "\tMOV $integer_string, %rdi\n");
					fprintf(file, "\tMOV %s, %rsi\n", register_name(e->left->reg));
				}
				fprintf(file, "\tMOV $0, %rax\t\t# There are no floating point args\n", register_name(e->left->reg)); 
				fprintf(file, "\tPUSHQ %r10\n");
				fprintf(file, "\tPUSHQ %r11\n");
				fprintf(file, "\n\tCALL printf\n\n");
				fprintf(file, "\tPOPQ %r11\n");
				fprintf(file, "\tPOPQ %r10\n\n");
				register_free(e->left->reg);
				e = e->right;
			}
			break;
		case STMT_RETURN:
			expr_codegen(s->expr, file);
			fprintf(file, "\tMOV %s, %rax\t\t# Setup rax for returning\n", register_name(s->expr->reg));
			register_free(s->expr->reg);
			break;
		case STMT_BLOCK:
			/* do nothing */
			break;
	}
	stmt_codegen(s->next, file);
}

int stmt_count_local_variables( struct stmt *s ) {
	if(!s) return 0;
	if(s->kind == STMT_DECL) {
		return 1 + stmt_count_local_variables(s->next);
	} else {
		return 0 + stmt_count_local_variables(s->next);
	}
}
