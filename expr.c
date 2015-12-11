#include "expr.h"
#include "stdlib.h"
#include "scope.h"
#include "symbol.h"
#include "error.h"
#include "register.h"
#include "string_list.h"

extern int type_check_errors;
extern struct string_node *string_head;
extern int labelNum;

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right ) {
	struct expr *e;
	e = (struct expr*)malloc(sizeof(*e));
	e->kind = kind;
	e->left = left;
	e->right = right;
	e->reg = -1;
	e->string_literal = 0;
	e->name = 0;
	e->literal_value = 0;
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
		case EXPR_ARRAY_LITERAL:
			break;
	}
	expr_print(e->right);
	if(e->kind != EXPR_LIST && e->kind != EXPR_FUNCTION && e->kind != EXPR_ARRAY && e->kind != EXPR_NAME && e->kind != EXPR_ASSIGNMENT) printf(")");
	if(e->kind == EXPR_FUNCTION) printf(")");
	if(e->kind == EXPR_ARRAY) printf("]");
}

void expr_resolve (struct expr *e, int quiet) {
	if(!e) return;
	expr_resolve(e->left, quiet);
	expr_resolve(e->right, quiet);
	if(e->kind == EXPR_NAME) {
		struct symbol *s = scope_lookup(e->name);
		if(s) {
			e->symbol = s;
			if(!quiet) printf("Found use of symbol %s\n", e->name);
		} else {
			if(!quiet) printf("Unfound identifier: %s\n", e->name);
			exit(1);
		}
	} else if(e->kind == EXPR_STRING) {
		push_string_front(e->string_literal);
	}
}

struct type *expr_typecheck(struct expr *e) {
	if(!e) return type_create(TYPE_VOID, 0, 0, 0);
	struct param_list *param_ptr;
	struct expr *expr_ptr;
	struct type *left;
	struct type *right;
	switch(e->kind) {
		case EXPR_LIST:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			return type_create(TYPE_VOID, 0, 0, 0);
			break;
		case EXPR_ASSIGNMENT:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			while(left->kind == TYPE_ARRAY) left = left->subtype; 
			while(right->kind == TYPE_ARRAY) left = left->subtype; 
			if(type_equal(left, right) && left->kind != TYPE_FUNCTION) {
				return type_copy(left);
			} else {
				printf("Cannot assign ");
				type_print(right);
				printf(" to ");
				type_print(left);
				if(e->left->name) {
					printf(" %s\n", e->left->name);
				} else {
					printf("\n");
				}
				type_check_errors++;
				return left;
			}
			break;
		case EXPR_NOT_EQUALS:
		case EXPR_EQUALS:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(type_equal(left, right) && left->kind != TYPE_FUNCTION && left->kind != TYPE_ARRAY) {
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			} else {
				printf("Cannot perform logical equals operation on ");
				type_print(left);
				printf(" and ");
				type_print(right);
				printf("\n");
				type_check_errors++;
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			}
			break;
		case EXPR_LT:
		case EXPR_GT:
		case EXPR_LE:
		case EXPR_GE:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(left->kind == TYPE_INTEGER && right->kind == TYPE_INTEGER) {
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			} else {
				printf("Cannot perform boolean operations on ");
				type_print(left);
				printf(" and ");
				type_print(right);
				printf("\n");
				type_check_errors++;
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
			}
			break;
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
				type_check_errors++;
				return type_create(TYPE_INTEGER, 0, 0, 0);
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
				type_check_errors++;
				return type_create(TYPE_INTEGER, 0, 0, 0);
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
				type_check_errors++;
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
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
				type_check_errors++;
				return type_create(TYPE_BOOLEAN, 0, 0, 0);
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
				type_check_errors++;
				return type_create(TYPE_INTEGER, 0, 0, 0);
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
				type_check_errors++;
				return type_create(TYPE_INTEGER, 0, 0, 0);
			}
			break;
		case EXPR_FUNCTION:
			param_ptr = e->left->symbol->type->params;
			expr_ptr = e->right;
			while(param_ptr) {
				if(!expr_ptr) {
					printf("Not enough arguments given for function %s\n", e->left->name);
					type_check_errors++;
					break;
				}
				if(!type_equal(param_ptr->type, expr_typecheck(expr_ptr->left))) {
					printf("Function %s requires a paramater of type ", e->left->name);
					type_print(param_ptr->type);
					printf(" but ");
					expr_print(expr_ptr->left);
					printf(" is of type ");
					type_print(expr_typecheck(expr_ptr->left));
					printf("\n");
					type_check_errors++;
					break;
				}
				param_ptr = param_ptr->next;
				expr_ptr = expr_ptr->right;
			}
			if(expr_ptr) {
				printf("Too many arguments given for function %s\n", e->left->name);
				type_check_errors++;
			}
			return e->left->symbol->type->subtype;
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
			if (e->symbol) return e->symbol->type;
			return type_create(TYPE_VOID, 0, 0, 0);
			break;
		case EXPR_ARRAY:
			left = expr_typecheck(e->left);
			right = expr_typecheck(e->right);
			if(right->kind == TYPE_INTEGER) {
				return type_create(left->subtype->kind, 0, 0, 0);
			} else {
				printf("Cannot use ");
				type_print(right);
				printf(" as an array index. Must use an integer\n");
				type_check_errors++;
				return type_create(left->subtype->kind, 0, 0, 0);
			}
			break;
		case EXPR_ARRAY_LITERAL:
			left = expr_typecheck(e->right->left);
			return type_create(TYPE_ARRAY, 0, 0, left->subtype);
			break;
	}
	return type_create(TYPE_VOID, 0, 0, 0);
}

void expr_codegen(struct expr *e, FILE *file) {
	int string_count = 0, found = 0;
	struct string_node *sn;
	char* string_label = (char*)malloc(9);
	char* label_name = (char*)malloc(9);
	if(!e) return;
	switch(e->kind) {
		case EXPR_LIST:
			break;
		case EXPR_ASSIGNMENT:
			expr_codegen(e->right, file);
			fprintf(file, "\tMOV %s, %s\t\t# Assignment\n", register_name(e->right->reg), symbol_code(e->left->symbol));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_OR:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tOR %s, %s\n", register_name(e->left->reg), register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_AND:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tAND %s, %s\n", register_name(e->left->reg), register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_LT:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJG %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_GT:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJL %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_LE:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJGE %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_GE:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJLE %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_NOT_EQUALS:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJNE %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_EQUALS:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tCMP %s, %s\t\t\n", register_name(e->left->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV $1, %s\n", register_name(e->right->reg));
			sprintf(label_name, "done_%d", labelNum);
			fprintf(file, "\tJE %s\n", label_name);
			fprintf(file, "\tMOV $0, %s\n", register_name(e->right->reg));
			fprintf(file, "%s:\n", label_name);
			e->reg = e->right->reg;
			register_free(e->left->reg);
			labelNum++;
			break;
		case EXPR_ADD:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tADD %s, %s\n", register_name(e->left->reg), register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_MINUS:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tSUB %s, %s\n", register_name(e->right->reg), register_name(e->left->reg));
			e->reg = e->left->reg;
			register_free(e->right->reg);
			break;
		case EXPR_TIMES:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tMOV %s, %rax\t\t#Move left value into rax to prepare for multiplying\n", register_name(e->left->reg));
			fprintf(file, "\tIMUL %s\t\t# Multiply %rax by the value in %s\n", register_name(e->right->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV %rax, %s\t\t# Move multiplied result back to non-scratch register\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_DIVIDES:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tMOV %s, %rax\t\t#Move left value into rax to prepare for dividing\n", register_name(e->left->reg));
			fprintf(file, "\tCDQ\n");
			fprintf(file, "\tIDIV %s\t\t\t# Divide %rax by the value in %s\n", register_name(e->right->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV %rax, %s\t\t# Move divided result back to non-scratch register\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_MOD:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tMOV %s, %rax\t\t#Move left value into rax to prepare for dividing\n", register_name(e->left->reg));
			fprintf(file, "\tCDQ\n");
			fprintf(file, "\tIDIV %s\t\t\t# Divide %rax by the value in %s\n", register_name(e->right->reg), register_name(e->right->reg));
			fprintf(file, "\tMOV %rdx, %s\t\t# Move remainder back to non-scratch register\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_POWER:
			expr_codegen(e->left, file);
			expr_codegen(e->right, file);
			fprintf(file, "\tMOV %s, %rdi\t\t# Move first argument for base of power\n", register_name(e->left->reg));
			fprintf(file, "\tMOV %s, %rsi\t\t# Move second argument for exponent\n", register_name(e->right->reg));
			fprintf(file, "\n\tPUSHQ %r10\n");
			fprintf(file, "\tPUSHQ %r11\n");
			fprintf(file, "\n\tCALL integer_power\n\n");
			fprintf(file, "\tPOPQ %r11\n");
			fprintf(file, "\tPOPQ %r10\n");
			fprintf(file, "\tMOV %rax, %s\t\t# Move result of integer_power into register\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			register_free(e->left->reg);
			break;
		case EXPR_NEGATIVE:
			expr_codegen(e->right, file);
			fprintf(file, "\tNEG %s\t\t# Negate the value\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			break;
		case EXPR_NOT:
			expr_codegen(e->right, file);
			fprintf(file, "\tSUB $1, %s\t\t# Negate the boolean\n", register_name(e->right->reg));
			e->reg = e->right->reg;
			break;
		case EXPR_PRE_INCREMENT:
			expr_codegen(e->right, file);
			fprintf(file, "\tINC %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV %s, %s\n", register_name(e->right->reg), symbol_code(e->right->symbol));
			e->reg = e->right->reg;
			break;
		case EXPR_PRE_DECREMENT:
			expr_codegen(e->right, file);
			fprintf(file, "\tDEC %s\n", register_name(e->right->reg));
			fprintf(file, "\tMOV %s, %s\n", register_name(e->right->reg), symbol_code(e->right->symbol));
			e->reg = e->right->reg;
			break;
		case EXPR_POST_INCREMENT:
			// TODO: Delay incrementing
			expr_codegen(e->left, file);
			fprintf(file, "\tINC %s\n", register_name(e->left->reg));
			fprintf(file, "\tMOV %s, %s\n", register_name(e->left->reg), symbol_code(e->left->symbol));
			e->reg = e->left->reg;
			break;
		case EXPR_POST_DECREMENT:
			expr_codegen(e->left, file);
			fprintf(file, "\tDEC %s\n", register_name(e->left->reg));
			fprintf(file, "\tMOV %s, %s\n", register_name(e->left->reg), symbol_code(e->left->symbol));
			e->reg = e->left->reg;
			break;
		case EXPR_FUNCTION:
			fprintf(file, "\n\tPUSH %r10\n");
			fprintf(file, "\tPUSH %r11\n");
			fprintf(file, "\n\tCALL %s\n\n", e->left->name);
			fprintf(file, "\tPOP %r11\n");
			fprintf(file, "\tPOP %r10\n");
			e->reg = 0;
			break;
		case EXPR_BOOLEAN:
			e->reg = register_alloc();			
			fprintf(file, "\tMOV $%d,  %s\n", e->literal_value, register_name(e->reg));
			break;
		case EXPR_INT:
			e->reg = register_alloc();			
			fprintf(file, "\tMOV $%d,  %s\n", e->literal_value, register_name(e->reg));
			break;
		case EXPR_CHAR:
			e->reg = register_alloc();			
			fprintf(file, "\tMOV $%d,  %s\n", e->literal_value, register_name(e->reg));
			break;
		case EXPR_STRING:
			e->reg = register_alloc();
			// find the string
			sn = string_head;
			while(sn) {
				if(!strcmp(sn->text, e->string_literal)) {
					// string found
					found = 1;
					sprintf(string_label, "LC%d", string_count);
					e->reg = register_alloc();
					fprintf(file, "\tMOV $%s, %s\t\t# Move string into register\n", string_label, register_name(e->reg));
					break;
				}
				string_count++;
				sn = sn->next;
			}
			if(!found) {
				printf("Could not find string %s in data\n", e->string_literal);
				exit(1);
			}
			break;
		case EXPR_NAME:
			e->reg = register_alloc();
			fprintf(file, "\tMOV %s, %s\n", symbol_code(e->symbol), register_name(e->reg));
			break;
		case EXPR_ARRAY:
			break;
		case EXPR_ARRAY_LITERAL:
			break;	
	}
}
