##C-- Compiler Programming Assignment
--------------------------------------------------------------
###Lab1 Lexical Analysis and Syntax Analysis
####Folder structure

    Lab1
    │  Makefile
    │  
    │  README.md
    │  
    ├─include
    │      syntax_tree.h
    │      
    ├─script
    │      lexical.l
    │      syntax.y
    │      
    ├─src
    │      main.c
    │      syntax_tree.c
    │      
    └─testcase
            test1_1.cmm
            test1_2.cmm
            test1_3.cmm
            test1_4.cmm
            test1_5.cmm
            test1_6.cmm
            test1_7.cmm
            test1_8.cmm
            test1_9.cmm
            test1_10.cmm
            test1_11.cmm  # My own testcase for illegal id.
            test1_12.cmm  # My own testcase for one of illegal exp float.
####How to Compile
`make` will generate lex.yy.c and syntax.tab.* and compile to `parser`  
`make test` will run 12 testcases and display the result.
