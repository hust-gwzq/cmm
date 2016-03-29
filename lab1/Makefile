default:
	flex lexical.l
	bison -d syntax.y
	gcc main.c syntax.tab.c -lfl -ly -o parser
lex:
	flex lexical.l
syntax:
	bison -d syntax.y
scanner:
	gcc main.c lex.yy.c -lfl -o scanner
compile:
	gcc main.c syntax.tab.c -lfl -ly -o parser
parse:
	./parser test.c
clean:
	rm lex.yy.c
	rm scanner
	rm parser
