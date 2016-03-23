%{
#include <math.h>
%}
letter  [a-zA-Z]
digit   [0-9]
number  {digit}+
TYPE    int|float
ID      [a-zA-Z_][a-zA-Z0-9_]*
INTEGER [-+]?[1-9][0-9]* 
FLOAT   [-+]?[0-9]+\.[0-9]*
%option yylineno
%%
[ \t]+      { }
\n          { yylineno = yylineno + 1; }
{TYPE}      { printf("Line %d: Type\n", yylineno); }
{ID}        { printf("Line %d: Identifier: %s\n", yylineno, yytext); }
{INTEGER}   { printf("Line %d: Integer: %d\n", yylineno, atoi(yytext)); }
{FLOAT}     { printf("Line %d: Float: %f\n", yylineno, atof(yytext)); }
";"         { printf("Line %d: SEMI\n", yylineno); }
","         { printf("Line %d: COMMA\n", yylineno); }
"="         { printf("Line %d: ASSIGNOP\n", yylineno); }
"+"         { printf("Line %d: PLUS\n", yylineno); }
"-"         { printf("Line %d: SUB\n", yylineno); }
"*"         { printf("Line %d: STAR\n", yylineno); }
"/"         { printf("Line %d: DIV\n", yylineno); }
"&&"        { printf("Line %d: AND\n", yylineno); }
"||"        { printf("Line %d: OR\n", yylineno); }
"."         { printf("Line %d: DOT\n", yylineno); }
"!"         { printf("Line %d: NOT\n", yylineno); }
"("         { printf("Line %d: LP\n", yylineno); }
")"         { printf("Line %d: RP\n", yylineno); }
"["         { printf("Line %d: LB\n", yylineno); }
"]"         { printf("Line %d: RB\n", yylineno); }
"{"         { printf("Line %d: LC\n", yylineno); }
"}"         { printf("Line %d: RC\n", yylineno); }
\/\/.*      { }
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
