%option yylineno

%{
#include "token.h" 

token_t comment();
void eatToEndOfLine();
%}

DIGIT	         [0-9]
LETTER	         [a-zA-Z]
ID      	 [a-z][a-z0-9]*
WHITESPACE	 [ \t\n] 

%%

\/\/.*					return TOKEN_COMMENT;
\/\*					return comment();	
[-+]?{DIGIT}+  				return TOKEN_INTEGER;
{LETTER}({DIGIT}|{LETTER})*           	return TOKEN_IDENTIFIER;
{DIGIT}+\.{DIGIT}+			return TOKEN_FLOAT;
\"[^\"]*\"				return TOKEN_STRING;
{WHITESPACE}+                           return TOKEN_WHITESPACE;
.					return TOKEN_UNRECOGNIZED;

%%

// eat up all characters until end of multiline comment of EOF
token_t comment() {
	char lastChar, currentChar;

	lastChar = input();
	currentChar = input();

	while(currentChar != 0) {
		if(lastChar == '*' && currentChar == '/') break;
		lastChar = currentChar;
		currentChar = input();
	}

	return TOKEN_COMMENT;
}
