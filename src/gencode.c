#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "scope.h"
#include "gencode.h"
#include "parser.tab.h"

#define GENSWITCH(t) switch((!t->left && !t->right && (t->rank == 1)) ? 0 : ((t->right && (t->right->rank == 0)) ? 1 : (t->left && t->right && t->left->rank >= t->right->rank) ? 2 : ((t->left && t->right && t->left->rank < t->right->rank) ? 3 : 4)))

extern FILE * outsrc;

char registers[][5] = {
	"%r10",
	"%r11",
	"%r12",
	"%r13",
	"%r14",
	"%r15",
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
	// TODO: Fix this
	for (i = 5; i >= 0; --i) {
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

int gen_preamble(void)
{
	fprintf(outsrc, "\t.globl main\n");
	fprintf(outsrc, "main:\n");
	fprintf(outsrc, "\tpushq\t%%rbp\n");
	fprintf(outsrc, "\tmovq\t%%rsp, %%rbp\n");

	return 0;
}

int gen_postamble(void)
{
	fprintf(outsrc, "\tleave\n");
	fprintf(outsrc, "\tret\n");

	return 0;
}

static int gen_addop(tree_t * t, reg_t * l, reg_t * r)
{
	if (l == NULL) {
		fprintf(outsrc, "\taddq\t$%d, %s\n", t->right->attribute.ival, registers[r->num]);
		return 0;
	}
	assert(r);

	fprintf(outsrc, "\taddq\t%s, %s\n", registers[l->num], registers[r->num]);

	return 0;
}

static int gen_mulop(tree_t * t, reg_t * l, reg_t * r)
{
	if (l == NULL) {
		fprintf(outsrc, "\timulq\t$%d, %s\n", t->right->attribute.ival, registers[r->num]);
		return 0;
	}
	assert(r);

	fprintf(outsrc, "\timulq\t%s, %s\n", registers[l->num], registers[r->num]);

	return 0;
}

static int gen_op(tree_t * t, reg_t * l, reg_t * r)
{
	switch (t->type) {
		case ADDOP: gen_addop(t,l,r); break;
		case MULOP: gen_mulop(t,l,r); break;
		default: fprintf(stderr, "BALRGH\n"); break;
	}

	return 0;
}


static int gen_go(tree_t * t)
{
	reg_t * r;

	//int moo = GENSWITCH(t);

	//printf("Case %d\n", moo);
	//switch(moo) {
	GENSWITCH(t) {
		case 0:
			// MOV to top
			printf("MOV %d, %s\n", t->attribute.ival, registers[st.top->num]);
			fprintf(outsrc, "\tmovq\t$%d, %s\n", t->attribute.ival, registers[st.top->num]);
			break;
		case 1:
			gen_go(t->left);
			gen_op(t, NULL, st.top);
			printf("OP %d, %s\n", t->right->attribute.ival, registers[st.top->num]);
			break;
		case 2:
			reg_swap();
			gen_go(t->right);
			r = reg_pop();
			gen_go(t->left);
			printf("OP %s, %s\n", registers[r->num], registers[st.top->num]);
			gen_op(t, r, st.top);
			reg_push(r);
			reg_swap();
		case 3:
			gen_go(t->left);
			r = reg_pop();
			gen_go(t->right);
			printf("OP %s, %s\n", registers[st.top->num], registers[r->num]);
			gen_op(t, st.top, r);
			reg_push(r);
			break;

		default: fprintf(stderr, "Wtf?\n"); assert(0); break;
	}

	return 0;
}

int gencode(tree_t * t)
{
	if (!t) return -1;

	reg_init();

	gen_rankify(t);
	gen_go(t);

	reg_deinit();

	return 0;
}
