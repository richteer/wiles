#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

node_t * node_create(char * name)
{
	node_t * p = (node_t *) malloc(sizeof(node_t));
	p->name = strdup(name);
	return p;
}

node_t * node_push(node_t * head, char * name)
{
	node_t * p = node_create(name);
	p->next = head;
	return p;
}

node_t * node_search(node_t * head, char * name)
{
	node_t * p = head;

	while (p != NULL) {
		if (!strcmp(p->name, name)) {
			return p;
		}
		p = p->next;
	}
	return NULL;
}
