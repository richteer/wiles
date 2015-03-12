#ifndef _node_h_
#define _node_h_

typedef struct node_s {
	char * name;
	int type;

	struct node_s * next;
} node_t;

node_t * make_node(char * name);
node_t * push_node(node_t * head, char * name);
node_t * search_node(node_t * head, char * name);

#endif
