#ifndef __SEMANIC_H__
#define __SEMANIC_H__

#include "syntax_tree.h"
#define HASHSIZE 100  // HashTable Size

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

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

uint32_t hash_pjw(char* name);

#endif