%option yylineno

%{
#include "parser.tab.h"

void comment();
void eatToEndOfLine();
%}

DIGIT	         [0-9]
LETTER	         [a-zA-Z]
ID      	 [a-z][a-z0-9]*
WHITESPACE	 [ \t\n\r] 
ESCAPE           \\[abfnrtb\\\'\"\?]

%%

array 							return TOKEN_ARRAY;
boolean 						return TOKEN_BOOL;
char 							return TOKEN_CHAR;
else 							return TOKEN_ELSE;
false 							return TOKEN_FALSE;
for 							return TOKEN_FOR;
function 						return TOKEN_FUNCTION;
if 							return TOKEN_IF;
integer 						return TOKEN_INTEGER;
print 							return TOKEN_PRINT;
return		 					return TOKEN_RETURN;
string 							return TOKEN_STRING;
true 							return TOKEN_TRUE;
void 							return TOKEN_VOID;
while							return TOKEN_WHILE;
\+							return TOKEN_PLUS;
-							return TOKEN_MINUS;
\++							return TOKEN_INCREMENT;
--							return TOKEN_DECREMENT;
!							return TOKEN_NOT;
\/							return TOKEN_DIVISION;
%							return TOKEN_MODULUS;
!=							return TOKEN_NOT_EQUALS;
&&							return TOKEN_AND;
\|\|							return TOKEN_OR;
\^							return TOKEN_EXPONENT;
\*							return TOKEN_ASTERISK;
:							return TOKEN_COLON;
,							return TOKEN_COMMA;
\{							return TOKEN_LEFT_BRACE;
\}							return TOKEN_RIGHT_BRACE;
;							return TOKEN_SEMICOLON;
\(							return TOKEN_LEFT_PAREN;
\)							return TOKEN_RIGHT_PAREN;
\<							return TOKEN_LT;
\>							return TOKEN_GT;
\<=							return TOKEN_LE;
\>=							return TOKEN_GE;
=							return TOKEN_ASSIGNMENT;
==							return TOKEN_EQUALS;
\/\/.*							/* comment: return nothing */
\/\*							comment();	
\[							return TOKEN_LEFT_BRACKET;
\]							return TOKEN_RIGHT_BRACKET;
({LETTER}|_)({DIGIT}|{LETTER}|_)*      			return TOKEN_IDENTIFIER;
{DIGIT}+  						return TOKEN_INTEGER_LITERAL;
\'({LETTER}|{DIGIT}|{ESCAPE}|[ /])\'			return TOKEN_CHAR_LITERAL;
\"[^"\n]*\"						return TOKEN_STRING_LITERAL;
{WHITESPACE}+                           		/* return nothing */
.							return TOKEN_UNRECOGNIZED;

%%

// eat up all characters until end of multiline comment or EOF
void comment() {
	char lastChar, currentChar;

	lastChar = input();
	currentChar = input();

	while(currentChar != 0) {
		if(lastChar == '*' && currentChar == '/') break;
		lastChar = currentChar;
		currentChar = input();
	}
}
