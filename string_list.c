#include "string_list.h"
#include <stdlib.h>

void push_string_front(const char* s) {
	struct string_node *old_head = string_head;
	string_head = (struct string_node*)malloc(sizeof(struct string_node));
	string_head->text = s;
	string_head->next = old_head;
}
