##C-- Compiler Programming Assignment
--------------------------------------------------------------
###Lab2 Semanic Analysis
####Folder structure

    Lab2
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
            test2_1.cmm
            test2_2.cmm
            test2_3.cmm
            test2_4.cmm
            test2_5.cmm
            test2_6.cmm
            test2_7.cmm
            test2_8.cmm
            test2_9.cmm
            test2_10.cmm
            test2_11.cmm
            test2_12.cmm
            test2_13.cmm
            test2_14.cmm
            test2_15.cmm
            test2_16.cmm
            test2_17.cmm
####How to Compile
`make` will generate lex.yy.c and syntax.tab.* and compile to `parser`  
`make test will run 17 testcases and display the result.
