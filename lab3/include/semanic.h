#ifndef __SEMANIC_H__
#define __SEMANIC_H__

#include "syntax_tree.h"

#define HASHSIZE 0x3fff

// TYPE
#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_ARRAY 2
#define TYPE_STRUCT 3
#define TYPE_FUNCTION 4
#define TYPE_ALL 5
#define TYPE_VAR 6
#define TYPE_ERROR 7
#define TYPE_VALID 8
#define TYPE_FIELDLIST 9
#define TYPE_LOCAL 10
#define TYPE_BASIC 11

typedef struct Type_ Type_;
typedef struct FieldList_ FieldList_;
typedef struct Var_ Var_;
typedef struct Func_ Func_;
typedef struct Args_ Args_;
typedef struct ReturnType_ ReturnType_;
typedef struct Unit_ Unit_;
// Top type
typedef Type_* Type;
typedef FieldList_* FieldList;
typedef Var_* Var;
typedef Func_* Func;
typedef Args_* Args;
typedef ReturnType_* ReturnType;
typedef Unit_* Unit;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        int basic;
        struct
        {
            Type elem;
            int size;
        } array;
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
    Args next;  // ptr to next arg
};

struct Func_
{
    char* name;
    Args args;
    Type return_type;  // return type

};
// expression return type.
struct ReturnType_
{
    int kind;
    Type type;
    int flag;
};

// hash table unit
struct Unit_
{
    char* name;
    int kind;
    union
    {
        Type type;
        FieldList fieldlist;
        Var var;
        Func func;
    } u;
    Unit next;
};

// hash table
uint32_t hash_pjw(char* name);
void init_hash_table();
void insert_hash_table(Unit unit);
int check_hash_table(char* name);
Unit get_unit(char* name);

// semanic analysis
void Program(struct Node* node);
void ExtDefList(struct Node* node);
void ExtDef(struct Node* node);
void ExtDecList(struct Node* node, Type type);
Type Specifier(struct Node* node);
Type StructSpecifier(struct Node* node);
Unit OptTag(struct Node* node);
Var VarDec(struct Node* node);
Func FunDec(struct Node* node);
int args(struct Node* node, Args arg);
Args VarList(struct Node* node);
Args ParamDec(struct Node* node);
void CompSt(struct Node* node);
void StmtList(struct Node* node);
void Stmt(struct Node* node);
// bool is_val or struct
// val = true
// struct = false
FieldList DefList(struct Node* node, bool is_val);
FieldList Def(struct Node* node, bool is_val);
FieldList DecList(struct Node*, Type type, bool is_val);
FieldList Dec(struct Node*, Type type, bool is_val);
ReturnType_ Exp(struct Node* node);

#endif
