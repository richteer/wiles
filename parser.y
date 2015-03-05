%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"

int _verbose = 0;

void yyerror(char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	exit(1);
}

int yywrap(void)
{
	return 1;
}

int main(int argc, char ** argv)
{
	_verbose = (argc == 2) && !(strcmp(argv[1],"-v"));

	yyparse();
}
%}

%token RELOP ASNOP
%token ADDOP MULOP

%token PROGRAM PROCEDURE FUNCTION
%token BEGINN END
%token ARRAY RANGE OF
%token IF THEN ELSE NOT
%token DO WHILE
%token VAR ID INTEGER REAL NUM

%%

program
	: PROGRAM ID '(' identifier_list ')' ';'
		declarations
		subprogram_declarations
		compound_statement
		'.'
	;

identifier_list
	: ID
	| identifier_list ',' ID
	;

declarations
	: declarations VAR identifier_list ':' type ';'
	|
	;

type
	: standard_type
	| ARRAY '[' NUM RANGE NUM ']' OF standard_type
	;

standard_type
	: INTEGER
	| REAL
	;

subprogram_declarations
	: subprogram_declarations subprogram_declaration ';'
	|
	;

subprogram_declaration
	: subprogram_head declarations compound_statement
	;

subprogram_head
	: FUNCTION ID arguments ':' standard_type ';'
	| PROCEDURE ID arguments ';'
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
	| procedure_statement
	| compound_statement
	| IF expression THEN statement ELSE statement
	| WHILE expression DO statement
	;

variable
	: ID
	| ID '[' expression ']'
	;

procedure_statement
	: ID
	| ID '(' expression_list ')'
	;

expression_list
	: expression
	| expression_list ',' expression
	;

expression
	: simple_expression
	| simple_expression RELOP simple_expression
	;

simple_expression
	: term
	| sign term
	| simple_expression ADDOP term
	;

term
	: factor
	| term MULOP factor
	;

factor
	: ID
	| ID '(' expression_list ')'
	| NUM
	| '(' expression ')'
	| NOT factor
	;

sign
	: '+'
	| '-'
	;


%%

