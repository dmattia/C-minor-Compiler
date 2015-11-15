#ifndef ERROR_H
#define ERROR_H

enum errorTypes {
	ERROR_UNFOUND_IDENTIFIER = 1,
	ERROR_UNRECOGNIZED_TOKEN,
	ERROR_BUFFER_OVERFLOW,
	ERROR_INVALID_ARGUMENT,
	ERROR_MULTIPLE_DECLARATION,
};

typedef enum errorTypes error_t;

typedef struct {
	error_t errorType;
	char* description;
	int lineNum;
} error;

char* error_type(error);
void throw_error(error);

#endif
