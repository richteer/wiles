#ifndef _scope_h_
#define _scope_h_

#include "node.h"

#define HASH_SIZE 211
#define EOS       '\0'

typedef struct scope_s {
	// Hash table: hashing with chaining
	node_t * table[HASH_SIZE];

	struct scope_s * next;
} scope_t;

// Basic search and insert
node_t * scope_search(scope_t * head, char * name);
node_t * scope_insert(scope_t * head, char * name);

node_t * scope_searchall(scope_t * head, char * name);


scope_t * make_scope(void);

// Stack routines
scope_t * scope_push(scope_t * top);
scope_t * scope_pop(scope_t * top);

#endif
