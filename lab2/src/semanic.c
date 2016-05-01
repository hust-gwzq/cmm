#include "../include/semanic.h"

// hash table
Unit hash_table[HASHSIZE];

// type int
struct Type_ type_int;
// type float
struct Type_ type_float;

Type this = NULL;

// init hash table and type.
void init()
{
    init_hash_table();
    
    type_int.kind = BASIC;
    type_int.u.basic = TYPE_INT;
    
    type_float.kind = BASIC;
    type_float.u.basic = TYPE_FLOAT;
}

uint32_t hash_pjw(char* name)
{
    uint32_t val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASHSIZE) val = (val ^ (i >> 12)) & HASHSIZE;
    }
    return val;
}

void init_hash_table()
{
    uint32_t i = 0;
    for (i = 0; i < HASHSIZE; ++i)
    {
        hash_table[i] = (Unit)malloc(sizeof(struct Unit_));
        hash_table[i]->next = NULL;
    }
}

// linked list with hash table
void insert_hash_table(Unit unit)
{
    uint32_t index = hash_pjw(unit->name);

    Unit temp = hash_table[index];
    unit->next = temp->next;
    temp->next = unit;
}

// return value -1, 0, 1
// -1 for not exist
// 1 for exist
// NULL for not find in the hash linked list.
int check_hash_table(char* name)
{
    uint32_t index = hash_pjw(name);
    Unit p = hash_table[index];
    if (p == NULL)
        return -1;
    while (p != NULL)
    {
        if (p->name != 0 && strcmp(p->name, name) == 0)
            return 1;
        p = p->next;
    }
    // not find in the linked list.
    return NULL;
}

Unit get_unit(char* name)
{
    uint32_t index = hash_pjw(name);
    Unit temp = hash_table[index];
    while (temp != NULL)
    {
        if (temp->name != NULL && strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// top program function
void Program(struct Node* node)
{
    // initialize hash table
    init();
    
    ExtDefList(node->children[0]);
}

void ExtDefList(struct Node* node)
{
    if (node->children_num != 0)
    {
        ExtDef(node->children[0]);
        ExtDefList(node->children[1]);
    }
}

void ExtDef(struct Node* node)
{
    if (node->children_num == 2)
    {
        Specifier(node->children[0]);
    }
    else if (strcmp(node->children[1], "ExtDecList") == 0)
    {
        Type type = Specifier(node->children[0]);
        ExtDecList(node->children[1], type);
    }
    else
    {
        Type type = Specifier(node->children[0]);
        Func func = FunDec(node->children[1]);

        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = func->name;
        unit->kind = TYPE_FUNCTION;
        func->return_type = type;
        unit->u.func = func;

        insert_hash_table(unit);

        this = type;

        CompSt(node->children[2]);
    }
}

void ExtDecList(struct Node* node, Type type)
{
    if (node->children_num == 1)
    {
        Var var = VarDec(node->children[0]);
        if (check_hash_table(var->name) == 1)
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->name);
        else
        {
            Unit unit = (Unit)malloc(sizeof(Unit_));
            unit->name = var->name;
            unit->kind = TYPE_VAR;
            unit->u.var->type = type;
            unit->u.var = var;

            insert_hash_table(unit);
        }
    }
    else
    {
        //VarDec COMMA ExtDecList
        Var var = VarDec(node->children[0]);
        ExtDecList(node->children[2], type);

    }
}

Type Specifier(struct Node* node)
{
    if (strcmp(node->children[0]->name, "TYPE") == 0)
    {
        if (strcmp(node->children[0]->value, "int") == 0)
        {
            type_int.kind = BASIC;
            type_int.u.basic = TYPE_INT;
            return &type_int;
        }
        else
        {
            type_float.kind = BASIC;
            type_float.u.basic = TYPE_FLOAT;
            return &type_float;
        }
    }
    else
    {
        return StructSpecifier(node->children[0]);
    }
}

Type StructSpecifier(struct Node* node)
{
    // print_tree(node, 0);
    if (node->children_num == 5)
    {
        //STRUCT OptTag LC Defieldlistist RC
        Unit unit = OptTag(node->children[1]);

        unit->u.type->u.structure->tail = DefList(node->children[3], true);

        return unit->u.type;
    }
    else
    {
        //STRUCT Tag
        // print_tree(node, 0);
        // printf("tag: %s\n", node->children[1]->children[0]->value);
        Unit unit = get_unit(node->children[1]->children[0]->value);

        if (unit == NULL)
        {
            printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", node->children[1]->line_num, node->children[1]->children[0]->value);
            return NULL;
        }
        else
        {
            if (unit->kind == TYPE_ALL)
            {
                // printf("%d", unit->u.type->kind);
                return unit->u.type;
            }
                
            else
            {
                printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", node->children[1]->line_num), node->children[1]->children[0]->value;
                return NULL;
            }
        }
    }
}

Unit OptTag(struct Node* node)
{
    // print_tree(node, 0);
    if (node->children_num == 1)
    {
        // struct id is existed
        if (check_hash_table(node->children[0]->value) == 1)
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", node->children[0]->line_num, node->children[0]->value);

        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = node->children[0]->value;
        fieldlist->type = 0;
        fieldlist->tail = 0;
        Type type = (Type)malloc(sizeof(Type_));
        type->kind = STRUCTURE;
        type->u.structure = fieldlist;
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = fieldlist->name;
        unit->kind = TYPE_ALL;
        unit->u.type = type;
        // printf("%s");
        insert_hash_table(unit);

        return unit;
    }
    else
    {

        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = 0;
        fieldlist->type->kind = STRUCTURE;
        fieldlist->tail = 0;
        Type type = (Type)malloc(sizeof(Type_));
        type->kind = STRUCTURE;
        type->u.structure = fieldlist;
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = unit->u.type->u.structure->name;
        return unit;

    }
}

Var VarDec(struct Node* node)
{
    // print_tree(node, 0);
    if (node->children_num == 1)
    {
        //ID
        Var var = (Var)malloc(sizeof(Var_));
        var->name = node->children[0]->value;
        var->type = TYPE_INT;
        return var;
    }
    else
    {
        Var var = VarDec(node->children[0]);
        Type type = (Type)malloc(sizeof(Type_));
        type->kind == ARRAY;
        type->u.array.elem = var->type;
        type->u.array.size = strtol(node->children[2]->value, 0, TYPE_LOCAL);
        var->type = type;
        return var;
    }
}

Func FunDec(struct Node* node)
{
    // print_tree(node, 0);
    if (check_hash_table(node->children[0]->value) == 1)
        printf("Error type 4 at Line %d: Redefined function \"%s\"\n", node->children[0]->line_num,node->children[0]->value);
    if (node->children_num == 4)
    {
        Func func = (Func)malloc(sizeof(Func_));
        func->name = node->children[0]->value;
        func->return_type = NULL;
        func->args = VarList(node->children[2]);
        return func;
    }
    else
    {
        Func func = (Func)malloc(sizeof(Func_));
        func->name = node->children[0]->value;
        func->return_type = NULL;
        func->args = NULL;
        return func;
    }
}

int args(struct Node* node, Args arg)
{
    // print_tree(node, 0);
    ReturnType_ returntype = Exp(node->children[0]);

    if(node->children_num == 1)
    {
        if (returntype.kind == TYPE_ERROR)
            return NULL;
        else if (arg == NULL || returntype.type == arg->var->type)
        {
            //   print_tree(node, 0);
            printf("Error type 9 at Line %d: Function is not applicable for arguments.\n", node->children[0]->line_num);
            return NULL;
        }
        else
        {
            printf("Error type 9 at Line %d: Function is not applicable for arguments.\n", node->children[0]->line_num);
            return NULL;
        }
    }
    else
    {
        return args(node->children[2], arg->next);
    }
}

Args VarList(struct Node* node)
{
    // print_tree(node, 0);
    Args arg = ParamDec(node->children[0]);
    if (node->children_num == 1)
        return arg;
    else
    {
        arg->next = VarList(node->children[2]);
        return arg;
    }
}

Args ParamDec(struct Node* node)
{
    // print_tree(node, 0);
    Type type = Specifier(node->children[0]);
    Var var = VarDec(node->children[1]);
    if (var->type == 0)
        var->type = type;
    else
    {
        Type type2 = var->type;
        while (type2->u.array.elem != 0)
            type2 = type2->u.array.elem;
        type2->u.array.elem = type;
    }

    if (check_hash_table(var->name) == 1)
        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[1]->line_num, node->children[1]->name);

    Unit unit = (Unit)malloc(sizeof(Unit_));
    unit->name = var->name;
    unit->kind = TYPE_VAR;
    unit->u.var = var;

    insert_hash_table(unit);

    Args arg = (Args)malloc(sizeof(Args_));
    arg->var = var;
    arg->next = NULL;
    return arg;
}

void CompSt(struct Node* node)
{
    DefList(node->children[1], false);
    StmtList(node->children[2]);
}

void StmtList(struct Node* node)
{
    if (node->children_num != 0)
    {
        Stmt(node->children[0]);
        StmtList(node->children[1]);
    }
}

void Stmt(struct Node* node)
{
    // print_tree(node, 0);
    if (node->children_num == 2)
    {
        //Exp SEMI
        Exp(node->children[0]);
    }
    else if (node->children_num == 1)
    {
        //CompSt
        CompSt(node->children[0]);
    }
    else if(node->children_num == 3)
    {
        //RETURN Exp SEMI
        ReturnType_ returntype = Exp(node->children[1]);
        if (returntype.kind != TYPE_ERROR && this != returntype.type)
            printf("Error type 8 at Line %d: Type mismatched for return.\n", node->children[1]->line_num);

    }
    else if (node->children_num == TYPE_ERROR)
    {
        //IF LP Exp RP Stmt ELSE Stmt
        ReturnType_ returntype = Exp(node->children[2]);
        if (returntype.kind != TYPE_ERROR && returntype.type != &type_int)
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[0]->line_num);

        Stmt(node->children[4]);
        Stmt(node->children[6]);
    }
    else
    {
        Exp(node->children[2]);

        ReturnType_ returntype = Exp(node->children[2]);
        if (returntype.kind != TYPE_ERROR && returntype.type != &type_int)
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[0]->line_num);
        Stmt(node->children[4]);

    }
}

FieldList DefList(struct Node* node, bool is_val)
{
    // print_tree(node, 0);
    if (strcmp(node->name, "") == 0)
        return NULL;

    if (node->children_num == 0)
    {
        return NULL;
    }
    else
    {
        FieldList fieldlist = Def(node->children[0], is_val);

        if (!is_val)
            DefList(node->children[1], is_val);
        else
        {
            if (fieldlist != 0)
            {
                FieldList fieldlist = fieldlist;
                while (fieldlist->tail != 0)
                    fieldlist =fieldlist->tail;
                fieldlist->tail = DefList(node->children[1], is_val);
            }
        }
        return fieldlist;
    }
}

FieldList Def(struct Node* node, bool is_val)
{
    // print_tree(node, 0);
    Type type = Specifier(node->children[0]);
    FieldList fieldlist = DecList(node->children[1], type, is_val);

    return fieldlist;
}

FieldList DecList(struct Node* node, Type type, bool is_val)
{
    // print_tree(node, 0);
    FieldList fieldlist = Dec(node->children[0], 0, is_val);
    if (node->children_num == 1)
        return fieldlist;

    else
    {
        if (is_val)
            fieldlist->tail = DecList(node->children[2], type, is_val);
        else
        {
            DecList(node->children[2], type, is_val);
        }
        return fieldlist;
    }
}

FieldList Dec(struct Node* node, Type type, bool is_val)
{
    // print_tree(node, 0);
    Var var = VarDec(node->children[0]);
    if (check_hash_table(var->name) == 1)
    {
        if (is_val)
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n", node->children[0]->line_num, var->name);
        else
        {
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, var->name);
        }
    }

    var->type = type;

    FieldList fieldlist = NULL;

    if (is_val)
    {
        // insert in the hash table.
        fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = var->name;
        fieldlist->type = var->type;
        fieldlist->tail = NULL;

        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = fieldlist->name;
        unit->kind = TYPE_FIELDLIST;
        unit->u.fieldlist = fieldlist;

        insert_hash_table(unit);
    }
    else
    {
        // insert in the hash table.
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = var->name;
        unit->kind = TYPE_VAR;
        unit->u.var = var;

        insert_hash_table(unit);
    }

    if (node->children_num == 3)
    {
        // VarDec ASSIGNOP Exp
        if (is_val)
            printf("Error type 15 at Line %d: Redefined names in the structure.\n", node->children[0]->line_num);
        else
        {
            ReturnType_ returntype = Exp(node->children[2]);
        }
    }

    return fieldlist;
}

ReturnType_ Exp(struct Node* node)
{
    // print_tree(node, 0);
    
    ReturnType_ returntype;

    if (strcmp(node->children[0]->name, "Exp") == 0)
    {
        // printf("In the first is Exp\n");
        ReturnType_ returntype1 = Exp(node->children[0]);
        
        // printf("flag is %d\n", returntype1.flag);
        if (strcmp(node->children[1]->name, "ASSIGNOP") == 0)
        {
            if (returntype1.kind != TYPE_ERROR && returntype1.kind != TYPE_VAR)
            {
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->children[0]->line_num);
                returntype.kind = TYPE_ERROR;
                return returntype;
            }

            struct ReturnType_ returntype2 = Exp(node->children[2]);

            if (returntype1.kind == TYPE_ERROR || returntype2.kind == TYPE_ERROR)
                returntype.kind = TYPE_ERROR;
            else if (returntype1.type != returntype2.type)
            {
                printf("Error type 5 at Line %d: Type mismatched for assignment.\n", node->children[1]->line_num);
                returntype.kind = TYPE_ERROR;
            }
            else
            {
                returntype.kind = TYPE_LOCAL;
                returntype.type = returntype1.type;
                returntype.flag = returntype1.flag;
            }
            return returntype;
        }
        else if (strcmp(node->children[1]->name, "AND") == 0 || strcmp(node->children[1]->name, "OR") == 0)
        {
            ReturnType_ returntype2 = Exp(node->children[2]);

            if (returntype1.kind == TYPE_ERROR || returntype2.kind == TYPE_ERROR)
                returntype.kind = TYPE_ERROR;
            else if (returntype1.flag == 0 && returntype2.flag == 0)
            {
                returntype.kind = TYPE_LOCAL;
                returntype.type = returntype1.type;
                returntype.flag = returntype1.flag;
            }
            else
            {
                returntype.kind = TYPE_ERROR;
                if (returntype1.type->kind != BASIC || returntype1.type->u.basic != 0)
                    printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);

                if (returntype2.type->kind != BASIC || returntype2.type->u.basic != 1)
                    printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);
            }
            return returntype;
        }
        
        // operator
        else if ((strcmp(node->children[1]->name, "RELOP") == 0) ||
                 (strcmp(node->children[1]->name, "PLUS") == 0)  ||
                 (strcmp(node->children[1]->name, "MINUS") == 0) ||
                 (strcmp(node->children[1]->name, "STAR") == 0)  ||
                 (strcmp(node->children[1]->name, "DIV") == 0))
        {
            struct ReturnType_ returntype2 = Exp(node->children[2]);

            if (returntype1.kind == TYPE_ERROR || returntype2.kind == TYPE_ERROR)
                returntype.kind = TYPE_ERROR;

            else if ((returntype1.flag <= 1 && returntype2.flag <= 1) && (returntype1.kind == returntype2.kind))
            {
                returntype.kind = TYPE_LOCAL;
                returntype.type = &type_int;
                returntype.flag = returntype1.flag;
            }
            else
            {
                returntype.kind = TYPE_ERROR;
                printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);
            }
            return returntype;
        }
        else if (strcmp(node->children[1]->name, "LB") == 0)
        {
            // print_tree(node, 0);
            ReturnType_ returntype2 = Exp(node->children[2]);

            if (returntype1.kind == TYPE_ERROR || returntype2.kind == TYPE_ERROR)
                returntype.kind = TYPE_ERROR;
            else if (returntype1.flag == 2 && returntype2.type == &type_int)
            {
                //Array
                returntype.kind = returntype1.kind;
                returntype.type = returntype1.type->u.array.elem;

                if (returntype.type == 0)
                    returntype.flag = -1;
                else if (returntype.type  == &type_int)
                    returntype.flag = 0;
                else if (returntype.type  == &type_float)
                    returntype.flag = 1;
                else if (returntype.type->kind == ARRAY)
                    returntype.flag = 2;
                else
                {
                    returntype.flag = 3;
                }

            }
            else
            {
                returntype.kind = TYPE_ERROR;
                if (returntype1.flag != 2)
                {
                    // print_tree(node, 0);
                    printf("Error type 10 at Line %d: \"%s\" is not an array.\n", node->children[1]->line_num, node->children[0]->children[0]->value);
                }
                if (returntype2.type != &type_int)
                {
                    // print_tree(node, 0);
                    printf("Error type 12 at Line %d: \"%s\" is not an integer.\n", node->children[1]->line_num, node->children[2]->children[0]->value);
                }
            }
            return returntype;
        }
        else
        {
            //Exp DOT ID
            // print_tree(node, 0);
            // struct Node* temp = node;
            
            // while (strcmp(temp->name, "ID") != 0)
            // {
            //     temp = temp->children[0];
            //     // printf("temp name %s:", temp->name);
            // }
            // printf("id: %s\n", temp->value);
            // int x = check_hash_table(temp->value);
            // printf("x: %d\n", x);
            // Unit unit = get_unit(temp->value);
            // int x = unit->u.type->u.basic;
            // printf("x :%d\n", x);
            // printf("name: %s\n", unit->name);
            // printf("kind: %d\n", unit->u.type);
            // printf("returntype: %d\n", returntype1.type);
            // printf("type_int: %d\n", &type_int);
            // if (unit->u.var->type == NULL)
            //     printf("NULL");
            // printf("kind: %s\n", unit->u.var->type->kind);
            // printf("union: %s\n", unit->u.fieldlist->type->kind);
            
            // printf("%s\n", unit->u.type->kind);
            
            //printf("unit: %d\n", unit->u.type->u.basic);
            
            if (returntype1.kind == TYPE_ERROR)
                return returntype1;
        //    printf("%s\n", node->children[2]->value);
            // else if (returntype1.flag == 3)
            {
                // printf("%d\n", returntype1.flag);
                FieldList fieldlist = NULL;
                if (returntype1.type != NULL)
                    fieldlist = returntype1.type->u.structure->tail;
                // if (fieldlist != NULL)
                    // printf("Here");
                while (fieldlist != 0)
                {
                    // printf("%s", fieldlist->name);
                    // printf("%s\n", node->children[2]->name);
                    if (strcmp(fieldlist->name, node->children[2]->name) == 0)
                    {
                        returntype1.type = fieldlist->type;
                        if (returntype1.type == 0)
                            returntype1.flag = -1;
                        else if (returntype1.type  == &type_int)
                            returntype1.flag = 0;
                        else if (returntype1.type  == &type_float)
                            returntype1.flag = 1;
                        else if (returntype1.type->kind == ARRAY)
                            returntype1.flag = 2;
                        else
                        {
                            returntype1.flag = 3;
                        }
                        break;
                    }
                    fieldlist = fieldlist->tail;
                }

                if (fieldlist == NULL && returntype1.flag == 3)
                {
                    returntype1.kind = TYPE_ERROR;
                    printf("Error type 14 at Line %d: Non-existent field.\n", node->children[1]->line_num);
                    return returntype1;
                }
            }
            
            if (returntype1.flag == -1)
            {
                // printf("flag: %d\n", returntype1.flag);
                returntype1.kind = TYPE_ERROR;
                printf("Error type 13 at Line %d: Illegal use of \'.\'.\n", node->children[1]->line_num);
                return returntype1;
            }

            // return returntype1;
        }

    }
    else if (strcmp(node->children[0]->name, "LP") == 0)
    {
        returntype = Exp(node->children[1]);
        return returntype;
    }
    else if (strcmp(node->children[0]->name, "MINUS") == 0 || strcmp(node->children[0]->name, "NOT") == 0)
    {
        returntype = Exp(node->children[1]);

        if (returntype.flag <= 1 && returntype.kind != TYPE_ERROR)
            returntype.kind = TYPE_LOCAL;
        else
            returntype.kind = TYPE_ERROR;

        return returntype;
    }
    else if (strcmp(node->children[0]->name, "ID") == 0)
    {
        Unit unit = get_unit(node->children[0]->value);
        if (unit == 0)
            returntype.kind = TYPE_ERROR;
        else
        {
            returntype.kind = unit->kind;
            switch (unit->kind)
            {
                case TYPE_FUNCTION:
                    returntype.type = unit->u.func->return_type;
                    break;
                case TYPE_VAR:
                    returntype.type = unit->u.var->type;
                    break;
                case TYPE_FIELDLIST:
                    returntype.type = 0;
                    break;
            }

            if (returntype.type == 0)
                returntype.flag = -1;
            else if (returntype.type  == &type_int)
                returntype.flag = 0;
            else if (returntype.type  == &type_float)
                returntype.flag = 1;
            else if (returntype.type->kind == ARRAY)
                returntype.flag = 2;
            else
            {
                returntype.flag = 3;
            }
        }

        if (node->children_num == 1)
        {
            // the unit is not in the hash table
            if (unit == NULL)
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->value);

            return returntype;
        }
        else
        {
            if (unit == NULL)
            {
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n", node->children[0]->line_num, node->children[0]->value);
                return returntype;
            }
            // unit is not function.
            if (unit->kind != TYPE_FUNCTION)
            {
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n", node->children[0]->line_num, unit->name);
                returntype.kind = TYPE_ERROR;
                return returntype;
            }
            returntype.kind = TYPE_LOCAL;
            returntype.type = unit->u.func->return_type;

            Args a = unit->u.func->args;
            if (node->children_num == 4)
            {
                if (args(node->children[2], a) == 0)
                    returntype.kind = TYPE_ERROR;
            }
            return returntype;

        }
    }
    // INT
    else if (strcmp(node->children[0]->name, "INT") == 0)
    {
        returntype.type = &type_int;
        returntype.flag = 0;
        return returntype;
    }
    // FLOAT
    else if (strcmp(node->children[0]->name, "FLOAT") == 0)
    {
        returntype.type = &type_float;
        returntype.flag = 1;
        return returntype;
    }

}