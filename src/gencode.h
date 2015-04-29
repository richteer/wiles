#include "tree.h"

#define spew(...) fprintf(outsrc, __VA_ARGS__)

int gencode(tree_t * t);
int gen_preamble(void);
int gen_postamble(char * name);
int gen_intro(char * name);
int gen_outro(void);
int gen_stalloc(int off);
int gen_dealloc(int off);
int gen_write(char * name, tree_t * t);
int gen_read(char * name, tree_t * t);
int gen_label(void);
int spew_jmp(char * jmp, int offset);
int gen_jmp(tree_t * t, int offset);
