%{
#include "syntax_tree.h"
#include "lex.yy.c"

extern struct Node;
Node *root;
int error_num = 0;
%}

/* declared types */
%union
{
//	int type_int;
//	float type_float;
//	double type_double;
	struct Node *type_node;
}

/* declared tokens */
%token <type_node> INT
%token <type_node> FLOAT
%token <type_node> SEMI COMMA
%token <type_node> TYPE STRUCT ID
%token <type_node> LP RP LB RB LC RC
%token <type_node> RETURN IF ELSE WHILE
%token <type_node> ASSIGNOP PLUS MINUS STAR DIV
%token <type_node> AND OR NOT DOT RELOP

%type <type_node> Program ExtDefList ExtDef ExtDecList 
%type <type_node> Specifier StructSpecifier OptTag Tag
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%error-verbose

%%

/* High-level Definitions */
Program : ExtDefList { $$ = create_node("Program", "", true, @$.first_line, 1, $1); root = $$; }
;
ExtDefList : /* empty */{ $$ = create_node("", "", true, @$.first_line, 0); }
  | ExtDef ExtDefList { $$ = create_node("ExtDefList", "", true, @$.first_line, 2, $1, $2); }
;
ExtDef : Specifier ExtDecList SEMI { $$ = create_node("", "", true, @$.first_line, 3, $1, $2, $3); }
  | Specifier SEMI { $$ = create_node("ExtDef", "", true, @$.first_line, 2, $1, $2); }
  | Specifier FunDec CompSt { $$ = create_node("ExtDef", "", true, @$.first_line, 3, $1, $2, $3); }
  | Specifier error { error_num++; }
  | error SEMI { error_num++; }
;
ExtDecList : VarDec { $$ = create_node("ExtDecList", "", true, @$.first_line, 1, $1); }
  | VarDec COMMA ExtDecList { $$ = create_node("ExtDecList", "", true, @$.first_line, 3, $1, $2, $3); }
;

/* Specifiers */
Specifier : TYPE { $$ = create_node("Specifier", "", true, @$.first_line, 1, $1); }
  | StructSpecifier { $$ = create_node("Specifier", "", true, @$.first_line, 1, $1); }
;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = create_node("StructSpecifier", "", true, @$.first_line, 5, $1, $2, $3, $4, $5); }
  | STRUCT Tag { $$ = create_node("StructSpecifier", "", true, @$.first_line, 2, $1, $2); }
;
OptTag :  /* empty */{ $$ = create_node("", "", true, @$.first_line, 0); }
  | ID { $$ = create_node("OptTag", "", true, @$.first_line, 1, $1); }
;
Tag : ID { $$ = create_node("Tag", "", true, @$.first_line, 1, $1); }
;

/* Declarators */
VarDec : ID { $$ = create_node("VarDec", "", true, @$.first_line, 1, $1); }
  | VarDec LB INT RB { $$ = create_node("VarDec", "", true, @$.first_line, 4, $1, $2, $3, $4); }
  | VarDec LB error RB { error_num++; }
;
FunDec : ID LP VarList RP { $$ = create_node("FunDec", "", true, @$.first_line, 4, $1, $2, $3, $4); }
  | ID LP RP { $$ = create_node("FunDec", "", true, @$.first_line, 3, $1, $2, $3); }
  | ID error RP { error_num++; }
;
VarList : ParamDec COMMA VarList { $$ = create_node("VarList", "", true, @$.first_line, 3, $1, $2, $3); }
  | ParamDec { $$ = create_node("VarList", "", true, @$.first_line, 1, $1); }
;
ParamDec : Specifier VarDec { $$ = create_node("ParamDec", "", true, @$.first_line, 2, $1, $2); }
;

/* Statements */
CompSt : LC DefList StmtList RC { $$ = create_node("CompSt", "", true, @$.first_line, 4, $1, $2, $3, $4); }
;
StmtList : /* empty */{ $$ = create_node("", "", true, @$.first_line, 0); }
  | Stmt StmtList { $$ = create_node("StmtList", "", true, @$.first_line, 2, $1, $2); }
;
Stmt : Exp SEMI { $$ = create_node("Stmt", "", true, @$.first_line, 2, $1, $2); }
  | CompSt { $$ = create_node("Stmt", "", true, @$.first_line, 1, $1); }
  | RETURN Exp SEMI { $$ = create_node("Stmt", "", true, @$.first_line, 3, $1, $2, $3); }
  | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = create_node("Stmt", "", true, @$.first_line, 5, $1, $2, $3, $4, $5); }
  | IF LP Exp RP Stmt ELSE Stmt { $$ = create_node("Stmt", "", true, @$.first_line, 7, $1, $2, $3, $4, $5, $6, $7); }
  | WHILE LP Exp RP Stmt { $$ = create_node("Stmt", "", true, @$.first_line, 5, $1, $2, $3, $4, $5); }
  | Exp error { error_num++; }
  | RETURN Exp error { error_num++; }
  | error SEMI { error_num++; }
;

/*Local Definitions*/
DefList : /* empty */{ $$ = create_node("", "", true, @$.first_line, 0); }
  | Def DefList { $$ = create_node("DefList", "", true, @$.first_line, 2, $1, $2); }
;
Def : Specifier DecList SEMI { $$ = create_node("Def", "", true, @$.first_line, 3, $1, $2, $3); }
;
DecList : Dec { $$ = create_node("DecList", "", true, @$.first_line, 1, $1); }
  | Dec COMMA DecList { $$ = create_node("DecList", "", true, @$.first_line, 3, $1, $2, $3); }
 ;
Dec : VarDec { $$ = create_node("Dec", "", true, @$.first_line, 1, $1); }
  | VarDec ASSIGNOP Exp { $$ = create_node("Dec", "", true, @$.first_line, 3, $1, $2, $3); }
;

/* Expressions*/
Exp : Exp ASSIGNOP Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp AND Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp OR Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp RELOP Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp PLUS Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp MINUS Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp STAR Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp DIV Exp { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | LP Exp RP { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | MINUS Exp { $$ = create_node("Exp", "", true, @$.first_line, 2, $1, $2); }
  | NOT Exp { $$ = create_node("Exp", "", true, @$.first_line, 2, $1, $2); }
  | ID LP Args RP { $$ = create_node("Exp", "", true, @$.first_line, 4, $1, $2, $3, $4); }
  | ID LP RP { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp LB Exp RB { $$ = create_node("Exp", "", true, @$.first_line, 4, $1, $2, $3, $4); }
  | Exp DOT ID { $$ = create_node("Exp", "", true, @$.first_line, 3, $1, $2, $3); }
  | ID { $$ = create_node("Exp", "", true, @$.first_line, 1, $1); }
  | INT { $$ = create_node("Exp", "", true, @$.first_line, 1, $1); }
  | FLOAT { $$ = create_node("Exp", "", true, @$.first_line, 1, $1); }
  | Exp LB error RB { error_num++; }
;
Args : Exp COMMA Args { $$ = create_node("Args", "", true, @$.first_line, 3, $1, $2, $3); }
  | Exp { $$ = create_node("Args", "", true, @$.first_line, 1, $1); }
;
%%
