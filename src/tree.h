typedef struct tree_s {
	int type;
	union {
		int ival;
		int opval;
		float rval;
		char * sval;
	} attribute;
	struct tree_s * left;
	struct tree_s * right;
} tree_t;

tree_t * make_tree(int type, tree_t * left, tree_t * right);
tree_t * make_inum(int val);
tree_t * make_rnum(int val);
tree_t * make_id(char * name);
