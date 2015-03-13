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

tree_t * make_id(node_t * nd)
{
	tree_t * p = make_tree(ID, NULL, NULL);
	p->attribute.sval = nd;
	return p;
}

tree_t * make_op(int type, int attr, tree_t * left, tree_t * right)
{
	tree_t * p = make_tree(type, left, right);
	p->attribute.opval = attr;
	return p;
}
