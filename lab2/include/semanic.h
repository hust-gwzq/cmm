#ifndef __SEMANIC_H__
#define __SEMANIC_H__

#include "syntax_tree.h"
#include "hash_table.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Var_* Var;
typedef struct Args_* Args;
typedef struct Func_* Func;
typedef struct Unit_* Unit;
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

struct Func_
{
    char* name;
    Type return_value;
    Args args;
};

struct Unit_
{
    char* name;
    int kind;
    union
    {
        Type type;
        Var var;
        Func func;
        FieldList field_list;
    } u;
    Unit next;
};

struct ExpReturnType_
{
    int kind;
    Type type;
    int flag;    
};

uint32_t hash_pjw(char* name);


#endif