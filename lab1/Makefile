default:
	flex lexical.l
	bison -d syntax.y
	gcc main.c syntax_tree.c syntax.tab.c -lfl -o parser
lex:
	flex lexical.l
syntax:
	bison -d syntax.y
scanner:
	clang main.c lex.yy.c -lfl -o scanner
compile:
	clang main.c syntax.tab.c -lfl -o parser
parse:
	./parser test.c
clean:
	rm lex.yy.c syntax.tab.h syntax.tab.c
	rm parser
