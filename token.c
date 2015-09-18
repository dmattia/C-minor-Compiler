#include "token.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>
#include "scannerUtil.h"

extern char* yytext;
extern int yylineno;

const char *token_string(token_t t) {
	error e;
	char* char_literal = "CHAR_LITERAL ";
	char* string_literal = "STRING_LITERAL ";
	char* without_quotes;
	char* literal;
	int i;

	switch(t) {
		case TOKEN_INTEGER:
			return "INTEGER";
		case TOKEN_ASTERISK:
			return "*";
		case TOKEN_INTEGER_LITERAL:
			return "INTEGER_LITERAL";
		case TOKEN_PLUS:
			return "PLUS";
		case TOKEN_MINUS:
			return "MINUS";
		case TOKEN_INCREMENT:
			return "INCREMENT";
		case TOKEN_DECREMENT:
			return "DECREMENT";
		case TOKEN_DIVISION:
			return "SLASH";
		case TOKEN_MODULUS:
			return "MODULO";
		case TOKEN_NOT_EQUALS:
			return "NOT_EQUALS";
		case TOKEN_AND:
			return "AND";
		case TOKEN_OR:
			return "OR";
		case TOKEN_EXPONENT:
			return "CARAT";
		case TOKEN_ARRAY:
			return "ARRAY";
		case TOKEN_BOOL:
			return "BOOL";
		case TOKEN_CHAR:
			return "CHAR";
		case TOKEN_ELSE:
			return "ELSE";
		case TOKEN_FALSE:
			return "FALSE";
		case TOKEN_FOR:
			return "FOR";
		case TOKEN_FUNCTION:
			return "FUNCTION";
		case TOKEN_IF:
			return "IF";
		case TOKEN_PRINT:
			return "PRINT";
		case TOKEN_RETURN:
			return "RETURN";
		case TOKEN_TRUE:
			return "TRUE";
		case TOKEN_VOID:
			return "VOID";
		case TOKEN_WHILE:
			return "WHILE";
		case TOKEN_COMMA:
			return "COMMA";
		case TOKEN_LEFT_BRACKET:
			return "[";
		case TOKEN_RIGHT_BRACKET:
			return "]";
		case TOKEN_CHAR_LITERAL:
			literal = (char*)malloc(strlen(char_literal) + strlen(yytext) - 1);
			strcpy(literal,char_literal);
			strcat(literal,remove_quotes(yytext));
			return literal;
		case TOKEN_COLON:
			return "COLON";
		case TOKEN_LEFT_BRACE:
			return "{";
		case TOKEN_RIGHT_BRACE:
			return "}";
		case TOKEN_SEMICOLON:
			return "SEMICOLON";
		case TOKEN_IDENTIFIER:
			if(strlen(yytext) > 256) {
				e.errorType = ERROR_BUFFER_OVERFLOW;
				e.description = "Identifiers cannot be more than 256 chars long";
				e.lineNum = yylineno;
				throw_error(e);
			}
			return "IDENTIFIER";
		case TOKEN_WHITESPACE:
			return "";
		case TOKEN_COMMENT:
			return "COMMENT";
		case TOKEN_STRING_LITERAL:
			literal = (char*)malloc(strlen(string_literal) + strlen(yytext) - 1);
			strcpy(literal,string_literal);
			strcat(literal,remove_quotes(yytext));
			if(strlen(fix_escape_chars(literal)) >= 256) {
				e.errorType = ERROR_BUFFER_OVERFLOW;
				e.description = "Strings cannot be more than 256 chars long";
				e.lineNum = yylineno;
				throw_error(e);
			}
			return literal;
		case TOKEN_LEFT_PAREN:
			return "(";
		case TOKEN_RIGHT_PAREN:
			return ")";
		case TOKEN_STRING:
			return "STRING";
		case TOKEN_GT:
			return "GT";
		case TOKEN_GE:
			return "GE";
		case TOKEN_LT:
			return "LT";
		case TOKEN_LE:
			return "LE";
		case TOKEN_EQUALS:
			return "EQUALS";
		case TOKEN_ASSIGNMENT:
			return "ASSIGNMENT";
		case TOKEN_UNRECOGNIZED:
			e.errorType = ERROR_UNRECOGNIZED_TOKEN;
			char* temp = "Could not find token: ";
			e.description = (char*)malloc(strlen(temp) + strlen(yytext) + 1);
			strcpy(e.description,temp);
			strcat(e.description,yytext);
			e.lineNum = yylineno;
			throw_error(e);
		default:
			return "default";
	}
}
