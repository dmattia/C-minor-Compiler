// A global list of literal strings used throughout the program
// Used to get all literal strings into the data section

struct string_node {
	const char* text;
	struct string_node *next;
};

struct string_node *string_head;

void push_string_front(const char*);
