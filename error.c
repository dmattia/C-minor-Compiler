#include "error.h"
#include "stdio.h"
#include "stdlib.h"

void throw_error(error e) {
	if(e.lineNum != -1) {
		printf("%s: %s: on line %d\n",error_type(e),e.description,e.lineNum);	
	} else {
		printf("%s: %s\n",error_type(e),e.description);	
	}
	exit((int)e.errorType);
}

char* error_type(error e) {
	switch(e.errorType) {
		case ERROR_UNFOUND_IDENTIFIER:
			return "Unfound Identifier";
		case ERROR_INVALID_ARGUMENT:
			return "Invalid Argument";
		case ERROR_UNRECOGNIZED_TOKEN:
			return "Unrecognized Token";
		default:
			return "Unknown error type";
	}
}
