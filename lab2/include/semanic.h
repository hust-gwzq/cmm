#ifndef __SEMANIC_H__
#define __SEMANIC_H__

#include "syntax_tree.h"
#define HASHSIZE 15

// #define INT 0
// #define FLOAT 1
// #define ARRAY 2
// #define STRUCT 3
// #define FUNCTION 4
// #define ALLFIT 5
// #define VAR 6
// #define ERROR 7
// #define VALID 8
// #define FIELDLIST 9
// #define LOCAL 10
// #define BASIC 11

typedef struct Type_ Type_;
typedef struct FieldList_ FieldList_;
typedef struct Var_ Var_;
typedef struct Func_ Func_;
typedef struct Args_ Args_;
typedef struct Unit_ Unit_;
typedef struct ExpReturnType_ ExpReturnType_;

typedef Type_* Type;
typedef FieldList_* FieldList;
typedef Var_* Var;
typedef Func_* Func;
typedef Args_* Args;
typedef Unit_* Unit;
typedef ExpReturnType_* ExpReturnType;

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
	Type returnType;
	Args args;
};

struct Unit_
{
	char* name;
	int kind;
	union
	{
		Var var;
		Func func;
		FieldList fieldlist;
		Type type;
	} u;
	Unit next;
};

struct ExpReturnType_
{
	int kind;
	Type type;
	int flag;
};


unsigned int hash_pjw(char* name);

void init_hash_table();

void insert_hash_table(Unit unit);

int check_hash_table(char* name);

Unit get_unit(char* name);


//Function.
void program(struct Node* node);
void extDefList(struct Node* node);
void extDef(struct Node* node);
void extDecList(struct Node* node, Type type);

Type specifier(struct Node* node);
Type structSpecifier(struct Node* node);
Unit optTag(struct Node* node);
void tag(struct Node* node);

Var varDec(struct Node* node);
Func funDec(struct Node* node);
Args varList(struct Node* node);
Args paramDec(struct Node* node);

void compSt(struct Node* node);
void stmtList(struct Node* node);
void stmt(struct Node* node);

FieldList defList(struct Node* node, int);
FieldList def(struct Node* node, int);
FieldList decList(struct Node*, Type type, int);
FieldList dec(struct Node*, Type type, int);

ExpReturnType_ exp(struct Node* node);
int args(struct Node* node, Args arg);

#endif