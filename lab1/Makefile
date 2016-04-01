# CC can be gcc or clang.
# clang can make more useful error message than gcc.
# use gcc for submit and test.
CC		= gcc
#CC		= clang
SCRIPT	= ./script
SRC		= ./src
INCLUDE = ./include
BIN		= ./bin
PARSER	= ./bin/parser

all:
	flex -o $(SRC)/lex.yy.c $(SCRIPT)/lexical.l
	bison -d $(SCRIPT)/syntax.y
	mv $(SCRIPT)/syntax.tab.c $(SRC)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.h $(INCLUDE)/syntax.tab.h
# 	use -w for disable some warning.
	$(CC) $(SRC)/main.c $(SRC)/syntax_tree.c $(SRC)/syntax.tab.c -lfl -w -o $(BIN)/parser
lex:
#	flex lexical.l
	flex $(SCRIPT)/lexical.l
syntax:
	bison -d $(SCRIPT)/syntax.y
	mv $(SCRIPT)/syntax.tab.c $(SRC)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.h $(INCLUDE)/syntax.tab.h
compile:
	$(CC) $(SRC)/main.c $(SRC)/syntax_tree.c $(SRC)/syntax.tab.c -lfl -o $(BIN)/parser
test:
	$(PARSER)	testcase/test1_1.cmm
	$(PARSER)	testcase/test1_2.cmm
	$(PARSER)	testcase/test1_3.cmm
	$(PARSER)	testcase/test1_4.cmm
	$(PARSER)	testcase/test1_5.cmm
	$(PARSER)	testcase/test1_6.cmm
	$(PARSER)	testcase/test1_7.cmm
	$(PARSER)	testcase/test1_8.cmm
	$(PARSER)	testcase/test1_9.cmm
	$(PARSER)	testcase/test1_10.cmm
clean:
	rm $(SRC)/lex.yy.c $(SRC)/syntax.tab.c $(INCLUDE)/syntax.tab.h -f
	rm $(BIN)/parser -f
