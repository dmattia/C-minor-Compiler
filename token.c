#include "token.h"
#include "error.h"
#include "string.h"
#include "stdlib.h"

extern char* yytext;
extern int yylineno;

const char *token_string(token_t t) {
	error e;

	switch(t) {
		case TOKEN_INTEGER:
			return "integer";
		case TOKEN_IDENTIFIER:
			return "identifier";
		case TOKEN_FLOAT:
			return "float";
		case TOKEN_WHITESPACE:
			return yytext;
		case TOKEN_COMMENT:
			return "comment";
		case TOKEN_STRING:
			return "string";
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
