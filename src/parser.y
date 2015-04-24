%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "scope.h"
#include "semantic.h"
#include "parser.tab.h"

extern scope_t * top;

int _verbose = 0;

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
%token ARRAY_ACCESS

%token <sval> ID
%token <ival> INUM
%token <rval> RNUM

%type <tval> expression
%type <tval> expression_list
%type <tval> identifier_list
%type <tval> simple_expression
%type <tval> term
%type <tval> factor
%type <ival> standard_type
%type <ival> type
%type <sval> variable;

%%

program
	: { top = scope_push(top);  }
		PROGRAM ID '(' identifier_list ')' ';'
		declarations
		subprogram_declarations
		compound_statement
		'.'
      { top = scope_pop(top); }
	;

identifier_list
	: ID
    	{ $$ = make_id(scope_insert(top, $1)); }
	| identifier_list ',' ID
    	{ $$ = make_tree(COMMA, $1, make_id(scope_insert(top, $3))); }
	;

declarations
	: declarations VAR identifier_list ':' type ';'
		{ typeify($3, $5); }
	|
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
	: subprogram_head declarations compound_statement
		{
			top = scope_pop(top);
		}
	;

subprogram_head
	: FUNCTION ID
			{  scope_insert(top, $2); top = scope_push(top); }
		arguments ':' standard_type ';'
	| PROCEDURE ID
			{ top = scope_push(top); }
		arguments ';'
	;

arguments
	: '(' parameter_list ')'
	|
	;

parameter_list
	: identifier_list ':' type
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
		{ tree_print($3); assert(!sem_check($3)); gencode($3); }
	| procedure_statement
	| compound_statement
	| IF expression THEN statement ELSE statement
	| WHILE expression DO statement
	;

variable
	: ID
		{ $$ = scope_search(top, $1); }
	//| ID '[' expression ']'
	;

procedure_statement
	: ID
	| ID '(' expression_list ')'
		{ assert(!sem_check($3)); }
	;

expression_list
	: expression
		{ $$ = $1; }
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

	gen_preamble();
	yyparse();
	gen_postamble();

	fclose(outsrc);
}

int yyerror(char * msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	exit(1);
}
