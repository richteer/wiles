#include <stdio.h>
#include "scope.h"
#include "node.h"
#include "semantic.h"
#include "parser.tab.h"

extern int _verbose;
extern scope_t * top;

static int get_type(tree_t * t)
{
	int l,r;

	if (t->type == INUM) return INUM;
	else if (t->type == ID) {fprintf(stderr, "TYPECHECK: %s is %d\n", t->attribute.sval->name, t->attribute.sval->type); return t->attribute.sval->type; }
	else if (t->type == FUNCTION_CALL) {
		fprintf(stderr, "FUNCTION CALLLLLLLLL\n");
		return t->left->attribute.sval->func->rettype;
	}
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

static int sem_mulop(tree_t * t)
{

	return 0;
}

static int asnop_findproc(tree_t * t)
{
	int l,r;

	if (!t) {
		return 0;
	}

	if (t->type == PROCEDURE_CALL) {
		return 1;
	}

	l = r = 0;

	l = asnop_findproc(t->left);
	r = asnop_findproc(t->right);

	return l || r;
}

static int sem_asnop(tree_t * t)
{
	int r;
	// Case for returning in functions
	fprintf(stderr, "asnop checking %d <--> %d\n", t->left->type, t->right->type);
	if (t->left->type == ID && t->left->attribute.sval->func && t->left->attribute.sval->func->rettype) {
		if (t->left->attribute.sval->func->rettype != (r = get_type(t->right))) {
			fprintf(stderr, "Error: Returning a type other than the return type of the function%d <--> %d\n", t->left->attribute.sval->func->rettype, r);
			return 3;
		}
		fprintf(stderr, "ITS GOOOOOOOOOD\n");
		return 0;
	}

	if (t->type == ID && t->left->attribute.sval->type != (r = get_type(t->right))) {
		fprintf(stderr, "Error: tried to assign a type %d to a type %d\n", r, t->left->type);
		return 1;
	}

	if (asnop_findproc(t->right)) {
		fprintf(stderr, "Error: Silly rabbit, procedures are for other things!\n");
		return 2;
	}

	return 0;
}

static int procall_countargs(tree_t * t)
{
	if (!t) return 0;

	return procall_countargs(t->left) + 1;
}

static int sem_procall(tree_t * t)
{
	int num;
	func_t * fun;

	num = procall_countargs(t->right);
	fun = t->left->attribute.sval->func;

	if (fun->numargs != num) {
		fprintf(stderr, "Error, number of arguments in function %s does not match declaration. Expected %d, got %d\n", t->attribute.sval->name, fun->numargs, num);
		return 1;
	}

	return 0;
}

int sem_check(tree_t * t)
{
	if (!t) return -1;

	int ret, l, r;
	ret = l = r = 0;

	switch(t->type) {
		case ADDOP: ret = sem_addop(t); break;
		case MULOP: ret = sem_mulop(t); break;
		case ASNOP: ret = sem_asnop(t); break;
		case FUNCTION_CALL: ret = 0; break;
		case PROCEDURE_CALL: ret = sem_procall(t); break;
		case INUM:  ret = 0; break;
		case ID: ret = 0; break;
		default: fprintf(stderr, "Unknown operation, bailing... [%d]\n",t->type); return -1;
	}

	return ret;
}
