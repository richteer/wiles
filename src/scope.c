#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "scope.h"

#define RETNULL(a) if(NULL==a)return NULL

scope_t * top;
extern int _verbose;
extern int offstat;

int depth = 0;

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
	int offset;

	RETNULL(scp);

	if (scope_search(scp, name)) {
		fprintf(stderr, "ERROR! Name '%s' already defined in this scope!\n", name);
		assert(0);
	}

	DEBUG("Inserting %s\n",name);

	i = hashpjw(name);
	hd = scp->table[i];

	switch(offstat) {
		case -1: offset = scp->off_loc -= 8; break;
		case 0 : offset = 0;                 break;
		case 1 : offset = scp->off_arg += 8; break;
	}

	scp->table[i] = node_push(hd, name, offset);
	return scp->table[i];

}

// Global Search
node_t * scope_searchall(scope_t * scp, char * name)
{
	node_t * ret;

	RETNULL(scp);

	while (scp != NULL) {
		ret = scope_search(scp, name);
		if (ret != NULL) {
			ret->depth = depth;
			depth = 0;
			return ret;
		}
		depth++;
		scp = scp->next;
	}

	depth = 0;
	fprintf(stderr, "Error: could not find id '%s', did you declare it?\n", name);
	assert(0);
	return NULL;
}

// Constructor
scope_t * make_scope(void)
{
	scope_t * p = calloc(1,sizeof(scope_t));
	p->off_arg = 8;
	return p;
}

// Push new scope
scope_t * scope_push(scope_t * top, int type)
{
	scope_t * new = make_scope();
	new->type = type;
	new->next = top;
	return new;
}

void scope_func(scope_t * top, node_t * fun, int type)
{
	int i;
	node_t * n;
	typen_t * t;

	assert(top);
	assert(fun);

	fun->func = calloc(1,sizeof(func_t));
	fun->func->rettype = type;

	for (i = 0; i < HASH_SIZE; i++) {
		for (n = top->table[i]; n != NULL; n = n->next) {
			if (n == fun) {
				fprintf(stderr,"WLKAJHGSKJDHASKJDGASKJHDGASD\n");
				continue;
			}
			(fun->func->numargs)++;
			t = calloc(1, sizeof(typen_t));
			t->type = n->type;
			t->next = fun->func->types;
			fun->func->types = t;
		}
	}
}



static void scope_free_list(node_t * n)
{
	node_t * t = NULL;

	for (n = t; n && t ;) {
		t = n->next;
		node_free(n);
		n = t;
	}
}

static void scope_free(scope_t * scp)
{
	int i;

	for (i = 0; i < HASH_SIZE; i++) {
		scope_free_list(scp->table[i]);
	}

	free(scp);
}

// Pop first in scope
scope_t * scope_pop(scope_t * top)
{
	scope_t * tmp;

	RETNULL(top);

	tmp = top->next;
	scope_free(top);

	return tmp;
}
