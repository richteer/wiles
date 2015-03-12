%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "tree.h"

// Inspired by exm's macros
#define DISP(a) _verbose&&fprintf(stderr,"[%s]",#a);return a
#define DISPY(a) _verbose&&fprintf(stderr,"[%s{%s}]",#a,yytext);return a

extern int _verbose;

%}

whitespace  [ \t]+

letter      [a-zA-Z]

digit       [0-9]
digits      {digit}+
frac        \.{digits}
exp         E(\+|\-)?{digits}
num         {digits}{frac}?{exp}?
real        {digits}{frac}{exp}
int         {digits}

id	        {letter}({letter}|{digit})*

%%


 /* Relation Operators */
"="     { DISPY(RELOP); }
"<>"    { DISPY(RELOP); }
"<"     { DISPY(RELOP); }
"<="    { DISPY(RELOP); }
">="    { DISPY(RELOP); }
">"     { DISPY(RELOP); }

 /* Additive Operators */
"+"     { DISPY(ADDOP); }
"-"     { DISPY(ADDOP); }
"or"    { DISPY(ADDOP); }

 /* Multiplicative Operators */
"*"     { DISPY(MULOP); }
"/"     { DISPY(MULOP); }
"div"   { DISPY(MULOP); }
"mod"   { DISPY(MULOP); }
"and"   { DISPY(MULOP); }

 /* The rest */
":="    { DISP(ASNOP); }

"procedure" { DISP(PROCEDURE); }
"function"  { DISP(FUNCTION); }
"integer"   { DISP(INTEGER); }
"program"   { DISP(PROGRAM); }
"begin"     { DISP(BEGINN); }
"array"     { DISP(ARRAY); }
"while"     { DISP(WHILE); }
"else"      { DISP(ELSE); }
"real"      { DISP(REAL); }
"then"      { DISP(THEN); }
"end"       { DISP(END); }
"not"       { DISP(NOT); }
"var"       { DISP(VAR); }
"if"        { DISP(IF); }
"do"        { DISP(DO); }
"of"        { DISP(OF); }

".."        { DISP(RANGE); }

{int}       { yylval.ival = atoi(yytext);   DISPY(INUM); }
{real}      { yylval.rval = atof(yytext);   DISPY(RNUM); }
{id}        { yylval.sval = strdup(yytext); DISPY(ID);    }

{whitespace} { ; }
\n           { fprintf(stderr, "%s", yytext); }
.            { fprintf(stderr, "<%c>", yytext[0]); return yytext[0]; }

%%


