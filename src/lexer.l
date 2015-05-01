%{
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "parser.tab.h"

// Inspired by exm's macros
#define SHOW(...) _verbose&&fprintf(stderr,__VA_ARGS__)
#define DISP(a) _verbose&&fprintf(stderr,"[%s]",#a);return a
#define DISPY(a) _verbose&&fprintf(stderr,"[%s{%s}]",#a,yytext);return a

extern int _verbose;
int linecount = 1;
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

comment     \(\*.*\*\)

%%


 /* Relation Operators */
"="     { yylval.opval = EQ; DISPY(RELOP); }
"<>"    { yylval.opval = NE; DISPY(RELOP); }
"<"     { yylval.opval = LT; DISPY(RELOP); }
"<="    { yylval.opval = LE; DISPY(RELOP); }
">="    { yylval.opval = GE; DISPY(RELOP); }
">"     { yylval.opval = GT; DISPY(RELOP); }

 /* Additive Operators */
"+"     { yylval.opval = PLUS;  DISPY(ADDOP); }
"-"     { yylval.opval = MINUS; DISPY(ADDOP); }
"or"    { yylval.opval = OR;    DISPY(ADDOP); }

 /* Multiplicative Operators */
"*"     { yylval.opval = STAR;  DISPY(MULOP); }
"/"     { yylval.opval = SLASH; DISPY(MULOP); }
"div"   { yylval.opval = DIV;   DISPY(MULOP); }
"mod"   { yylval.opval = MOD;   DISPY(MULOP); }
"and"   { yylval.opval = AND;   DISPY(MULOP); }

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
"for"       { DISP(FOR); }
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
{comment}    { ; }
\n           { SHOW("%s", yytext); linecount++; }
.            { SHOW("<%c>", yytext[0]); return yytext[0]; }

%%
