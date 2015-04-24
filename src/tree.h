#ifndef _tree_h_
#define _tree_h_
#include "node.h"

typedef struct tree_s {
	int type;
	union {
		int ival;
		int opval;
		float rval;
		node_t * sval;
	} attribute;
	struct tree_s * left;
	struct tree_s * right;
} tree_t;

tree_t * make_tree(int type, tree_t * left, tree_t * right);
tree_t * make_inum(int val);
tree_t * make_rnum(int val);
tree_t * make_id(node_t * nd);
tree_t * make_op(int type, int attr, tree_t * left, tree_t * right);
void tree_print(tree_t *);

int typeify(tree_t *, int type);
#endif
