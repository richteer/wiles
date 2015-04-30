%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "scope.h"
#include "semantic.h"
#include "parser.tab.h"
#include "gencode.h"

extern scope_t * top;
extern int linecount;

int _verbose = 0;
int offstat = 0;

FILE * outsrc;

%}

%union {
	int ival;
	float rval;
	char * sval;
	int opval;

	tree_t * tval;
}

%token <opval> ASNOP
%token <opval> RELOP
%token <opval> ADDOP
%token <opval> MULOP
%token LT LE GT GE EQ NE
%token OR PLUS MINUS
%token SLASH STAR AND DIV MOD

%token PROGRAM PROCEDURE FUNCTION
%token BEGINN END
%token ARRAY RANGE OF
%token IF THEN ELSE NOT
%token DO WHILE
%token VAR INTEGER REAL NUM

%token COMMA
%token FUNCTION_CALL
%token PROCEDURE_CALL
%token ARRAY_ACCESS

%token <sval> ID
%token <ival> INUM
%token <rval> RNUM

%type <tval> expression
%type <tval> expression_list
%type <tval> identifier_list
%type <tval> declarations
%type <tval> simple_expression
%type <tval> term
%type <tval> factor
%type <ival> standard_type
%type <ival> type
%type <sval> variable;

%%

program
	: { gen_preamble(); top = scope_push(top); }
		PROGRAM ID '(' identifier_list ')' ';'
	{ offstat = -1; }
		declarations
	{ offstat = 0; }
		subprogram_declarations
	{ gen_intro($3); gen_stalloc(top->off_loc); }
		compound_statement
	{
		gen_dealloc(top->off_loc);
		gen_outro();
	}
		'.'
      { gen_postamble($3); top = scope_pop(top); }
	;

identifier_list
	: ID
    	{ fprintf(stderr, "derpings %s\n", $1); $$ = make_tree(COMMA, NULL, make_id(scope_insert(top, $1))); }
	| identifier_list ',' ID
    	{ $$ = make_tree(COMMA, $1, make_id(scope_insert(top, $3))); }
	;

declarations
	: declarations VAR identifier_list ':' type ';'
		{ tree_typeify($3, $5); $$ = $3; }
	|
		{ $$ = NULL; }
	;

type
	: standard_type
		{ $$ = $1; }
	//| ARRAY '[' INUM RANGE INUM ']' OF standard_type
	;

standard_type
	: INTEGER
		{ $$ = INUM; }
	//| REAL
	;

subprogram_declarations
	: subprogram_declarations subprogram_declaration ';'
	|
	;

subprogram_declaration
	: subprogram_head
		{ offstat = -1; }
	declarations
		{ offstat = 0; }
	compound_statement
		{
			gen_dealloc(top->off_loc); top = scope_pop(top); gen_outro();
		}
	;

subprogram_head
	: FUNCTION ID
			{
				scope_insert(top, $2);
				top = scope_push(top);
				gen_intro($2);
			}
		arguments ':' standard_type ';'
			{
				node_t * n;
				n = scope_search(top->next, $2);
				scope_func(top, n, $6);
				gen_stalloc(top->off_loc);
			}
	| PROCEDURE ID
			{
				scope_insert(top, $2);
				top = scope_push(top);
				gen_intro($2);
			}
		arguments ';'
			{
				node_t * n;
				n = scope_search(top->next, $2);
				scope_func(top, n, 0);
				gen_stalloc(top->off_loc);
			}
	;

arguments
	: { offstat = 1; }
	'(' parameter_list ')'
	  { offstat = 0; }
	|
	;

parameter_list
	: identifier_list ':' type
		{
			tree_typeify($1, $3);
		}
	| parameter_list ';' identifier_list ':' type
	;

compound_statement
	: BEGINN optional_statements END
	;

optional_statements
	: statement_list
	|
	;

statement_list
	: statement
	| statement_list ';' statement
	;

statement
	: variable ASNOP expression
		{
			tree_t * t;
			t = make_tree(ASNOP, make_id(scope_searchall(top, $1)), $3);
			fprintf(stderr, "trying to assign to %s:%d\n", $1, t->left->type);
			tree_print(t);
			assert(!sem_check(t));
			gencode(t);
			tree_free(t);
		}
	| procedure_statement
	| compound_statement
	| IF expression
		{
			if ($2->type == RELOP) {
				gencode($2);
				gen_jmp($2, 0);
				spew_jmp("jmp", 1);
			}
			else {
				gencode($2);
				spew("\tcmpl\t$0, %%r10\n");
				spew_jmp("jne", 0);
				spew_jmp("jmp", 1);
			}
			tree_free($2);
		}
	THEN
	{ gen_label(); }
	statement
	ELSE
	{ spew_jmp("jmp",1); gen_label(); }
	statement
	{ gen_label(); }

	| WHILE
	{ spew_jmp("jmp",1); }
	expression DO
	{ gen_label(); }
	 statement
	{ gen_label(); }
	{ gencode($3); gen_jmp($3, -2); tree_free($3); }
	;

variable
	: ID
		{ $$ = $1; }
	//| ID '[' expression ']'
	;

procedure_statement
	: ID
	| ID '(' expression_list ')'
		{
			tree_t * t = NULL;
			fprintf(stderr, "--->>> %s\n", $1);
			if (gen_write($1, $3));
			else if (gen_read($1, $3));
			else {
				t = make_tree(PROCEDURE_CALL, make_id(scope_searchall(top, $1)), $3);
				assert(!sem_check(t));
				gencode(t);
				tree_free(t);
			}
		}
	;

expression_list
	: expression
		{ $$ = make_tree(COMMA, NULL, $1); }
	| expression_list ',' expression
		{ $$ = make_tree(COMMA, $1, $3); }
	;

expression
	: simple_expression
		{ $$ = $1; }
	| simple_expression RELOP simple_expression
		{ $$  = make_op(RELOP, $2, $1, $3); }
	;

simple_expression
	: term
		{ $$ = $1; }
	/*| sign term*/
	| simple_expression ADDOP term
		{ $$  = make_op(ADDOP, $2, $1, $3); }
	;

term
	: factor
		{ $$ = $1; }
	| term MULOP factor
		{ $$ = make_op(MULOP, $2, $1, $3); }
	;

factor
	: ID
		{ $$ = make_id(scope_searchall(top, $1)); }
	| ID '(' expression_list ')'
		{ $$ = make_tree(FUNCTION_CALL, make_id(scope_searchall(top, $1)), $3); }
	| ID '[' expression_list ']'
		{ $$ = make_tree(ARRAY_ACCESS, make_id(scope_searchall(top, $1)), $3); }
	| INUM
		{ $$ = make_inum($1); }
	| RNUM
		{ $$ = make_rnum($1); }
	| '(' expression ')'
		{ $$ = $2; }
	| NOT factor
		{ $$ = make_tree(NOT, NULL, NULL); } /* Type alone can infer the op */
	;
 /*
sign
	: '+'
	| '-'
	;
 */

%%

int main(int argc, char ** argv)
{
	_verbose = (argc == 2) && !(strcmp(argv[1],"-v"));

	outsrc = fopen("loloutput.s", "w");

	yyparse();

	fclose(outsrc);

	return 0;
}

int yyerror(char * msg)
{
	fprintf(stderr, "Error on line %d: %s\n", linecount, msg);
	exit(1);
}
