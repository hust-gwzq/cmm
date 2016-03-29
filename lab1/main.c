#include <stdio.h>
//#include "syntax.tab.h"
//#include "syntax_tree.h"
extern FILE* yyin;
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
	}
	while (yylex() != 0);
	return 0;
}
