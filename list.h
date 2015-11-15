#ifndef LIST_H
#define LIST_H

#include "hash_table.h"

struct node {
	struct hash_table *hash_table;
	struct node *next;	
};

struct node *head;

void push_front(struct hash_table *h);
void pop_front();

#endif
