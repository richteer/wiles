#ifndef _node_h_
#define _node_h_

typedef struct node_s {
	char * name;
	int type;

	struct node_s * next;
} node_t;

node_t * node_create(char * name);
node_t * node_push(node_t * head, char * name);
node_t * node_search(node_t * head, char * name);

#endif
