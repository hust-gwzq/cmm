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
	bison -d $(SCRIPT)/syntax.y -o $(SCRIPT)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.c $(SRC)/syntax.tab.c
	mv $(SCRIPT)/syntax.tab.h $(INCLUDE)/syntax.tab.h
# 	use -w for disable some warning.
	$(CC) $(SRC)/main.c $(SRC)/semanic.c $(SRC)/syntax_tree.c $(SRC)/syntax.tab.c -lfl -w -o $(BIN)/parser
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
	$(PARSER)	testcase/test2_1.cmm
	$(PARSER)	testcase/test2_2.cmm
	$(PARSER)	testcase/test2_3.cmm
	$(PARSER)	testcase/test2_4.cmm
	$(PARSER)	testcase/test2_5.cmm
	$(PARSER)	testcase/test2_6.cmm
	$(PARSER)	testcase/test2_7.cmm
	$(PARSER)	testcase/test2_8.cmm
	$(PARSER)	testcase/test2_9.cmm
	$(PARSER)	testcase/test2_10.cmm
	$(PARSER)	testcase/test2_11.cmm
	$(PARSER)	testcase/test2_12.cmm
	$(PARSER)	testcase/test2_13.cmm
	$(PARSER)	testcase/test2_14.cmm
	$(PARSER)	testcase/test2_15.cmm
	$(PARSER)	testcase/test2_16.cmm
	$(PARSER)	testcase/test2_17.cmm
clean:
	rm $(SRC)/lex.yy.c $(SRC)/syntax.tab.c $(INCLUDE)/syntax.tab.h -f
	rm $(BIN)/parser -f
