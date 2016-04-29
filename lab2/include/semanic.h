#ifndef __SEMANIC_H__
#define __SEMANIC_H__

#include "syntax_tree.h"
#define HASHSIZE 100  // HashTable Size

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Var_* Var;
typedef struct Args_* Args;
typedef struct Func_* Func;
typedef struct Item_* Item;
typedef struct ExpReturnType_* ExpReturnType;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
    } u;
};

struct FieldList_
{
    char* name;
    Type type;
    FieldList tail;
};

struct Var_
{
    char* name;
    Type type;
};

struct Args_
{
    Var var;
    Args next;
};

struct Func
{
    char* name;
    Type return_value;
    Args args;
};

uint32_t hash_pjw(char* name);

#endif