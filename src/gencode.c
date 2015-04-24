#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "scope.h"
#include "gencode.h"
#include "parser.tab.h"

#define GENSWITCH(t) switch((!t->left && !t->right && (t->rank == 1)) ? 0 : ((t->right && (t->right->rank == 0)) ? 1 : (t->left && t->right && t->left->rank >= t->right->rank) ? 2 : ((t->left && t->right && t->left->rank < t->right->rank) ? 3 : 4)))

char registers[3][5] = {
	"%esi",
	"%ebx",
	"%edi"
};

typedef struct reg_s {
	int num;
	struct reg_s * next;
} reg_t;

typedef struct {
	reg_t * top;
} stack_t;

stack_t st;

static void reg_init(void)
{
	reg_t * r;
	int i;
	st.top = NULL;
	for (i = 2; i >= 0; --i) {
		r = st.top;
		st.top = malloc(sizeof(reg_t));
		st.top->num = i;
		st.top->next = r;
	}

	return;
}

static void reg_deinit(void)
{
	reg_t *r, *tmp;
	for (r = st.top; r != NULL;) {
		tmp = r;
		r = r->next;
		free(tmp);
	}
}

static reg_t * reg_pop(void)
{
	reg_t * r;

	r = st.top;
	st.top = st.top->next;

	return r;
}

static void reg_push(reg_t * r)
{
	reg_t * t;

	t = st.top;
	st.top = r;
	r->next = t;

	return;
}

static void reg_swap(void)
{
	reg_t * n;

	n = st.top->next;
	st.top->next = n->next;
	n->next = st.top;
	st.top = n;

	return;
}

static int gen_rankify(tree_t * t)
{
	assert(t);
	if (!(t->left || t->right)) {
		return 0;
	}

	if (t->left && !t->left->left && !t->left->right) {
		t->left->rank = 1;
	}
	if (t->right && !t->right->left && !t->right->right) {
		t->right->rank = 0;
	}

	gen_rankify(t->left);
	gen_rankify(t->right);

	if (t->left->rank == t->right->rank) {
		t->rank = t->left->rank + 1;
	}
	else {
		t->rank = (t->left->rank < t->right->rank) ?
			t->right->rank : t->left->rank;
	}

	return 0;
}

static int gen_go(tree_t * t)
{
	reg_t * r;

	//int moo = GENSWITCH(t);

	//printf("Case %d\n", moo);
	//switch(moo) {
	GENSWITCH(t) {}
		case 0:
			// MOV to top
			printf("MOV %d, %s\n", t->attribute.ival, registers[st.top->num]);
			break;
		case 1:
			gen_go(t->left);
			// OP a,top
			printf("OP %d, %s\n", t->right->attribute.ival, registers[st.top->num]);
			break;
		case 2:
			reg_swap();
			gen_go(t->right);
			r = reg_pop();
			gen_go(t->left);
			printf("OP %s, %s\n", registers[r->num], registers[st.top->num]);
			// OP R,topstack
			reg_push(r);
			reg_swap();
		case 3:
			gen_go(t->left);
			r = reg_pop();
			gen_go(t->right);
			printf("OP %s, %s\n", registers[st.top->num], registers[r->num]);
			// OP
			reg_push(r);
			break;

		default: fprintf(stderr, "Wtf?\n"); assert(0); break;
	}

	return 0;
}

void reg_print(void)
{
	reg_t * r;
	for (r = st.top; r; r = r->next)
		printf("%d\n",r->num);
}

int gencode(tree_t * t)
{
	if (!t) return -1;

	reg_init();
	reg_print();

	gen_rankify(t);
	gen_go(t);

	reg_deinit();

	return 0;
}
