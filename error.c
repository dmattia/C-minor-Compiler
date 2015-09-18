#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void throw_error(error e) {
	fflush(stdout);
	if(e.lineNum != -1) {
		fprintf(stderr,"%s: %s: on line %d\n",error_type(e),e.description,e.lineNum);	
	} else {
		fprintf(stderr,"%s: %s\n",error_type(e),e.description);	
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
			return "scan error";
		case ERROR_BUFFER_OVERFLOW:
			return "buffer overflow";
		default:
			return "Unknown error type";
	}
}
