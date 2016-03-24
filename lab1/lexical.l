%{
#include "syntax.tab.h"

int yycolumn = 1;
#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;
%}
int_number				(0|([1-9][0-9]*))
normal_float_number		([0-9]*\.?[0-9]+|[0-9]+\.)
exp_float_number		[0-9]+(\.[0-9]+)?([Ee][+-]?[0-9]+)
id						[a-zA-Z_][a-zA-Z0-9_]*
type					int|float
comment1				(\/\/[^\r\n]*)+
comment2				"/*"([*]*(([^*/])+([/])*)*)*"*/"
%option yylineno
%%
[ \t]+      			{ }
\n          			{ yylineno = yylineno + 1; }
{type}      			{ printf("Line %d: Type\n", yylineno); return TYPE; }
{id}        			{ printf("Line %d: Identifier: %s\n", yylineno, yytext); return ID; }
{int_number}   			{ printf("Line %d: Integer: %d\n", yylineno, atoi(yytext)); return INT; }
{normal_float_number}	{ printf("Line %d: Float: %f\n", yylineno, atof(yytext)); return FLOAT; }
";"         			{ printf("Line %d: SEMI\n", yylineno); return SEMI; }
","         			{ printf("Line %d: COMMA\n", yylineno); return COMMA; }
"="         			{ printf("Line %d: ASSIGNOP\n", yylineno); return ASSIGNOP; }
"+"         			{ printf("Line %d: PLUS\n", yylineno); return PLUS; }
"-"         			{ printf("Line %d: SUB\n", yylineno); return SUB; }
"*"         			{ printf("Line %d: STAR\n", yylineno); return STAR; }
"/"         			{ printf("Line %d: DIV\n", yylineno); return DIV; }
"&&"        			{ printf("Line %d: AND\n", yylineno); return AND; }
"||"        			{ printf("Line %d: OR\n", yylineno); return OR; }
"."         			{ printf("Line %d: DOT\n", yylineno); return DOT; }
"!"         			{ printf("Line %d: NOT\n", yylineno); return NOT; }
"("         			{ printf("Line %d: LP\n", yylineno); return LP; }
")"         			{ printf("Line %d: RP\n", yylineno); return RP; }
"["         			{ printf("Line %d: LB\n", yylineno); return LB; }
"]"         			{ printf("Line %d: RB\n", yylineno); return RB; }
"{"         			{ printf("Line %d: LC\n", yylineno); return LC; }
"}"         			{ printf("Line %d: RC\n", yylineno); return RC; }
struct					{ printf("Line %d: STRUCT\n", yylineno); return STRUCT; }
return					{ printf("Line %d: RETURN\n", yylineno); return RETURN; }
if						{ printf("Line %d: IF\n", yylineno); return IF; }
else					{ printf("Line %d: ELSE\n", yylineno); return ELSE; }
while					{ printf("Line %d: WHILE\n", yylineno); return WHILE; }
{comment1}|{comment2}	{ }
.           			{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
