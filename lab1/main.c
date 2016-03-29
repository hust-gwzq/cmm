#include <stdio.h>
#include "syntax_tree.h"
#include "syntax.tab.h"
extern FILE* yyin;
extern struct YYLTYPE;
extern YYLTYPE yylloc;
extern struct Node;
extern struct Node* root;
extern int error_num;
int yylex();
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
		yyparse();
	}
	return 0;
}

yyerror(char *msg)
{
	printf("Error type B at Line %d: %s\n", yylloc.first_line, msg);
}
