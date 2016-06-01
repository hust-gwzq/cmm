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
		// use _ to prevent the enum conflicts	
        ASSIGN_,
        ADD_,
        SUB_,
        MUL_,
        DIV_,
        MINUS_,
        RETURN_,
        LABEL_,
        LABEL_GOTO_,
        READ_,
        WRITE_,
        CALLFUNC_,
        FUNCTION_,
        ARG_,
        PARAM_,
        REFASSIGN_,
        DEC_,
        COND_
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
string newLabel();
Operand* newOperand();
Operand* newVar();
InterCode* newInterCode();
InterCodes* insertLink(InterCode *interCode);

void translateArray(Node* node, Type type, Type tp, Operand* last, Operand* temp);

// translate functions
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
void translateArgs(struct Node* node, Operand** args);
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
void translateCond(Node* node, string, string);

// generateCode and output to file
void generateCode(FILE* file, InterCode* interCode);
void outputFile(string fileName);

#endif
