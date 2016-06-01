# CC can be gcc or clang.
# clang can make more useful error message than gcc.
# use gcc for submit and test.
CC		= gcc
# CC		= clang
SCRIPT	= ./script
SRC		= ./src
INCLUDE = ./include
BIN		= ./bin
PARSER	= ./bin/parser
OUTPUT	= ./output

all:
	flex -o $(SRC)/lex.yy.c $(SCRIPT)/lexical.l
	bison -d $(SCRIPT)/syntax.y -o $(SCRIPT)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.c $(SRC)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.h $(INCLUDE)/syntax.tab.h
# 	use -w for disable some warning.
	$(CC) $(SRC)/main.c $(SRC)/intercode.c $(SRC)/semanic.c $(SRC)/syntax_tree.c $(SRC)/syntax.tab.c -lfl -w -o $(BIN)/parser
lex:
#	flex lexical.l
	flex $(SCRIPT)/lexical.l
syntax:
	bison -d $(SCRIPT)/syntax.y -o $(SCRIPT)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.c $(SRC)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.h $(INCLUDE)/syntax.tab.h
compile:
	$(CC) $(SRC)/main.c $(SRC)/syntax_tree.c $(SRC)/syntax.tab.c -lfl -o $(BIN)/parser
test:
	$(PARSER)	testcase/test3_1.cmm output/test3_1.ir
	$(PARSER)	testcase/test3_2.cmm output/test3_2.ir
clean:
	rm $(SRC)/lex.yy.c $(SRC)/syntax.tab.c $(INCLUDE)/syntax.tab.h -f
	rm $(BIN)/parser -f
	rm $(OUTPUT)/*.ir -f
