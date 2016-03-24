%{
#include <math.h>
#include "syntax.tab.h"
%}
letter  [a-zA-Z]
digit   [0-9]
number  {digit}+
TYPE    int|float
ID      [a-zA-Z_][a-zA-Z0-9_]*
INTEGER [-+]?[1-9][0-9]* 
FLOAT   [-+]?[0-9]+\.[0-9]*
comment1 (\/\/[^\r\n]*)+
comment2 "/*"([*]*(([^*/])+([/])*)*)*"*/"
%option yylineno
%%
[ \t]+      { }
\n          { yylineno = yylineno + 1; }
{TYPE}      { printf("Line %d: Type\n", yylineno); return TYPE; }
{ID}        { printf("Line %d: Identifier: %s\n", yylineno, yytext); return Identifier; }
{INTEGER}   { printf("Line %d: Integer: %d\n", yylineno, atoi(yytext)); return Integer; }
{FLOAT}     { printf("Line %d: Float: %f\n", yylineno, atof(yytext)); return Float; }
";"         { printf("Line %d: SEMI\n", yylineno); return SEMI; }
","         { printf("Line %d: COMMA\n", yylineno); return COMMA; }
"="         { printf("Line %d: ASSIGNOP\n", yylineno); return ASSIGNOP; }
"+"         { printf("Line %d: PLUS\n", yylineno); return PLUS; }
"-"         { printf("Line %d: SUB\n", yylineno); return SUB; }
"*"         { printf("Line %d: STAR\n", yylineno); return STAR; }
"/"         { printf("Line %d: DIV\n", yylineno); return DIV; }
"&&"        { printf("Line %d: AND\n", yylineno); return AND; }
"||"        { printf("Line %d: OR\n", yylineno); return OR; }
"."         { printf("Line %d: DOT\n", yylineno); return DOT; }
"!"         { printf("Line %d: NOT\n", yylineno); return NOT; }
"("         { printf("Line %d: LP\n", yylineno); return LP; }
")"         { printf("Line %d: RP\n", yylineno); return RP; }
"["         { printf("Line %d: LB\n", yylineno); return LB; }
"]"         { printf("Line %d: RB\n", yylineno); return RB; }
"{"         { printf("Line %d: LC\n", yylineno); return LC; }
"}"         { printf("Line %d: RC\n", yylineno); return RC; }
\/\/.*      { }
{comment1}|{comment2}	{ }
 /* \/\*(.*\n)*.*\*\/)  { } */
.           { printf("Line %d: ERROR\n", yylineno); }
%%
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
}
