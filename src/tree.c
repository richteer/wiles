#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "parser.tab.h"

extern int _verbose;

#define DEBUG(...) _verbose&&fprintf(stderr,__VA_ARGS__)

tree_t * make_tree(int type, tree_t * left, tree_t * right)
{
	tree_t * p = malloc(sizeof(tree_t));

	p->type = type;
	p->left = left;
	p->right = right;

	return p;
}

tree_t * make_inum(int val)
{
	DEBUG("\nMaking INUM: %d\n", val);
	tree_t * p = make_tree(INUM, NULL, NULL);
	p->attribute.ival = val;
	return p;
}

tree_t * make_rnum(int val)
{
	DEBUG("\nMaking RNUM: %d\n", val);
	tree_t * p = make_tree(RNUM, NULL, NULL);
	p->attribute.rval = val;
	return p;
}

tree_t * make_id(node_t * nd)
{
	DEBUG("\nMaking ID\n");
	tree_t * p = make_tree(ID, NULL, NULL);
	p->attribute.sval = nd;
	return p;
}

tree_t * make_op(int type, int attr, tree_t * left, tree_t * right)
{
	DEBUG("\nMaking OP: type:%d attr:%d\n", type, attr);
	tree_t * p = make_tree(type, left, right);
	p->attribute.opval = attr;
	return p;
}

tree_t * make_asn(node_t * nd, tree_t * left, tree_t * right)
{
	DEBUG("\nMaking ASNOP\n");
	tree_t * p = make_tree(ASNOP, left, right);
	p->attribute.sval = nd;
	return p;
}

int typeify(tree_t * t, int type)
{
	if (t->type == ID) {
		t->attribute.sval->type = type;
		return 0;
	}
	typeify(t->left, type);
	typeify(t->right, type);
	return 0;
}

static void tprint(tree_t * t, int spaces)
{
	int i;
	if (t == NULL) return;

	for (i=0; i<spaces; i++) {
		fprintf(stderr, " ");
	}

	/* process root */
	switch(t->type) {
		case ID:    fprintf(stderr, "[ID:%s]",     (t->attribute.sval)->name); break;
		case INUM:  fprintf(stderr, "[INUM:%d]",    t->attribute.ival);        break;
		case RNUM:  fprintf(stderr, "[RNUM:%f]",    t->attribute.rval);        break;
		case ADDOP: fprintf(stderr, "[ADDOP:%d]",   t->attribute.opval);       break;
		case MULOP: fprintf(stderr, "[MULOP:%d]",   t->attribute.opval);       break;
		case RELOP: fprintf(stderr, "[RELOP:%d]",   t->attribute.opval);       break;
		default:    fprintf(stderr, "[UNKNOWN:%d]", t->type);                  break;
	}

	fprintf(stderr, "\n");
	tprint(t->left, spaces+4);
	tprint(t->right, spaces+4);
}


void tree_print(tree_t * t)
{
	fprintf(stderr, "\nTree:\n");
	tprint(t, 0);
	fprintf(stderr, "\n\n");
}
