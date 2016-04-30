#include <stdio.h>
#include "../include/syntax_tree.h"
#include "../include/syntax.tab.h"
#include "../include/semanic.h"
extern FILE* yyin;
extern struct YYLTYPE;
extern YYLTYPE yylloc;
extern struct Node;
extern struct Node* root;
extern int errorA_num;
extern int errorB_num;
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
		// No error and print syntax tree.
		if (errorA_num == 0 && errorB_num == 0)
			//print_tree(root, 0);
			program(root);
	}
	return 0;
}

