lex:
	flex lexical.l
syntax:
	bison -d syntax.y
compile:
	gcc main.c syntax.tab.c -lfl -ly -o parser
parse:
	./parser test.c
