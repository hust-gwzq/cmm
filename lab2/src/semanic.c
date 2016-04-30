#include "../include/semanic.h"

/*int kind in this file:
	0 = int
	1 = fieldlistoat
	2 = array
	3 = struct
	4 = function
	6 = var
	5 = allFit
	7 = error
	8 = valid
	9 = fieldlist
	10 = local
	11 = basic
*/

struct Type_ type_int;
struct Type_ type_float;

Unit hash_table[HASHSIZE];

Type thisReturnType;

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
        hash_table[i]->next = 0;
    }
    return 0;
}

void insert_hash_table(Unit unit)
{
    uint32_t index = hash_pjw(unit->name);

    Unit p = hash_table[index];
    unit->next = p->next;
    p->next = unit;
}

int check_hash_table(char* name)
{
    //printf("In check_hash_table\n");
    // printf("I am checking %s\n", n);
    int index = hash_pjw(name);
    Unit p = hash_table[index];
    if(p == 0)
        return -1;
    while(p != 0)
    {
        if (p->name != 0 && strcmp(p->name, name) == 0)
            return 1;
        p = p->next;
    }
    if(p == 0)
        return 0;
}

Unit get_unit(char* name)
{
    uint32_t index = hash_pjw(name);
    Unit p = hash_table[index];
    while (p != 0)
    {
        if (p->name != 0 && strcmp(p->name, name) == 0)
            return p;
        p = p->next;
    }
    return 0;
}


//------------------High-level Definitions

void program(struct Node* node)
{
    //printf("In Program\n");

    //Initialization.
    type_int.kind = BASIC;
    type_int.u.basic = 0;
    type_float.kind = BASIC;
    type_float.u.basic = 1;
    init_hash_table();
    //Program -> extDefList

    extDefList(node->children[0]);
    return;
}

void extDefList(struct Node* node)
{
    if (node->children_num != 0)
    {
        extDef(node->children[0]);
        extDefList(node->children[1]);
    }
}

void extDef(struct Node* node)
{
    //printf("In ExtDef\n");
    //ExtDef -> Specifier ExtDecList SEMI
    //		  | Specifier SEMI
    //		  | Specifier FunDec CompSt
    if (2 == node->children_num)
    {
        //Specifier SEMI
        specifier(node->children[0]);
        //Do nothing.
        return;
    }
    else if (strcmp(node->children[1], "ExtDecList") == 0)
    {
        //Specifier ExtDecList SEMI
        Type thisType = specifier(node->children[0]);
        extDecList(node->children[1], thisType);
    }
    else
    {
        //Specifier FunDec CompSt
        Type thisType = specifier(node->children[0]);
        Func f = funDec(node->children[1]);

        //Insert in the hash table.
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = f->name;//Just a pointer.
        unit->kind = 4;//Function.
        f->return_type = thisType;
        unit->u.func = f;

        insert_hash_table(unit);

        thisReturnType = thisType;

        compSt(node->children[2]);
    }
}

void extDecList(struct Node* node, Type t)
{
    if (node->children_num == 1)
    {
        //printf("name is %s", node->children[0]->name);
        Var v = varDec(node->children[0]);
        if (check_hash_table(v->name) == 1)
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->name);
        else
        {
            //Insert in the hash table.
            Unit unit = (Unit)malloc(sizeof(Unit_));
            unit->name = v->name;
            unit->kind = 6;//Var.
            unit->u.var->type = t;
            unit->u.var = v;

            insert_hash_table(unit);
        }
    }
    else
    {
        //VarDec COMMA ExtDecList
        Var v = varDec(node->children[0]);
        extDecList(node->children[2], t);

    }
    return;
}

//------------------Specifiers
Type specifier(struct Node* node)
{
    //printf("In Specifier\n");
    //Specifier -> TYPE
    //			 | StructSpecifier
    if (strcmp(node->children[0]->name, "TYPE") == 0)
    {
        if (strcmp(node->children[0]->value, "int") == 0)
        {
            type_int.kind = BASIC,
                     type_int.u.basic = 0;
            return &type_int;
        }
        else
        {
            type_float.kind = BASIC,
                       type_float.u.basic = 1;
            return &type_float;
        }
    }
    else
    {
        return structSpecifier(node->children[0]);
    }
}

Type structSpecifier(struct Node* node)
{
    //printf("In StructSpecifier\n");
    //StructSpecifier -> STRUCT OptTag LC Defieldlistist RC
    //				   | STRUCT Tag
    if (node->children_num == 5)
    {
        //STRUCT OptTag LC Defieldlistist RC
        Unit unit = optTag(node->children[1]);

        unit->u.type->u.structure->tail = defList(node->children[3], 1);

        return unit->u.type;
    }
    else
    {
        //STRUCT Tag
        Unit unit = get_unit(node->children[1]->children[0]->value);

        if (unit == 0)
        {
            printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", node->children[1]->line_num, node->children[1]->children[0]->value);
            return 0;
        }
        else
        {
            if (unit->kind == 5)
                return unit->u.type;
            else
            {
                printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", node->children[1]->line_num), node->children[1]->children[0]->value;
                return 0;
            }
        }
    }
}

Unit optTag(struct Node* node)
{
    //printf("In OptTag\n");
    //OptTag -> EMPTY
    //    	  | ID

    if (node->children_num == 1)
    {
        //ID
        if(check_hash_table(node->children[0]->value) == 1)
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", node->children[0]->line_num, node->children[0]->value);

        //Insert in the hash table.
        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = node->children[0]->value;
        fieldlist->type = 0;
        fieldlist->tail = 0;
        Type t = (Type)malloc(sizeof(Type_));
        t->kind = STRUCTURE;
        t->u.structure = fieldlist;
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = fieldlist->name;
        unit->kind = 5;
        unit->u.type = t;

        insert_hash_table(unit);

        return unit;
    }
    else
    {

        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = 0;
        fieldlist->type->kind = STRUCTURE;
        fieldlist->tail = 0;
        Type t = (Type)malloc(sizeof(Type_));
        t->kind = STRUCTURE;
        t->u.structure = fieldlist;
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = unit->u.type->u.structure->name;
        return unit;

    }
}

void tag(struct Node* node)
{
    //printf("In Tag\n");
    //Tag -> ID
    if(check_hash_table(node->children[0]->value) == 1)
        printf("Error type Unknown at Line %d:\n", node->children[0]->line_num);
    return;
}

//------------------Declarators
Var varDec(struct Node* node)
{
    //printf("In VarDec\n");
    //VarDec -> ID
    //		  | VarDec LB INT RB
    if (node->children_num == 1)
    {
        //ID
        Var v = (Var)malloc(sizeof(Var_));
        v->name = node->children[0]->value;
        v->type = 0;
        return v;
    }
    else
    {
        //VarDec LB INT RB
        Var v = varDec(node->children[0]);
        Type t = (Type)malloc(sizeof(Type_));
        t->kind == ARRAY;
        t->u.array.elem = v->type;
        t->u.array.size = strtol(node->children[2]->value, 0, 10);
        v->type = t;
        return v;
    }
}

Func funDec(struct Node* node)
{
    //printf("In FunDec\n");
    //FunDec -> ID LP VarList RP
    //		  | ID LP RP
    if (check_hash_table(node->children[0]->value) == 1)//Check ID.
        printf("Error type 4 at Line %d: Redefined function \"%s\"\n", node->children[0]->line_num,node->children[0]->value);
    if (node->children_num == 4)
    {
        //ID LP VarList RP
        Func f = (Func)malloc(sizeof(Func_));
        f->name = node->children[0]->value;
        f->return_type = 0;
        f->args = varList(node->children[2]);
        return f;
    }
    else
    {
        Func f = (Func)malloc(sizeof(Func_));
        f->name = node->children[0]->value;
        f->return_type = 0;
        f->args = 0;
        return f;
    }
}

Args varList(struct Node* node)
{
    //printf("In VarList\n");
    //VarList -> ParamDec COMMA VarList
    //		   | ParamDec
    Args a = paramDec(node->children[0]);
    if (node->children_num == 1)
        return a;
    else
    {
        a->next = varList(node->children[2]);
        return a;
    }
}

Args paramDec(struct Node* node)
{
    //printf("In ParamDec\n");
    //ParamDec -> Specifier VarDec

    Type t = specifier(node->children[0]);
    Var v = varDec(node->children[1]);
    if (v->type == 0)
        v->type = t;
    else//Array
    {
        Type tt = v->type;
        while (tt->u.array.elem != 0)
            tt = tt->u.array.elem;
        tt->u.array.elem = t;
    }

    if (check_hash_table(v->name) == 1)
        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[1]->line_num, node->children[1]->name);

    //Insert in the hash table.
    Unit unit = (Unit)malloc(sizeof(Unit_));
    unit->name = v->name;
    unit->kind = 6;//var.
    unit->u.var = v;

    insert_hash_table(unit);

    Args a = (Args)malloc(sizeof(Args_));
    a->var = v;
    a->next = 0;
    return a;
}

//------------------Statements
void compSt(struct Node* node)
{
    //printf("In CompSt\n");
    //CompSt -> LC Defieldlistist StmtList RC
    defList(node->children[1], 0);
    stmtList(node->children[2]);
    return;
}

void stmtList(struct Node* node)
{
    //printf("In StmtList\n");
    //StmtList -> EMPTY
    // 		    | Stmt StmtList
    if (node->children_num == 0)
        return;
    else
    {
        //printf("WHY..%s........%s.\n", node->name,  node->children[0]->name);
        stmt(node->children[0]);
        stmtList(node->children[1]);
        return;
    }
}

void stmt(struct Node* node)
{
    // printf("In Stmt\n");
    if (node->children_num == 2)
    {
        //Exp SEMI
        exp(node->children[0]);
    }
    else if (node->children_num == 1)
    {
        //CompSt
        compSt(node->children[0]);
    }
    else if(node->children_num == 3)
    {
        //RETURN Exp SEMI
        ExpReturnType_ ert = exp(node->children[1]);
        if (ert.kind != 7 && thisReturnType != ert.type)
            printf("Error type 8 at Line %d: Type mismatched for return.\n", node->children[1]->line_num);

    }
    else if (node->children_num == 7)
    {
        //IF LP Exp RP Stmt ELSE Stmt
        ExpReturnType_ ert = exp(node->children[2]);
        if (ert.kind != 7 && ert.type != &type_int)
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[0]->line_num);

        stmt(node->children[4]);
        stmt(node->children[6]);
    }
    else
    {

        //WHILE LP Exp RP Stmt
        //IF LP Exp RP Stmt
        exp(node->children[2]);

        ExpReturnType_ ert = exp(node->children[2]);
        if (ert.kind != 7 && ert.type != &type_int)
            printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[0]->line_num);
        stmt(node->children[4]);

    }
}

FieldList defList(struct Node* node, int varOrStruct)
{
    if (strcmp(node->name, "") ==0)
        return 0;
    //printf("In Defieldlistist\n");
    //Defieldlistist : EMPTY
    // 		  | Def Defieldlistist

    if (node->children_num == 0)
    {
        //printf("EMPTY in Defieldlistist\n");
        return 0;
    }
    else
    {
        FieldList fieldlist = def(node->children[0], varOrStruct);

        //printf("IDONTKNOW %d\n", varOrStruct);

        if (varOrStruct == 0)
            defList(node->children[1], varOrStruct);
        else
        {

            if (fieldlist != 0)
            {
                FieldList fieldlist = fieldlist;
                while (fieldlist->tail != 0)
                    fieldlist =fieldlist->tail;
                fieldlist->tail = defList(node->children[1], varOrStruct);
            }
        }
        return fieldlist;
    }
}

FieldList def(struct Node* node, int varOrStruct)
{
    //printf("In Def\n");
    //Def -> Specifier DecList SEMI
    Type t = specifier(node->children[0]);
    FieldList fieldlist = decList(node->children[1], t, varOrStruct);

    return fieldlist;
}

FieldList decList(struct Node* node, Type t, int varOrStruct)
{
    //printf("In DecList\n");
    //DecList -> Dec
    //		   | Dec COMMA DecList
    FieldList fieldlist =dec(node->children[0], 0, varOrStruct);
    if (node->children_num == 1)
        return fieldlist;

    else
    {
        if (varOrStruct == 1)
            fieldlist->tail = decList(node->children[2], t, varOrStruct);
        else
        {
            decList(node->children[2], t, varOrStruct);
        }
        return fieldlist;
    }
}

FieldList dec(struct Node* node, Type t, int varOrStruct)
{
    //printf("In Dec\n");
    //Dec -> VarDec
    // 	   | VarDec ASSIGNOP Exp

    Var v = varDec(node->children[0]);
    if (check_hash_table(v->name) == 1)
    {
        if (varOrStruct == 1)
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n", node->children[0]->line_num, v->name);
        else
        {
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, v->name);
        }
    }

    v->type = t;

    FieldList fieldlist = 0;

    if (varOrStruct == 1)
    {
        //Insert in the hash table.
        fieldlist = (FieldList)malloc(sizeof(FieldList_));
        fieldlist->name = v->name;
        fieldlist->type = v->type;
        fieldlist->tail = 0;

        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = fieldlist->name;
        unit->kind = 9;
        unit->u.fieldlist = fieldlist;

        insert_hash_table(unit);
    }
    else
    {
        //Insert in the hash table.
        Unit unit = (Unit)malloc(sizeof(Unit_));
        unit->name = v->name;
        unit->kind = 6;
        unit->u.var = v;

        insert_hash_table(unit);
    }

    if (node->children_num == 3)
    {
        // VarDec ASSIGNOP Exp
        if (varOrStruct == 1)
            printf("Error type 15 at Line %d: Redefined names in the structure.\n", node->children[0]->line_num);
        else
        {
            ExpReturnType_ ert = exp(node->children[2]);
        }
    }

    return fieldlist;
}

ExpReturnType_ exp(struct Node* node)
{
    ExpReturnType_ ert;

    if (strcmp(node->children[0]->name, "Exp") == 0)
    {
        //printf("In the first is Exp\n");
        ExpReturnType_ ert1 = exp(node->children[0]);
        if (strcmp(node->children[1]->name, "ASSIGNOP") == 0)
        {
            //printf("In assignop\n");
            //Exp ASSIGNOP Exp
            if (ert1.kind != 7 && ert1.kind != 6)
            {
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->children[0]->line_num);
                ert.kind = 7;
                return ert;
            }

            struct ExpReturnType_ ert2 = exp(node->children[2]);

            if (ert1.kind == 7 || ert2.kind == 7)
                ert.kind = 7;
            else if (ert1.type != ert2.type)
            {
                printf("Error type 5 at Line %d: Type mismatched for assignment.\n", node->children[1]->line_num);
                ert.kind = 7;
            }
            else
            {
                ert.kind = 10;
                ert.type = ert1.type;
                ert.flag = ert1.flag;
            }
            return ert;
        }
        else if (strcmp(node->children[1]->name, "AND") == 0 || strcmp(node->children[1]->name, "OR") == 0)
        {
            //Exp AND Exp
            //Exp OR Exp

            ExpReturnType_ ert2 = exp(node->children[2]);

            if (ert1.kind == 7 || ert2.kind == 7)
                ert.kind = 7;
            else if (ert1.flag == 0 && ert2.flag == 0)
            {
                ert.kind = 10;
                ert.type = ert1.type;
                ert.flag = ert1.flag;
            }
            else
            {
                ert.kind = 7;
                if (ert1.type->kind != BASIC || ert1.type->u.basic != 0)
                    printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);

                if (ert2.type->kind != BASIC || ert2.type->u.basic != 1)
                    printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);
            }
            return ert;
        }
        else if ((strcmp(node->children[1]->name, "RELOP") == 0) ||
                 (strcmp(node->children[1]->name, "PLUS") == 0)  ||
                 (strcmp(node->children[1]->name, "MINUS") == 0) ||
                 (strcmp(node->children[1]->name, "STAR") == 0)  ||
                 (strcmp(node->children[1]->name, "DIV") == 0))
        {
            struct ExpReturnType_ ert2 = exp(node->children[2]);

            if (ert1.kind == 7 || ert2.kind == 7)
                ert.kind = 7;

            else if ((ert1.flag <= 1 && ert2.flag <= 1) && (ert1.kind == ert2.kind))
            {
                ert.kind = 10;
                ert.type = &type_int;
                ert.flag = ert1.flag;
            }
            else
            {
                ert.kind = 7;
                printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->children[1]->line_num);
            }
            return ert;
        }
        else if (strcmp(node->children[1]->name, "LB") == 0)
        {
            ExpReturnType_ ert2 = exp(node->children[2]);

            if (ert1.kind == 7 || ert2.kind == 7)
                ert.kind = 7;
            else if (ert1.flag == 2 && ert2.type == &type_int)
            {
                //Array
                ert.kind = ert1.kind;
                ert.type = ert1.type->u.array.elem;

                if (ert.type == 0)
                    ert.flag = -1;
                else if (ert.type  == &type_int)
                    ert.flag = 0;
                else if (ert.type  == &type_float)
                    ert.flag = 1;
                else if (ert.type->kind == ARRAY)
                    ert.flag = 2;
                else
                {
                    ert.flag = 3;
                }

            }
            else
            {
                ert.kind = 7;
                if (ert1.flag != 2)
                {
                    // print_tree(node, 0);
                    printf("Error type 10 at Line %d: \"%s\" is not an array.\n", node->children[1]->line_num, node->children[0]->children[0]->value);
                }
                if (ert2.type != &type_int)
                {
                    // print_tree(node, 0);
                    printf("Error type 12 at Line %d: \"%s\" is not an integer.\n", node->children[1]->line_num, node->children[2]->children[0]->value);
                }
            }
            return ert;
        }
        else
        {
            //Exp DOT ID

            if (ert1.kind == 7)
                return ert1;
            else if (ert1.flag == 3)
            {
                //printf("why i'm here\n");
                FieldList fieldlist = ert1.type->u.structure->tail;
                while (fieldlist != 0)
                {
                    if (strcmp(fieldlist->name, node->children[2]->name) == 0)
                    {
                        ert1.type = fieldlist->type;
                        if (ert1.type == 0)
                            ert1.flag = -1;
                        else if (ert1.type  == &type_int)
                            ert1.flag = 0;
                        else if (ert1.type  == &type_float)
                            ert1.flag = 1;
                        else if (ert1.type->kind == ARRAY)
                            ert1.flag = 2;
                        else
                        {
                            ert1.flag = 3;
                        }
                        break;
                    }
                    fieldlist = fieldlist->tail;
                }

                if (fieldlist == 0)
                {
                    ert1.kind = 7;
                    printf("Error type 14 at Line %d: Non-existent field.\n", node->children[1]);
                }
            }

            else
            {
                ert1.kind = 7;
                printf("Error type 13 at Line %d: Illegal use of \'.\'.\n", node->children[1]->line_num);
            }
            return ert1;
        }

    }
    else if (strcmp(node->children[0]->name, "LP") == 0)
    {
        //LP Exp RP
        ert = exp(node->children[1]);
        return ert;
    }
    else if (strcmp(node->children[0]->name, "MINUS") == 0 || strcmp(node->children[0]->name, "NOT") == 0)
    {
        //MINUS Exp
        ert = exp(node->children[1]);

        if (ert.flag <= 1 && ert.kind != 7)
            ert.kind = 10;
        else
            ert.kind = 7;

        return ert;
    }
    else if (strcmp(node->children[0]->name, "ID") == 0)
    {
        Unit unit = get_unit(node->children[0]->value);
        if (unit == 0)
            ert.kind = 7;
        else
        {
            ert.kind = unit->kind;
            switch (unit->kind)
            {
            case 6:
                ert.type = unit->u.var->type;
                break;//var
            case 4:
                ert.type = unit->u.func->return_type;
                break;//function
            case 9:
                ert.type = 0;
                break;
            }

            if (ert.type == 0)
                ert.flag = -1;
            else if (ert.type  == &type_int)
                ert.flag = 0;
            else if (ert.type  == &type_float)
                ert.flag = 1;
            else if (ert.type->kind == ARRAY)
                ert.flag = 2;
            else
            {
                ert.flag = 3;
            }
        }


        if (node->children_num == 1)
        {
            //ID
            if (unit == 0)
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->value);

            return ert;
        }
        else
        {
            if (unit == 0)
            {
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n", node->children[0]->line_num, node->children[0]->value);
                return ert;
            }
            if (unit->kind != 4)
            {
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n", node->children[0]->line_num, unit->name);
                ert.kind = 7;
                return ert;
            }
            ert.kind = 10;
            ert.type = unit->u.func->return_type;

            Args a = unit->u.func->args;
            if (node->children_num == 4)
            {
                if (args(node->children[2], a) == 0)
                    ert.kind = 7;
            }
            return ert;

        }
    }
    else if (strcmp(node->children[0]->name, "INT") == 0)
    {
        ert.type = &type_int;
        ert.flag = 0;
        return ert;
    }
    else
    {
        ert.type = &type_float;
        ert.flag = 1;
        return ert;
    }

}

int args(struct Node* node, Args arg)
{
    ExpReturnType_ ert = exp(node->children[0]);

    if(node->children_num == 1)
    {
        if (ert.kind == 7)
            return 0;
        else if (arg == 0 || ert.type == arg->var->type)
        {
            //   print_tree(node, 0);
            printf("Error type 9 at Line %d: Function is not applicable for arguments.\n", node->children[0]->line_num);
            return 0;
        }
        else
        {
            printf("Error type 9 at Line %d: Function is not applicable for arguments.\n", node->children[0]->line_num);
            return 0;
        }
    }
    else
    {
        return args(node->children[2], arg->next);
    }
}
