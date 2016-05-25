##C-- Compiler Programming Assignment
--------------------------------------------------------------
###Lab3 Intercode Generate
####Folder structure

    Lab3
    │  Makefile
    │  
    │  README.md
    │  
    ├─include
    │      syntax_tree.h
    │      semanic.h
    ├─script
    │      lexical.l
    │      syntax.y
    │      
    ├─src
    │      main.c
    │      syntax_tree.c
    |      semanic.c
    │      
    └─testcase
            test3_1.cmm
            test3_2.cmm
####How to Compile
`make` will generate lex.yy.c and syntax.tab.* and compile to `parser`  
`make` test will run 2 testcases and display the result.
