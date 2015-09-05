#include "token.h"
#include "stdio.h"
#include "string.h"
#include "error.h"
#include "stdlib.h"

extern token_t yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void scan_input(const char*);

int main(int argc, char*argv[]) {
	int i;
	int scan = 0; // controls if scan should be performed. true if -scan flag is present

	//Check through command line args for flags
	for(i=1; i < argc; ++i) {
		if (strcmp(argv[i],"-scan")==0) scan = 1;
	}

	if(scan && argc > 2) scan_input(argv[2]);

	return 0;
}

void scan_input(const char* filename) {
	token_t token;
	yyin = fopen(filename,"r");
	if(!yyin) {
		error e;
		e.errorType = ERROR_INVALID_ARGUMENT;
		char* temp = "Could not find file ";
		e.description = (char*)malloc(strlen(temp) + strlen(filename) + 1);
		strcpy(e.description,temp);
		strcat(e.description,filename);
		e.lineNum = -1; //as this is not in the actual file yet
		throw_error(e);
	}
	token = yylex();
	while(token) {
		printf("%s",token_string(token));
		token = yylex();
	}	
}
