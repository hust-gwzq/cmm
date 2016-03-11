%{
#include <math.h>
%}
letter  [a-zA-Z]
digit   [0-9]
number  {digit}+
ID      [a-zA-Z_][a-zA-Z0-9_]*

%%
{ID}        { printf("Identifier: %s\n", yytext); }
{number}    { printf("Number: %d\n", atoi(yytext)); }

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
