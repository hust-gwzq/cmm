#ifndef __INTERCODE_H__
#define __INTERCODE_H__

#include "syntax_tree.h"
#include "semanic.h"
#include <stdio.h>

// typedef for char*.
typedef char* string;

struct Operand
{
    // use ARRAY_ because previous defined.
    enum { VARIABLE, CONSTANT, ADDRESS, ARRAY_ } kind;
    union
    {
        int var_no;
        int value;
        string name;
    } u;
};

typedef struct Operand Operand;

struct InterCode
{
    enum
    {
        ASSIGN_IC,
        ADD_IC,
        SUB_IC,
        MUL_IC,
        DIV_IC,
        MINUS_IC,
        RETURN_IC,
        LABEL_IC,
        LABEL_GOTO_IC,
        READ_IC,
        WRITE_IC,
        CALLFUNC_IC,
        FUNCTION_IC,
        ARG_IC,
        PARAM_IC,
        REFASSIGN_IC,
        DEC_IC,
        COND_IC
    } kind;

    union
    {
        struct
        {
            string name;
        } function;
        struct
        {
            string name;
        } param;
        struct
        {
            string name;
        } label;
        struct
        {
            string name;
        } label_goto;
        struct
        {
            Operand* operand;
            string name;
        } call;
        struct
        {
            Operand* operand;
        } ret;
        struct
        {
            Operand* operand;
        } write;
        struct
        {
            Operand* operand;
        } read;
        struct
        {
            Operand *operand;
        } arg;
        struct
        {
            Operand *operand;
            int size;
        } dec;
        struct
        {
            Operand *result;
        } sinop;
        struct
        {
            Operand *result;
			Operand *operand;
        } binop;
        struct
        {
            Operand *result;
			Operand *operand1;
			Operand *operand2;
        } triop;
        struct
        {
            Operand *operand1;
			Operand *operand2;
            string op;
			string *name;
        } cond;
    } u;
};

typedef struct InterCode InterCode;

// Use double direct linked list.
struct InterCodes
{
    struct InterCode* code;
    struct InterCodes* prev;
    struct InterCodes* next;
};

typedef struct InterCodes InterCodes;

int getTypeSize(Type type);
// link a node in the double linked list.
InterCodes* linkNode(InterCodes* interCodes1, InterCodes* interCodes2);
void generateCode(FILE* file, InterCode* interCode);
void printInFile(string fileName);

void translateCond(Node* node, string, string);
//Function.
void translateProgram(struct Node* node);
void translateExtDefList(struct Node* node);
void translateExtDef(struct Node* node);
void translateExtDecList(struct Node* node);

int translateSpecifier(struct Node* node);
void translateStructSpecifier(struct Node* node);
void translateOptTag(struct Node* node);
void translateTag(struct Node* node);

string translateVarDec(struct Node* node, int size);
void translateFunDec(struct Node* node);
void translateVarList(struct Node* node);
void translateParamDec(struct Node* node);

void translateCompSt(struct Node* node);
void translateStmtList(struct Node* node);
void translateStmt(struct Node* node);

void translateDefList(struct Node* node);
void translateDef(struct Node* node);
void translateDecList(struct Node* node, int size);
void translateDec(struct Node* node, int size);

void translateExp(struct Node* node, Operand* operand);
void translateArgs(struct Node* node, Operand** args);

#endif
