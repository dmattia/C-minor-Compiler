/*
Declare token types at the top of the bison file,
causing them to be automatically generated in parser.tab.h
for use by scanner.c.
*/

%token TOKEN_NOT
%token TOKEN_INTEGER
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_INCREMENT
%token TOKEN_DECREMENT
%token TOKEN_DIVISION
%token TOKEN_MODULUS
%token TOKEN_NOT_EQUALS
%token TOKEN_AND
%token TOKEN_OR
%token TOKEN_EXPONENT
%token TOKEN_INTEGER_LITERAL
%token TOKEN_ASTERISK
%token TOKEN_CHAR_LITERAL
%token TOKEN_STRING_LITERAL
%token TOKEN_ARRAY
%token TOKEN_BOOL
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE
%token TOKEN_LEFT_BRACKET
%token TOKEN_RIGHT_BRACKET
%token TOKEN_COMMA
%token TOKEN_COLON
%token TOKEN_LEFT_BRACE
%token TOKEN_RIGHT_BRACE
%token TOKEN_SEMICOLON
%token TOKEN_IDENTIFIER
%token TOKEN_FLOAT
%token TOKEN_COMMENT
%token TOKEN_STRING
%token TOKEN_UNRECOGNIZED
%token TOKEN_LT
%token TOKEN_GT
%token TOKEN_LE
%token TOKEN_GE
%token TOKEN_LEFT_PAREN
%token TOKEN_RIGHT_PAREN
%token TOKEN_EQUALS
%token TOKEN_ASSIGNMENT
%token TOKEN_WHITESPACE

%union {
	struct decl *decl;
	struct stmt *stmt;
	struct expr *expr;
	struct param_list *param_list;
	struct symbol *symbol;
	struct type *type;
	char *s;
};

%type <decl> program decl_list decl
%type <stmt> stmt stmt_list matched_stmt unmatched_stmt
%type <expr> expr expr_list opt_expr assignment_expr or_expr and_expr comp_expr add_expr mult_expr exponent_expr flip_expr prefix_expr postfix_expr group_expr primary_expr array_expr
%type <param_list> param_list not_empty_param_list param
%type <type> type
%type <s> ident

%{

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "param_list.h"
#include "symbol.h"
#include "type.h"

/*
Clunky: Manually declare the interface to the scanner generated by flex. 
*/

extern char *yytext;

extern int yylex();
extern int yyerror( char *str );

/*
Clunky: Keep the final result of the parse in a global variable,
so that it can be retrieved by main().
*/

struct decl *parser_result;

%}

%%

/* Here is the grammar: program is the start symbol. */

program		: decl_list
			{ parser_result = $1; return 0;}
		;

decl_list	: decl decl_list
			{ $1->next = $2; $$ = $1; }
		| /* nothing */
			{ $$ = 0; }
		;

decl		: ident TOKEN_COLON type TOKEN_ASSIGNMENT expr TOKEN_SEMICOLON
			{ $$ = decl_create( $1, $3, $5, 0, 0, 0); }
		| ident TOKEN_COLON type TOKEN_SEMICOLON
			{ $$ = decl_create( $1, $3, 0, 0, 0, 0); }
		| ident TOKEN_COLON type TOKEN_ASSIGNMENT TOKEN_LEFT_BRACE stmt_list TOKEN_RIGHT_BRACE
			{ $$ = decl_create( $1, $3, 0, $6, 0, 0); }
		| ident TOKEN_COLON type TOKEN_ASSIGNMENT TOKEN_LEFT_BRACE TOKEN_RIGHT_BRACE
			{ $$ = decl_create( $1, $3, 0, 0, 0, 1); }
		;

type		: TOKEN_STRING
			{ $$ = type_create(TYPE_STRING, 0, 0, 0); }
		| TOKEN_INTEGER
			{ $$ = type_create(TYPE_INTEGER, 0, 0, 0); }
		| TOKEN_CHAR
			{ $$ = type_create(TYPE_CHARACTER, 0, 0, 0); }
		| TOKEN_BOOL
			{ $$ = type_create(TYPE_BOOLEAN, 0, 0, 0); }
		| TOKEN_ARRAY TOKEN_LEFT_BRACKET opt_expr TOKEN_RIGHT_BRACKET type
			{ $$ = type_create(TYPE_ARRAY, $3, 0, $5); }
		| TOKEN_VOID
			{ $$ = type_create(TYPE_VOID, 0, 0, 0); }
		| TOKEN_FUNCTION type TOKEN_LEFT_PAREN param_list TOKEN_RIGHT_PAREN
			{ $$ = type_create(TYPE_FUNCTION, 0, $4, $2); }
		;

opt_expr	: expr
			{{ $$ = $1; }}
		| /* nothing */
			{{ $$ = 0; }}
		;

param_list	: not_empty_param_list
			{ $$ = $1; };
		| /* empty param list */
			{ $$ = 0; }
		;

not_empty_param_list 	: param TOKEN_COMMA param_list
				{ $1->next = $3; $$ = $1; }
			| param
				{ $$ = $1; }
			;

param		: ident TOKEN_COLON type
			{ $$ = param_list_create($1, $3, 0); }
		;

ident		: TOKEN_IDENTIFIER
			{ char *text;
			  text = (char *)malloc(sizeof(yytext));
			  strcpy(text, yytext);
			  $$ = text;
			}
		;

stmt_list	: stmt stmt_list
			{ $1->next = $2; $$ = $1; }
		| stmt
			{ $$ = $1; }
		;

stmt		: matched_stmt
			{ $$ = $1; }
		| unmatched_stmt
			{ $$ = $1; }
		;

matched_stmt	: TOKEN_IF TOKEN_LEFT_PAREN expr TOKEN_RIGHT_PAREN matched_stmt TOKEN_ELSE matched_stmt
			{ $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7); }
		| expr TOKEN_SEMICOLON
			{ $$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0); }
		| TOKEN_PRINT expr_list TOKEN_SEMICOLON
			{ $$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0); }
		| TOKEN_PRINT TOKEN_SEMICOLON
			{ $$ = stmt_create(STMT_PRINT, 0, 0, 0, 0, 0, 0); }
		| TOKEN_FOR TOKEN_LEFT_PAREN opt_expr TOKEN_SEMICOLON opt_expr TOKEN_SEMICOLON opt_expr TOKEN_RIGHT_PAREN matched_stmt
			{ $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0); }
		| TOKEN_LEFT_BRACE stmt_list TOKEN_RIGHT_BRACE
			{ $$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0); }
		| TOKEN_RETURN opt_expr TOKEN_SEMICOLON
			{ $$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0); }
		| decl
			{ $$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0); }
		;

unmatched_stmt	: TOKEN_IF TOKEN_LEFT_PAREN expr TOKEN_RIGHT_PAREN stmt
			{ $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, 0); }
		| TOKEN_IF TOKEN_LEFT_PAREN expr TOKEN_RIGHT_PAREN matched_stmt TOKEN_ELSE unmatched_stmt
			{ $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7); }
		;

expr_list	: expr TOKEN_COMMA expr_list
			{ $$ = expr_create(EXPR_LIST, $1, $3); }
		| expr
			{ $$ = expr_create(EXPR_LIST, $1, 0); }
		;

expr		: assignment_expr
			{ $$ = $1; }
		;

assignment_expr	: ident TOKEN_ASSIGNMENT assignment_expr
			{ $$ = expr_create(EXPR_ASSIGNMENT, expr_create_name($1), $3); }
		| array_expr TOKEN_ASSIGNMENT assignment_expr
			{ $$ = expr_create(EXPR_ASSIGNMENT, $1, $3); }
		| or_expr
			{ $$ = $1; }
		;

array_expr	: ident TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET
			{ $$ = expr_create(EXPR_ARRAY, expr_create_name($1), $3); }
		;

or_expr		: or_expr TOKEN_OR and_expr
			{ $$ = expr_create(EXPR_OR, $1, $3); }
		| and_expr
			{ $$ = $1; }
		;

and_expr	: and_expr TOKEN_AND comp_expr 
			{ $$ = expr_create(EXPR_AND, $1, $3); }
		| comp_expr
			{ $$ = $1; }
		;

comp_expr	: comp_expr TOKEN_LT add_expr
			{ $$ = expr_create(EXPR_LT, $1, $3); }
		| comp_expr TOKEN_GT add_expr
			{ $$ = expr_create(EXPR_GT, $1, $3); }
		| comp_expr TOKEN_LE add_expr
			{ $$ = expr_create(EXPR_LE, $1, $3); }
		| comp_expr TOKEN_GE add_expr
			{ $$ = expr_create(EXPR_GE, $1, $3); }
		| comp_expr TOKEN_EQUALS add_expr
			{ $$ = expr_create(EXPR_EQUALS, $1, $3); }
		| comp_expr TOKEN_NOT_EQUALS add_expr
			{ $$ = expr_create(EXPR_NOT_EQUALS, $1, $3); }
		| add_expr
			{ $$ = $1; }
		;

add_expr	: add_expr TOKEN_PLUS mult_expr
			{ $$ = expr_create(EXPR_ADD, $1, $3); }
		| add_expr TOKEN_MINUS mult_expr
			{ $$ = expr_create(EXPR_MINUS, $1, $3); }
		| mult_expr
			{ $$ = $1; }
		;

mult_expr	: mult_expr TOKEN_ASTERISK exponent_expr
			{ $$ = expr_create(EXPR_TIMES, $1, $3); }
		| mult_expr TOKEN_DIVISION exponent_expr
			{ $$ = expr_create(EXPR_DIVIDES, $1, $3); }
		| mult_expr TOKEN_MODULUS exponent_expr
			{ $$ = expr_create(EXPR_MOD, $1, $3); }
		| exponent_expr
			{ $$ = $1; }
		;

exponent_expr	: exponent_expr TOKEN_EXPONENT flip_expr
			{ $$ = expr_create(EXPR_POWER, $1, $3); }
		| flip_expr
			{ $$ = $1; }
		;

flip_expr	: TOKEN_MINUS flip_expr
			{ $$ = expr_create(EXPR_NEGATIVE, 0, $2); }
		| TOKEN_NOT flip_expr
			{ $$ = expr_create(EXPR_NOT, 0, $2); }
		| prefix_expr
			{ $$ = $1; }
		;

prefix_expr	: TOKEN_INCREMENT postfix_expr
			{ $$ = expr_create(EXPR_PRE_INCREMENT, 0, $2); }
		| TOKEN_DECREMENT postfix_expr
			{ $$ = expr_create(EXPR_PRE_DECREMENT, 0, $2); }
		| postfix_expr
			{ $$ = $1; }
		;

postfix_expr	: postfix_expr TOKEN_INCREMENT
			{ $$ = expr_create(EXPR_POST_INCREMENT, $1, 0); }
		| postfix_expr TOKEN_DECREMENT
			{ $$ = expr_create(EXPR_POST_DECREMENT, $1, 0); }
		| group_expr
			{ $$ = $1; }
		;

group_expr	: TOKEN_LEFT_PAREN expr TOKEN_RIGHT_PAREN
			{ $$ = $2; }
		| TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET
			{ $$ = $2; }
		| ident TOKEN_LEFT_PAREN expr_list TOKEN_RIGHT_PAREN
			{ $$ = expr_create(EXPR_FUNCTION, expr_create_name($1), $3); }
		| ident TOKEN_LEFT_PAREN TOKEN_RIGHT_PAREN
			{ $$ = expr_create(EXPR_FUNCTION, expr_create_name($1), 0); }
		| primary_expr
			{ $$ = $1; }
		| TOKEN_LEFT_BRACE expr_list TOKEN_RIGHT_BRACE
			{ $$ = expr_create(EXPR_ARRAY_LITERAL, 0, $2); }
		;

primary_expr	: TOKEN_TRUE
			{ $$ = expr_create_boolean_literal(1); }
		| TOKEN_FALSE
			{ $$ = expr_create_boolean_literal(0); }
		| TOKEN_INTEGER_LITERAL
			{ $$ = expr_create_integer_literal(atof(yytext)); }
		| TOKEN_CHAR_LITERAL
			{ $$ = expr_create_character_literal(yytext[1]); }
		| TOKEN_STRING_LITERAL
			{ char *text;
			  text = (char *)malloc(515); // max length string plus quotations plus null char
			  strcpy(text, yytext);
			  $$ = expr_create_string_literal(text);
			}
		| ident
			{ $$ = expr_create_name($1); }
		/*
		| array_expr
			{{ $$ = $1; }}
		;
		*/
		| ident TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET
			{ $$ = expr_create(EXPR_ARRAY, expr_create_name($1), $3); }
		;

%%

/*
This function will be called by bison if the parse should
encounter an error.  In principle, "str" will contain something
useful.  In practice, it often does not.
*/
int yyerror( char *str )
{
	printf("parse error: %s\n",str);
	return 1;
}
