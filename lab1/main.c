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
		// No error and print syntax tree.
		if (error_num == 0)
			print_tree(root, 0);
	}
	return 0;
}

