enum token {
	TOKEN_INTEGER = 1,
	TOKEN_IDENTIFIER,
	TOKEN_FLOAT,
	TOKEN_COMMENT,
	TOKEN_STRING,
	TOKEN_UNRECOGNIZED,
	TOKEN_WHITESPACE
};

typedef enum token token_t;

const char *token_string(token_t);
