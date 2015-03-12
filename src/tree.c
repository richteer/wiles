#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "parser.tab.h"

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
	tree_t * p = make_tree(INUM, NULL, NULL);
	p->attribute.ival = val;
	return p;
}

tree_t * make_rnum(int val)
{
	tree_t * p = make_tree(RNUM, NULL, NULL);
	p->attribute.rval = val;
	return p;
}

tree_t * make_id(char * name)
{
	tree_t * p = make_tree(ID, NULL, NULL);
	p->attribute.sval = strdup(name);
	return p;
}

tree_t * make_tree(int type, int attribute, tree_t * left, tree_t * right)
{
	tree_t * p = make_tree(ID, left, right);
	p->attribute.opval = attribute;
	return p;
}

