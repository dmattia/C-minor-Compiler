#include "list.h"
#include "stdlib.h"

void push_front(struct hash_table *h) {
	struct node *old_head = head;
	head = (struct node*)malloc(sizeof(struct node));
	head->hash_table = h;
	head->next = old_head;
}

void pop_front() {
	if(head) {
		struct node *old_head = head;
		head = head->next;
		hash_table_delete(old_head->hash_table);
		free(old_head);
	}
}
