#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "scope.h"

#define RETNULL(a) if(NULL==a)return NULL

scope_t * top;
extern int _verbose;
#define DEBUG(...) _verbose&&fprintf(stderr, __VA_ARGS__)

// Magic hash function (page 436)
static int hashpjw(char *s)
{
	char * p;

	unsigned h = 0, g;

	for (p = s; *p != EOS; p++) {
		h = (h << 4) + (*p);
		if ((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	return h % HASH_SIZE;
}

// Local Search
node_t * scope_search(scope_t * scp, char * name)
{
	int i;
	node_t * head;

	RETNULL(scp);

	i = hashpjw(name);

	head = scp->table[i];

	return node_search(head, name);
}

// Local Insert
node_t * scope_insert(scope_t * scp, char * name)
{
	int i;
	node_t * hd;

	RETNULL(scp);

	DEBUG("Inserting %s\n",name);

	i = hashpjw(name);
	hd = scp->table[i];

	scp->table[i] = node_push(hd, name);
	return scp->table[i];

}

// Global Search
node_t * scope_searchall(scope_t * scp, char * name)
{
	node_t * ret;

	RETNULL(scp);

	while (scp != NULL) {
		ret = scope_search(scp, name);
		if (ret != NULL) return ret;

		scp = scp->next;
	}

	fprintf(stderr, "Warning, could not find id '%s'\n", name);
	return NULL;
}

// Constructor
scope_t * make_scope(void)
{
	scope_t * p = calloc(1,sizeof(scope_t));

	return p;
}

// Push new scope
scope_t * scope_push(scope_t * top)
{
	scope_t * new = make_scope();

	new->next = top;
	return new;
}

// Pop first in scope
scope_t * scope_pop(scope_t * top)
{
	scope_t * tmp;

	RETNULL(top);

	tmp = top;

	return top->next;
}
