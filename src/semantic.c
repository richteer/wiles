#include <stdio.h>
#include "scope.h"
#include "semantic.h"
#include "parser.tab.h"

extern int _verbose;
extern scope_t * top;

static int get_type(tree_t * t)
{
	int l,r;

	if (t->type == INUM) return INUM;
	else if (t->type == ID) return t->attribute.sval->type;
	else if ((t->type == ADDOP) || (t->type == MULOP)) {
		l = get_type(t->left);
		r = get_type(t->right);
		if (l != r) return 0;
		return l;
	}

	return 0;
}


static int sem_addop(tree_t * t)
{
	int l,r;

	l = get_type(t->left);
	r = get_type(t->right);

	if (!l || !r) {
		fprintf(stderr, "Error: weird type error, yo\n");
		return 2;
	}

	if (l != r) {
		fprintf(stderr, "Error: type mismatch, %d != %d\n",l,r);
		return 1;
	}

	return 0;
}

int sem_mulop(tree_t * t)
{

	return 0;
}

int sem_asnop(tree_t * t)
{

	return 0;
}

int sem_check(tree_t * t)
{
	if (!t) return -1;

	switch(t->type) {
		case ADDOP: return sem_addop(t);
		case MULOP: return sem_mulop(t);
		case ASNOP: return sem_asnop(t);
		case FUNCTION_CALL: return 0;
		case INUM:  return 0;
		default: fprintf(stderr, "Unknown operation, bailing... [%d]\n",t->type); return -1;
	}

	return 0;
}
