%{
#include "lex.yy.c"
%}

/* declared types */
%union
{
	int type_int;
	float type_float;
	double type_double;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%%
Calc :  /* empty */
  | Exp { printf("= %d\n", $1); }
  ;
Exp : Factor
  | Exp ADD Factor { $$ = $1 + $3; }
  | Exp SUB Factor { $$ = $1 - $3; }
  ;
Factor : Term
  | Factor MUL Term { $$ = $1 * $3; }
  | Factor DIV Term { $$ = $1 / $3; }
  ;
Term : INT
  ;
%%
