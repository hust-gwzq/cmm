%{
#include "../include/syntax_tree.h"
#include "../include/syntax.tab.h"
int errorA_num = 0;
int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

%}
int_number				(0|([1-9][0-9]*))
oct_int_number			0[0-7]+
hex_int_number			0[xX][0-9a-fA-F]+
normal_float_number		([0-9]*\.?[0-9]+|[0-9]+\.)
exp_float_number		[0-9]+(\.[0-9]+)?([Ee][+-]?[0-9]+)
id						[a-zA-Z_][a-zA-Z0-9_]*
type					int|float
comment1				(\/\/[^\r\n]*)+
comment2				"/*"([*]*(([^])+([/])*)*)*"*/"

wrong_id				[0-9]+[a-zA-Z_]+{id}*
wrong_oct				0([0-7]*[8-9]+[0-7]*)+
wrong_hex				0[Xx]([0-9a-fA-F]*[g-zG-Z]+[0-9a-fA-F]*)+
wrong_exp_float			([0-9]+(.[0-9]+)?([Ee][+-]?))|([Ee][+-]?[0-9]+)

%option yylineno
%%
[ \t]+      			{ }
struct					{ /* printf("Line %d: STRUCT\n", yylineno); */ yylval.type_node = create_node("STRUCT", yytext, true, yylineno, 0); return STRUCT; }
return					{ /* printf("Line %d: RETURN\n", yylineno); */ yylval.type_node = create_node("RETURN", yytext, true, yylineno, 0); return RETURN; }
if						{ /* printf("Line %d: IF\n", yylineno); */ yylval.type_node = create_node("IF", yytext, true, yylineno, 0); return IF; }
else					{ /* printf("Line %d: ELSE\n", yylineno); */ yylval.type_node = create_node("ELSE", yytext, true, yylineno, 0); return ELSE; }
while					{ /* printf("Line %d: WHILE\n", yylineno); */ yylval.type_node = create_node("WHILE", yytext, true, yylineno, 0); return WHILE; }
{type}      			{ /* printf("Line %d: Type\n", yylineno); */ yylval.type_node = create_node("TYPE", yytext, true, yylineno, 0); return TYPE; }
{id}        			{ /* printf("Line %d: Identifier: %s\n", yylineno, yytext); */ yylval.type_node = create_node("ID", yytext, true, yylineno, 0); return ID; }
{wrong_id}				{ printf("Error type A at Line %d: Wrong id \'%s\'.\n", yylineno, yytext); yylval.type_node = create_node("ID", yytext, true, yylineno, 0); errorA_num++; return ID; }
{wrong_oct}				{ printf("Error type A at Line %d: Illegal octal number \'%s\'.\n", yylineno, yytext); yylval.type_node = create_node("INT", yytext, true, yylineno, 0); errorA_num++; return INT; }
{wrong_hex}				{ printf("Error type A at Line %d: Illegal hexical number \'%s\'.\n", yylineno, yytext); yylval.type_node = create_node("INT", yytext, true, yylineno, 0); errorA_num++; return INT; }
{wrong_exp_float}		{ printf("Error type A at Line %d: Illegal floating point number \'%s\'.\n", yylineno, yytext); yylval.type_node = create_node("FLOAT", yytext, true, yylineno, 0); errorA_num++; return FLOAT; }
{int_number}   			{ /* printf("Line %d: Integer: %d\n", yylineno, atoi(yytext)); */ yylval.type_node = create_node("INT", yytext, true, yylineno, 0); return INT; }
{oct_int_number}		{ /* printf("Line %d: OctInteger: %d\n", yylineno, atoi(yytext)); */ yylval.type_node = create_node("INT", yytext, true, yylineno, 0); return INT; }
{hex_int_number}		{ /* printf("Line %d: HexInteger: %d\n", yylineno, atoi(yytext)); */ yylval.type_node = create_node("INT", yytext, true, yylineno, 0); return INT; }
{normal_float_number}	{ /* printf("Line %d: Float: %f\n", yylineno, atof(yytext)); */ yylval.type_node = create_node("FLOAT", yytext, true, yylineno, 0); return FLOAT; }
{exp_float_number}		{ /* printf("Line %d: ExpFloat: %f\n", yylineno, atof(yytext)); */ yylval.type_node = create_node("FLOAT", yytext, true, yylineno, 0); return FLOAT; }
";"         			{ /* printf("Line %d: SEMI\n", yylineno); */ yylval.type_node = create_node("SEMI", yytext, true, yylineno, 0); return SEMI; }
","         			{ /* printf("Line %d: COMMA\n", yylineno); */ yylval.type_node = create_node("COMMA", yytext, true, yylineno, 0); return COMMA; }
"="         			{ /* printf("Line %d: ASSIGNOP\n", yylineno); */ yylval.type_node = create_node("ASSIGNOP", yytext, true, yylineno, 0); return ASSIGNOP; }
>|<|>=|<=|==|!=			{ /* printf("Line %d: RELOP\n", yylineno); */ yylval.type_node = create_node("RELOP", yytext, true, yylineno, 0); return RELOP; }
"+"         			{ /* printf("Line %d: PLUS\n", yylineno); */ yylval.type_node = create_node("PLUS", yytext, true, yylineno, 0); return PLUS; }
"-"         			{ /* printf("Line %d: MINUS\n", yylineno); */ yylval.type_node = create_node("MINUS", yytext, true, yylineno, 0); return MINUS; }
"*"         			{ /* printf("Line %d: STAR\n", yylineno); */ yylval.type_node = create_node("STAR", yytext, true, yylineno, 0); return STAR; }
"/"         			{ /* printf("Line %d: DIV\n", yylineno); */ yylval.type_node = create_node("DIV", yytext, true, yylineno, 0); return DIV; }
"&&"        			{ /* printf("Line %d: AND\n", yylineno); */ yylval.type_node = create_node("AND", yytext, true, yylineno, 0); return AND; }
"||"        			{ /* printf("Line %d: OR\n", yylineno); */ yylval.type_node = create_node("OR", yytext, true, yylineno, 0); return OR; }
"."         			{ /* printf("Line %d: DOT\n", yylineno); */ yylval.type_node = create_node("DOT", yytext, true, yylineno, 0); return DOT; }
"!"         			{ /* printf("Line %d: NOT\n", yylineno); */ yylval.type_node = create_node("NOT", yytext, true, yylineno, 0); return NOT; }
"("         			{ /* printf("Line %d: LP\n", yylineno); */ yylval.type_node = create_node("LP", yytext, true, yylineno, 0); return LP; }
")"         			{ /* printf("Line %d: RP\n", yylineno); */ yylval.type_node = create_node("RP", yytext, true, yylineno, 0); return RP; }
"["         			{ /* printf("Line %d: LB\n", yylineno); */ yylval.type_node = create_node("LB", yytext, true, yylineno, 0); return LB; }
"]"         			{ /* printf("Line %d: RB\n", yylineno); */ yylval.type_node = create_node("RB", yytext, true, yylineno, 0); return RB; }
"{"         			{ /* printf("Line %d: LC\n", yylineno); */ yylval.type_node = create_node("LC", yytext, true, yylineno, 0); return LC; }
"}"         			{ /* printf("Line %d: RC\n", yylineno); */ yylval.type_node = create_node("RC", yytext, true, yylineno, 0); return RC; }
{comment1}|{comment2}	{ /* printf("Line %d: COMMENT\n", yylineno); */ }
\n          			{ yycolumn = 1; }
.           			{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); errorA_num++; }
%%
