#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "token.h"
#include "decl.h"
#include "scope.h"
#include "scannerUtil.h"

extern token_t yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern struct node *head;

/* Clunky: Declare the parse function generated from parser.bison */
extern int yyparse();
/* Clunky: Declare the result of the parser from parser.bison */
extern struct decl *parser_result;
int type_check_errors;

void scan_input(const char*);
void parse_input(const char*);
void resolve_input(const char*);
void typecheck_input(const char*);
void codegen_input(const char*);
FILE* safe_open(const char*);

int main(int argc, char* argv[]) {
	int i;
	int scan = 0;  // controls if scan should be performed. true if -scan flag is present
	int parse = 0; // controls if parsing should be performed. true if -parse flag is present
	int resolve = 0; // controls if resolving the ast should be performed. true if -resolve flag present
	int typecheck = 0; // controls if typecheckint should be performed. true if -typecheck flag present
	int codegen = 0; // controls if codegen should be printed. true if -codegen flag present

	//Check through command line args for flags
	for(i=1; i < argc; ++i) {
		if (strcmp(argv[i],"-scan")==0) scan = 1;
		if (strcmp(argv[i],"-parse")==0) parse = 1;
		if (strcmp(argv[i],"-resolve")==0) resolve = 1;
		if (strcmp(argv[i],"-typecheck")==0) typecheck = 1;
		if (strcmp(argv[i],"-codegen")==0) codegen = 1;
	}

	if ((scan || parse || resolve || typecheck || codegen) && argc <= 2) {
		error e;
		e.errorType = ERROR_INVALID_ARGUMENT;
		e.description = "Must include a file name";
		e.lineNum = -1;
		throw_error(e);
	}

	if(scan) {
		scan_input(argv[2]);
	} else if (parse) {
		parse_input(argv[2]);
	} else if (resolve) {
		resolve_input(argv[2]);
	} else if (typecheck) {
		typecheck_input(argv[2]);
	} else if (codegen) {
		codegen_input(argv[2]);
	} else {
		error e;
		e.errorType = ERROR_INVALID_ARGUMENT;
		e.description = "Must include -parse, -scan, or -resolve as a flag argument";
		e.lineNum = -1;
		throw_error(e);
	}

	return 0;
}

void scan_input(const char* filename) {
	token_t token;
	yyin = safe_open(filename);
	token = yylex();
	while(token) {
		const char* result = token_string(token);
		if(result[0] != '\0') {
			printf("%s\n",fix_escape_chars(token_string(token)));
		}
		token = yylex();
	}	
	fclose(yyin);
}

void parse_input(const char* filename) {
	yyin = safe_open(filename);
	if(!yyparse()) {
		decl_print(parser_result, 0);
	}
	fclose(yyin);
}

void resolve_input(const char* filename) {
	yyin = safe_open(filename);
	if(!yyparse()) {
		head = 0;
		scope_enter(0);
		decl_resolve(parser_result, 0);
		scope_leave(0);
	}
	fclose(yyin);
}

void typecheck_input(const char* filename) {
	type_check_errors = 0;
	yyin = safe_open(filename);
	if(!yyparse()) {
		printf("NAME RESOLUTION:\n\n");
		head = 0;
		scope_enter(0);
		decl_resolve(parser_result, 0);
		scope_leave(0);
		printf("\nTYPE CHECKING:\n\n");
		decl_typecheck(parser_result);
		printf("typecheck complete\n");
	}
	fclose(yyin);
	if(type_check_errors > 0) {
		exit(1);
	} else {
		exit(0);
	}
}

void codegen_input(const char* filename) {
	type_check_errors = 0;
	yyin = safe_open(filename);
	if(!yyparse()) {
		printf("NAME RESOLUTION:\n\n");
		head = 0;
		scope_enter(0);
		decl_resolve(parser_result, 0);
		scope_leave(0);
		printf("\nTYPE CHECKING:\n\n");
		decl_typecheck(parser_result);
		printf("typecheck complete\n");
	}
	fclose(yyin);
	if(type_check_errors > 0) {
		exit(1);
	} else {
		// No errors from scanning, parsing, resolving, and then typechecking
		// Begin codegen
		FILE *output;
		output = fopen("output.s", "w");
		fprintf(output, ".file \"%s\"\n\n", filename);
		decl_codegen(parser_result, output);
		exit(0);
	}
}

FILE* safe_open(const char* filename) {
	FILE *f = fopen(filename,"r");
	if(!f) {
		error e;
		e.errorType = ERROR_INVALID_ARGUMENT;
		char* temp = "Could not find file ";
		e.description = (char*)malloc(strlen(temp) + strlen(filename) + 1);
		strcpy(e.description,temp);
		strcat(e.description,filename);
		e.lineNum = -1; //as this is not in the actual file yet
		throw_error(e);
	}
	return f;
}
