#ifndef _node_h_
#define _node_h_

typedef struct node_s {
	char * name;
	int type;
	int offset;
	int depth;
	struct node_s * next;
} node_t;

node_t * node_create(char * name);
node_t * node_push(node_t * head, char * name, int offset);
node_t * node_search(node_t * head, char * name);
void node_free(node_t * n);

#endif
