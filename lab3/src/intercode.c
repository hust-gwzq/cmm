#include "../include/intercode.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define MAXSIZE 64

// define head ptr and tail ptr
InterCodes* head = 0;
InterCodes* tail = 0;

typedef struct FieldList_* FieldList;

int labelNo = 1;
int varNo = 1;

int getTypeSize(Type type)
{
    FieldList fl;
    switch(type->kind)
    {
    case BASIC:
        return 4;
    case ARRAY_:
        return getTypeSize(type->u.array.elem) * type->u.array.size;
    case STRUCTURE:

        fl = type->u.structure;
        int size = 0;
        for(; fl != 0; fl = fl->tail)
            size += getTypeSize(fl->type);
        return size;
    default:
        printf("Unknown type...\n");
        exit(-1);
    }
}

string newLabel()
{
    string temp = (string)malloc(MAXSIZE);
    sprintf(temp, "label%d", labelNo);
    labelNo++;
    return temp;
}

Operand* newOperand()
{
    //You need to free it after using it!
    Operand* operand = (Operand*)malloc(sizeof(Operand));
    return operand;
}

Operand* newVar()
{
    Operand* temp = (Operand*)malloc(sizeof(Operand));
    temp->kind = VARIABLE;
    temp->u.name = (string)malloc(MAXSIZE);
    sprintf(temp->u.name, "t%d", varNo);
    varNo++;
    return temp;
}

InterCode* newInterCode()
{
    //You need to free it after using it!
    InterCode* ic = (InterCode*)malloc(sizeof(InterCode));
    return ic;
}

InterCodes* linkNode(InterCodes* lic1, InterCodes* lic2)
{
    lic1->next = lic2;
    lic2->next = 0;
    lic1->prev = 0;
    lic2->prev = lic1;
    return lic1;
}

InterCodes* insertLink(InterCode* ic)
{
    InterCodes* lic = (InterCodes*)malloc(sizeof(struct InterCodes));
    lic->code = ic;
    lic->prev = 0;
    lic->next = 0;
    if(head == 0)
    {
        head = lic;
        tail = lic;
    }
    else
    {
        tail->next = lic;
        lic->prev = tail;
        tail = lic;
        tail->next = 0;//head?
        head->prev = 0;//tail? Loop linkedlist?
    }
    return lic;
}

void translateArray(Node* node, Type type, Type tp, Operand* last, Operand* temp)
{
    Node* q = node->children[0];

    Type t;

    int tpSize = 1;
    for(t=tp->u.array.elem; t->u.array.elem!=0; t=t->u.array.elem)
        tpSize *= t->u.array.size;

    Operand* t1 = newVar();
    translateExp(node->children[2], t1);

    InterCode* ic = newInterCode();
    ic->kind = MUL_IC;
    ic->u.triop.result = t1;
    ic->u.triop.operand1 = newOperand();
    ic->u.triop.operand1->kind = CONSTANT;
    ic->u.triop.operand1->u.value = 4;
    ic->u.triop.operand2 = t1;
    insertLink(ic);

    if (tpSize > 1)
    {
        // printf("Array here...\n");
        ic = newInterCode();
        ic->kind = MUL_IC;
        ic->u.triop.result = t1;
        ic->u.triop.operand1 = newOperand();
        ic->u.triop.operand1->kind = CONSTANT;
        ic->u.triop.operand1->u.value = tpSize;
        ic->u.triop.operand2 = t1;
        insertLink(ic);
    }

    if(last != 0)
    {
        ic = newInterCode();
        ic->kind = ADD_IC;
        ic->u.triop.result = t1;
        ic->u.triop.operand1 = t1;
        ic->u.triop.operand2 = last;
        insertLink(ic);
    }

    if(strcmp(q->children[0]->name,"ID")==0)
    {
        ic = newInterCode();
        ic->kind = ADD_IC;
        ic->u.triop.result = temp;
        ic->u.triop.operand1 = newOperand();
        ic->u.triop.operand1->kind = ARRAY_;
        ic->u.triop.operand1->u.name = strdup(q->children[0]->value);
        ic->u.triop.operand2 = t1;
        insertLink(ic);
    }
    else
    {
        //Find previous
        for(t=type; t->u.array.elem!=tp; t=t->u.array.elem);
        translateArray(q,type,t,t1,temp);
    }
}

void translateCond(Node* node, string labelTrue, string labelFalse)
{
    //node is Exp.
    //printf("In COND\n");
    assert(strcmp(node->name, "Exp") == 0);

    if (strcmp(node->children[1]->name, "RELOP") == 0)
    {
        //printf("Here am I\n");
        Operand* t1 = newVar();
        Operand* t2 = newVar();
        translateExp(node->children[0], t1);
        translateExp(node->children[2], t2);

        InterCode* ic = newInterCode();
        ic->kind = COND_IC;
        ic->u.cond.operand1 = t1;
        ic->u.cond.operand2 = t2;
        ic->u.cond.op = strdup(node->children[1]->value);
        ic->u.cond.name = strdup(labelTrue);
        insertLink(ic);

        ic = newInterCode();
        ic->kind = LABEL_GOTO_IC;
        ic->u.label_goto.name = strdup(labelFalse);
        insertLink(ic);
    }
    else if (strcmp(node->children[0]->name, "NOT") == 0)
    {
        translateCond(node->children[1], labelFalse, labelTrue);
    }
    else if (strcmp(node->children[1]->name, "AND") == 0)
    {
        string label1 = newLabel();

        translateCond(node->children[0], label1, labelFalse);

        InterCode* ic = newInterCode();
        ic->kind = LABEL_IC;
        ic->u.label.name = strdup(label1);
        insertLink(ic);

        translateCond(node->children[2], labelTrue, labelFalse);
        free(label1);
    }
    else if (strcmp(node->children[1]->name, "OR") == 0)
    {
        string label1 = newLabel();

        translateCond(node->children[0], labelTrue, label1);

        InterCode* ic = newInterCode();
        ic->kind = LABEL_IC;
        ic->u.label.name = strdup(label1);
        insertLink(ic);

        translateCond(node->children[2], labelTrue, labelFalse);
        free(label1);
    }
    else
    {
        Operand* t1 = newVar();
        translateExp(node, t1);

        InterCode* ic = newInterCode();
        ic->kind = COND_IC;
        ic->u.cond.operand1 = t1;
        ic->u.cond.operand2 = newOperand();
        ic->u.cond.operand2->kind = CONSTANT;
        ic->u.cond.operand2->u.value = 0;
        ic->u.cond.op = strdup("!=");
        ic->u.cond.name = strdup(labelTrue);
        insertLink(ic);

        ic = newInterCode();
        ic->kind = LABEL_GOTO_IC;
        ic->u.label_goto.name = strdup(labelFalse);
        insertLink(ic);
    }
    return;
}

void translateProgram(Node* node)
{
    //printf("In Program\n");
    assert(strcmp(node->name, "Program") == 0);
    translateExtDefList(node->children[0]);
    return;
}

void translateExtDefList(Node* node)
{
    //printf("In ExtDefList\n");
    assert(strcmp(node->name, "ExtDefList") == 0);
    if (0 == node->children_num)
        return;
    else
    {
        translateExtDef(node->children[0]);
        if (strcmp(node->children[1]->name, "") != 0)
            translateExtDefList(node->children[1]);
        return;
    }
}

void translateExtDef(Node* node)
{
    //printf("In ExtDef\n");
    assert(strcmp(node->name, "ExtDef") == 0);
    if (3 == node->children_num && strcmp(node->children[1]->name, "FunDec") == 0)
    {
        translateFunDec(node->children[1]);
        translateCompSt(node->children[2]);
    }
    return;
}

int translateSpecifier(Node* node)
{
    //printf("In Specifier\n");
    assert(strcmp(node->name, "Specifier") == 0);
    if (strcmp(node->children[0]->name, "TYPE") == 0)
    {
        //Never float.
        return 4;
    }
    else
    {
        //TODO
        assert(strcmp(node->children[0]->name, "StructSpecifier") == 0);
        printf("I can not do this, Mr. Struct.\n");

        Node* structNode = node->children[0];
        //Deal with tag;
        if (strcmp(structNode->children[1]->name, "Tag") == 0)
        {
            //printf("struct name is %s\n", structNode->children[1]->children[0]->value);
            Unit i = get_unit(structNode->children[1]->children[0]->value);
            int size = 0;
            FieldList fl = i->u.fieldlist;
            for(; fl != 0; fl = fl->tail)
                size += getTypeSize(fl->type);
        }
    }
}

string translateVarDec(Node* node, int size)
{
    //printf("In VarDec\n");
    assert(strcmp(node->name, "VarDec") == 0);
    if (node->children_num == 1)
    {
        //ID.
        //printf("children_num=1\n");
        string id = node->children[0]->value;
        if (size > 4)
        {

            InterCode* ic = newInterCode();
            ic->kind = DEC_IC;
            ic->u.dec.operand = newOperand();
            ic->u.dec.operand->kind = ARRAY_;
            ic->u.dec.operand->u.name = strdup(node->children[0]->value);
            ic->u.dec.size = size;
            insertLink(ic);

        }
        return id;
    }
    else
    {
        //VarDec LB INT RB
        assert(node->children_num == 4);
        int n = strtol(node->children[2]->value, 0, 10);
        //printf("%c\n", node->children[2]->value);
        size *= n;
        //printf("!!!%d %d\n", size, n);
        return translateVarDec(node->children[0], size);
    }
}

void translateFunDec(Node* node)
{
    //printf("In FunDec\n");
    //ID LP VarList RP
    //ID LP RP
    assert(strcmp(node->name, "FunDec") == 0);

    InterCode* ic = newInterCode();
    ic->kind = FUNCTION_IC;
    ic->u.function.name = strdup(node->children[0]->value);
    insertLink(ic);

    if (node->children_num == 4)
        translateVarList(node->children[2]);
    return;

}

void translateVarList(Node* node)
{
    //printf("In VarList\n");
    assert(strcmp(node->name, "VarList") == 0);
    //ParamDec COMMA VarList
    //ParamDec
    translateParamDec(node->children[0]);
    if (node->children_num == 3)
        translateVarList(node->children[2]);
}

void translateParamDec(Node* node)
{
    //printf("In ParamDec\n");
    assert(strcmp(node->name, "ParamDec") == 0);
    //Specifier VarDec
    string id  = translateVarDec(node->children[1], 0);

    InterCode* ic = newInterCode();
    ic->kind = PARAM_IC;
    ic->u.param.name = strdup(id);
    insertLink(ic);
    return;
}

void translateCompSt(Node* node)
{
    ////printf("In CompSt, %d--%s--\n", node->children_num, node->children[1]->name);
    //LC DefList StmtList RC
    assert(strcmp(node->name, "CompSt") == 0);
    translateDefList(node->children[1]);
    //printf("Next is here\n");
    translateStmtList(node->children[2]);
    return;
}

void translateStmtList(Node* node)
{
    //printf("In StmtList\n");

    //empty
    //Stmt StmtList
    assert(strcmp(node->name, "StmtList") == 0);
    if (node->children_num == 0)
        return;
    else
    {
        //printf("that's why %s  %s  %s\n", node->children[0]->name, node->children[0]->value,node->children[1]->name);
        assert(node->children_num == 2);
        //if (strcmp(node->children[0]->name, "Stmt") == 0)
        translateStmt(node->children[0]);
        if (strcmp(node->children[1]->name, "") != 0)
            translateStmtList(node->children[1]);
        return;
    }
}

void translateStmt(Node* node)
{
    //printf("In Stmt\n");
    assert(strcmp(node->name, "Stmt") == 0);
    //Exp SEMI
    //CompSt
    //RETURN Exp SEMI
    //IF LP Exp RP Stmt
    //IF LP Exp RP Stmt ELSE Stmt
    //WHILE LP Exp RP Stmt
    if (strcmp(node->children[0]->name, "Exp") == 0)
        translateExp(node->children[0], 0);
    else if (strcmp(node->children[0]->name, "CompSt") == 0)
        translateCompSt(node->children[0]);
    else if (strcmp(node->children[0]->name, "RETURN") == 0)
    {
        //TODO
        Operand* t1 = newVar();

        translateExp(node->children[1], t1);

        InterCode* ic = newInterCode();
        ic->kind = RETURN_IC;
        ic->u.ret.operand = t1;
        insertLink(ic);
    }
    else if (strcmp(node->children[0]->name, "WHILE") == 0)
    {
        string label1 = newLabel();
        string label2 = newLabel();
        string label3 = newLabel();

        InterCode* ic = newInterCode();
        ic->kind = LABEL_IC;
        ic->u.label.name = strdup(label1);
        insertLink(ic);

        translateCond(node->children[2], label2, label3);

        //Label 2
        ic = newInterCode();
        ic->kind = LABEL_IC;
        ic->u.label.name = strdup(label2);
        insertLink(ic);

        translateStmt(node->children[4]);

        //Go to label 1
        ic = newInterCode();
        ic->kind = LABEL_GOTO_IC;
        ic->u.label_goto.name = strdup(label1);
        insertLink(ic);

        //Label 3
        ic = newInterCode();
        ic->kind = LABEL_IC;
        ic->u.label.name = strdup(label3);
        insertLink(ic);

        free(label1);
        free(label2);
        free(label3);

    }
    else
    {
        //IF LP Exp RP Stmt
        //IF LP Exp RP Stmt ELSE Stmt
        assert(strcmp(node->children[0]->name, "IF") == 0);
        if (node->children_num == 5)
        {
            string label1 = newLabel();
            string label2 = newLabel();

            translateCond(node->children[2], label1, label2);

            //Label1
            InterCode* ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label1);
            insertLink(ic);

            translateStmt(node->children[4]);

            //Label 2
            ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label2);
            insertLink(ic);

            free(label1);
            free(label2);
        }
        else
        {
            assert(node->children_num == 7);

            string label1 = newLabel();
            string label2 = newLabel();
            string label3 = newLabel();

            translateCond(node->children[2], label1, label2);

            //Label1
            InterCode* ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label1);
            insertLink(ic);

            translateStmt(node->children[4]);

            //Goto label3
            ic = newInterCode();
            ic->kind = LABEL_GOTO_IC;
            ic->u.label_goto.name = strdup(label3);
            insertLink(ic);

            //Label 2
            ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label2);
            insertLink(ic);

            translateStmt(node->children[6]);
            ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label3);
            insertLink(ic);

            free(label1);
            free(label2);
            free(label3);

        }
    }
    return;
}

void translateDefList(Node* node)
{
    if (strcmp(node->name, "") == 0)
        return;
    //printf("In DefList\n");
    //empty
    //Def DefList
    assert(strcmp(node->name, "DefList") == 0);
    if (node->children_num == 0)
        return;
    else
    {
        assert(node->children_num == 2);
        //printf("here is it, %s, %s\n", node->children[0]->name, node->children[1]->name);
        translateDef(node->children[0]);
        //printf("Maybe here?\n");
        if (strcmp(node->children[1]->name, "") != 0)
            translateDefList(node->children[1]);
        return;
    }
}

void translateDef(Node* node)
{
    //printf("In Def\n");
    //Specifier DecList SEMI
    assert(strcmp(node->name, "Def") == 0);
    int size = translateSpecifier(node->children[0]);
    //printf("^^^^^^^%d\n", size);
    translateDecList(node->children[1], size);
    return;
}

void translateDecList(Node* node, int size)
{
    //printf("In DecList\n");
    assert(strcmp(node->name, "DecList") == 0);
    //Dec
    //Dec COMMA DecList
    if (node->children_num == 1)
        translateDec(node->children[0], size);
    else
    {
        assert(node->children_num == 3);
        translateDec(node->children[0], size);
        translateDecList(node->children[2], size);
    }
    return;
}

void translateDec(Node* node, int size)
{
    //printf("In Dec");
    //VarDec
    //VarDec ASSIGNOP Exp
    assert(strcmp(node->name, "Dec") == 0);
    string v = translateVarDec(node->children[0], size);
    //printf("woo is %s\n", v);

    if (node->children_num == 3)
    {
        Operand* t1 = newVar();

        translateExp(node->children[2], t1);

        //Assign.

        InterCode* ic = newInterCode();
        ic->kind = ASSIGN_IC;
        ic->u.binop.result = newOperand();
        ic->u.binop.result->kind = VARIABLE;
        //printf("here???\n");
        ic->u.binop.result->u.name = strdup(v);
        //printf("get here?\n");
        ic->u.binop.operand = t1;
        printf("ASS in Dec, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);
        insertLink(ic);
    }
    //printf("Dec over\n");
    return;

}

void translateExp(Node* node, Operand* operand)
{
    //printf("In Exp\n");
    //Exp ASSIGNOP Exp
    //Exp AND Exp
    //Exp OR Exp
    //Exp RELOP Exp
    //Exp PLUS Exp
    //Exp MINUS Exp
    //Exp STAR Exp
    //Exp DIV Exp
    //LP Exp RP
    //MINUS Exp
    //NOT Exp
    //ID LP Args RP
    //ID LP RP
    //Exp LB Exp RB
    //Exp DOT ID
    //ID
    //INT
    //FLOAT
    assert(strcmp(node->name, "Exp") == 0);

    if (strcmp(node->children[0]->name, "Exp") == 0)
    {
        if (strcmp(node->children[1]->name, "ASSIGNOP") == 0)
        {
            Unit it = get_unit(node->children[1]->value);
            if (it == 0)
            {
                Operand* t1 = newVar();

                translateExp(node->children[2], t1);

                //Assign.
                InterCode* ic = newInterCode();
                ic->kind = ASSIGN_IC;
                ic->u.binop.result = newOperand();
                ic->u.binop.result->kind = VARIABLE;
                ic->u.binop.result->u.name = strdup(node->children[0]->children[0]->value);
                ic->u.binop.operand = t1;
                //printf("ASS in Exp assop 1, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);
                insertLink(ic);

                if (operand != 0)
                {
                    ic = newInterCode();
                    ic->kind = ASSIGN_IC;
                    ic->u.binop.result = operand;
                    ic->u.binop.operand = newOperand();
                    ic->u.binop.operand->kind = VARIABLE;
                    //Here node->children[2]?
                    ic->u.binop.operand->u.name = strdup(node->children[2]->children[0]->value);
                    //printf("ASS in Exp assop 2, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);

                    insertLink(ic);
                }
            }
            else if( it != 0 && it->kind == ARRAY_)
            {
                printf("I'm here...\n");
                Operand* t1 = newVar();
                Operand* t2 = newVar();

                translateExp(node->children[0], t1);
                translateExp(node->children[2], t2);
                InterCode* ic = newInterCode();
                ic->kind = ASSIGN_IC;
                ic->u.binop.result = t1;
                ic->u.binop.operand = t2;
                insertLink(ic);

                if (operand != 0)
                {
                    ic = newInterCode();
                    ic->kind = ASSIGN_IC;
                    ic->u.binop.result = operand;
                    ic->u.binop.operand = newOperand();
                    ic->u.binop.operand->kind = VARIABLE;
                    ic->u.binop.operand->u.name = strdup(node->children[2]->children[0]->value);
                }
            }
        }
        else if (strcmp(node->children[1]->name, "PLUS") == 0 ||
                 strcmp(node->children[1]->name, "MINUS") == 0 ||
                 strcmp(node->children[1]->name, "STAR") == 0 ||
                 strcmp(node->children[1]->name, "DIV") == 0)
        {
            Operand* t1 = newVar();
            Operand* t2 = newVar();

            translateExp(node->children[0], t1);
            translateExp(node->children[2], t2);

            InterCode* ic;

            if (strcmp(node->children[1]->name, "PLUS") == 0)
            {
                ic = newInterCode();
                ic->kind = ADD_IC;
                ic->u.triop.result = operand;
                ic->u.triop.operand1 = t1;
                ic->u.triop.operand2 = t2;
                insertLink(ic);
            }
            else if (strcmp(node->children[1]->name, "MINUS") == 0)
            {
                ic = newInterCode();
                ic->kind = SUB_IC;
                ic->u.triop.result = operand;
                ic->u.triop.operand1 = t1;
                ic->u.triop.operand2 = t2;
                insertLink(ic);
            }
            else if (strcmp(node->children[1]->name, "STAR") == 0)
            {
                ic = newInterCode();
                ic->kind = MUL_IC;
                ic->u.triop.result = operand;
                ic->u.triop.operand1 = t1;
                ic->u.triop.operand2 = t2;
                insertLink(ic);
            }
            else
            {
                assert(strcmp(node->children[1]->name, "DIV") == 0);
                ic = newInterCode();
                ic->kind = DIV_IC;
                ic->u.triop.result = operand;
                ic->u.triop.operand1 = t1;
                ic->u.triop.operand2 = t2;
                insertLink(ic);
            }
        }
        else if ((strcmp(node->children[1]->name, "RELOP") == 0) ||
                 (strcmp(node->children[1]->name, "AND") == 0) ||
                 (strcmp(node->children[1]->name, "OR") == 0) ||
                 (strcmp(node->children[0]->name, "NOT") == 0))
        {

            string label1 = newLabel();
            string label2 = newLabel();

            InterCode* ic = newInterCode();
            ic->kind = ASSIGN_IC;
            ic->u.binop.result = operand;
            ic->u.binop.operand = newOperand();
            ic->u.binop.operand->kind = CONSTANT;
            ic->u.binop.operand->u.value = 0;
            //printf("ASS in Exp andnotor1, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);

            insertLink(ic);

            translateCond(node, label1, label2);

            ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label1);
            insertLink(ic);

            ic = newInterCode();
            ic->kind = ASSIGN_IC;
            ic->u.binop.result = operand;
            ic->u.binop.operand = newOperand();
            ic->u.binop.operand->kind = CONSTANT;
            ic->u.binop.operand->u.value = 1;
            //printf("ASS in Exp andnotor2, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);

            insertLink(ic);

            ic = newInterCode();
            ic->kind = LABEL_IC;
            ic->u.label.name = strdup(label2);
            insertLink(ic);

            free(label1);
            free(label2);

        }
        else if (strcmp(node->children[1]->name, "LB") == 0)
        {

            //EXP LB EXP RB
            Node* n = node->children[0]->children[0];

            for(; strcmp(n->name,"ID") != 0; n = n->children[0]);

            //printf("Before not found%s\n", n->value);
            //printTable();

            Unit it = get_unit(n->value);

            if (it == 0)
            {
                printf("Not Found.\n");
                return;
            }

            Type type = it->u.var->type;
            Type tp = type;
            for(; tp->u.array.elem->kind != BASIC ; tp = tp->u.array.elem);
            operand->kind = ADDRESS;
            translateArray(node, type, tp, 0, operand);

        }
    }
    else if (strcmp(node->children[0]->name, "LP") == 0)
    {
        translateExp(node->children[1], operand);
    }
    else if (strcmp(node->children[0]->name, "MINUS") == 0)
    {
        Operand* t1 = newVar();
        translateExp(node->children[1], t1);

        //MINUS
        InterCode* ic = newInterCode();
        ic->kind = MINUS_IC;
        ic->u.triop.result = operand;
        ic->u.triop.operand1 = newOperand();
        ic->u.triop.operand1->kind = CONSTANT;
        ic->u.triop.operand1->u.value = 0;
        ic->u.triop.operand2 = t1;
        insertLink(ic);
    }
    else if (strcmp(node->children[0]->name, "ID") == 0)
    {
        ////printf("In ID\n");
        if (node->children_num == 1)
        {
            InterCode* ic = newInterCode();
            ic->kind = ASSIGN_IC;
            ic->u.binop.result = operand;
            ic->u.binop.operand = newOperand();
            ic->u.binop.operand->kind = VARIABLE;
            ic->u.binop.operand->u.name = strdup(node->children[0]->value);
            //printf("ASS in Exp ID, %d, %d\n", ic->u.binop.result->kind, ic->u.binop.operand->kind);

            insertLink(ic);
        }
        else if (node->children_num == 4)
        {
            //ID LP Args RP
            Operand* args[10];
            int i = 0;
            for (i = 0; i < 10; i++)
                args[i] = 0;

            translateArgs(node->children[2], args);

            //Deal with write.
            if (strcmp(node->children[0]->value, "write") == 0)
            {
                //printf("I'm writing...\n");
                InterCode* ic = newInterCode();
                ic->kind = WRITE_IC;
                ic->u.write.operand = args[0];
                insertLink(ic);
            }
            else
            {
                int i = 0;
                int j = 0;
                for (j = 0; args[j] != 0; j++);
                for (i = j - 1; i >= 0; i--)
                {
                    InterCode* ic = newInterCode();
                    ic->kind = ARG_IC;
                    ic->u.arg.operand = args[i];
                    insertLink(ic);
                }

                //int i = 0;
                //for (i = 0; args[i] != 0; i++)
                //{
                //	InterCode* ic = newInterCode();
                //	ic->kind = ARG_IC;
                //	ic->u.arg.operand = args[i];
                //	insertLink(ic);
                //}

                InterCode* ic = newInterCode();
                ic->kind = CALLFUNC_IC;
                ic->u.call.operand = operand;
                ic->u.call.name = strdup(node->children[0]->value);
                insertLink(ic);
            }

        }
        else
        {
            //ID LP RP
            assert(node->children_num == 3);
            //Deal with read
            if (strcmp(node->children[0]->value, "read") == 0)
            {
                //printf("I'm reading...\n");
                InterCode* ic = newInterCode();
                ic->kind = READ_IC;
                ic->u.read.operand = operand;
                insertLink(ic);
            }
            else
            {
                InterCode* ic = newInterCode();
                ic->kind = CALLFUNC_IC;
                ic->u.call.operand = operand;
                ic->u.call.name = strdup(node->children[0]->value);
                insertLink(ic);
            }
        }

    }
    else if(strcmp(node->children[0]->name, "INT") == 0)
    {
        //printf("Should be here\n");
        operand->kind = CONSTANT;
        operand->u.value = strtol(node->children[0]->value, 0, 10);
        //printf("----%d\n", operand->u.value);
        //printf("woo\n");
    }
    else
    {
        printf("%s\n", node->children[0]->name);
        //Do nothing.
    }
    //printf("exp over\n");
    return;

}

void translateArgs(Node* node, Operand** args)
{
    //printf("In Args\n");
    assert(strcmp(node->name, "Args") == 0);
    Operand* t1 = newVar();
    translateExp(node->children[0], t1);
    int i = 0;
    for(i = 0; args[i] != 0; i++);
    //printf("Here i is %d\n", i);
    args[i] = t1;

    if (node->children_num == 3)
        translateArgs(node->children[2], args);

    return;
}

void printInFile(string filename)
{
    FILE* file = fopen(filename, "w");
    if (file != NULL)
    {
        InterCodes* lic = head;
        for (lic = head; lic != 0; lic = lic->next)
            generateCode(file, lic->code);
    }
    else
    {
        printf("Error openning file!\n");
        exit(-1);
    }

    fclose(file);
}

void generateCode(FILE* f, InterCode* ic)
{
    //printf("Writing a code  %d\n", ic->kind);
    switch(ic->kind)
    {
    case FUNCTION_IC:
        fprintf(f, "FUNCTION %s :\n", ic->u.function.name);
        break;
    case PARAM_IC:
        fprintf(f, "PARAM %s\n", ic->u.param.name);
        break;
    case LABEL_IC:
        fprintf(f, "LABEL %s :\n", ic->u.label.name);
        break;
    case LABEL_GOTO_IC:
        fprintf(f, "GOTO %s\n", ic->u.label_goto.name);
        break;
    case CALLFUNC_IC:
        fprintf(f, "%s := CALL %s\n", ic->u.call.operand->u.name, ic->u.call.name);
        break;
    case RETURN_IC:
        if(ic->u.ret.operand->kind == CONSTANT)
            fprintf(f, "RETURN #%d\n", ic->u.ret.operand->u.value);
        else
            fprintf(f, "RETURN %s\n", ic->u.ret.operand->u.name);
        break;
    case WRITE_IC:
        //printf("I'm in writing ic, %d\n", ic->u.write.operand->kind);
        if(ic->u.write.operand->kind == CONSTANT)
            fprintf(f, "WRITE #%d\n",ic->u.write.operand->u.value);
        else
            fprintf(f, "WRITE %s\n",ic->u.write.operand->u.name);
        break;
    case READ_IC:
        fprintf(f, "READ %s\n",ic->u.read.operand->u.name);
        break;
    case ARG_IC:
        if(ic->u.arg.operand->kind==CONSTANT)
            fprintf(f,"ARG #%d\n",ic->u.arg.operand->u.value);
        else
            fprintf(f,"ARG %s\n",ic->u.arg.operand->u.name);
        break;
    case DEC_IC:
        fprintf(f, "DEC %s %d\n", ic->u.dec.operand->u.name, ic->u.dec.size);
        break;
    case COND_IC:
        if(ic->u.cond.operand1->kind==CONSTANT&&ic->u.cond.operand2->kind==CONSTANT)
            fprintf(f,"IF #%d %s #%d GOTO %s\n",ic->u.cond.operand1->u.value,\
                    ic->u.cond.op,ic->u.cond.operand2->u.value,ic->u.cond.name);
        else if(ic->u.cond.operand1->kind == CONSTANT)
            fprintf(f,"IF #%d %s %s GOTO %s\n",ic->u.cond.operand1->u.value,\
                    ic->u.cond.op,ic->u.cond.operand2->u.value,ic->u.cond.name);
        else if(ic->u.cond.operand2->kind == CONSTANT)
            fprintf(f,"IF %s %s #%d GOTO %s\n",ic->u.cond.operand1->u.value,\
                    ic->u.cond.op,ic->u.cond.operand2->u.value,ic->u.cond.name);
        else
            fprintf(f,"IF %s %s %s GOTO %s\n",ic->u.cond.operand1->u.value,\
                    ic->u.cond.op,ic->u.cond.operand2->u.value,ic->u.cond.name);
        break;
    case MINUS_IC:
        //printf("I'm in minus ic... %d   %d  \n", ic->u.triop.operand1->kind, ic->u.triop.operand2->kind);
        if(ic->u.triop.operand1->kind==CONSTANT&&ic->u.triop.operand2->kind==CONSTANT)
        {
            //printf("WAHAHA\n");
            fprintf(f,"%s := #%d - #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.value);
        }
        else if(ic->u.triop.operand1->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d - %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.name);
        }
        else if(ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := %s - %d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.value);
        }
        else
        {
            fprintf(f,"%s := %s - %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.name);
        }
        break;
    case ADD_IC:
        if(ic->u.triop.operand1->kind==CONSTANT&&ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d + #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.value);
        }
        else if(ic->u.triop.operand1->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d + %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.name);
        }
        else if(ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := %s + #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.value);
        }
        else
        {
            fprintf(f,"%s := %s + %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.name);
        }
        break;
    case SUB_IC:
        if(ic->u.triop.operand1->kind==CONSTANT&&ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d - #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.value);
        }
        else if(ic->u.triop.operand1->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d - %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.name);
        }
        else if(ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := %s - #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.value);
        }
        else
        {
            fprintf(f,"%s := %s - %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.name);
        }
        break;
    case MUL_IC:
        if(ic->u.triop.operand1->kind==CONSTANT&&ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d * #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.value);
        }
        else if(ic->u.triop.operand1->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d * %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.name);
        }
        else if(ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := %s * #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.value);
        }
        else
        {
            fprintf(f,"%s := %s * %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.name);
        }
        break;
    case DIV_IC:
        if(ic->u.triop.operand1->kind==CONSTANT&&ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d / #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.value);
        }
        else if(ic->u.triop.operand1->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d / %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.value,ic->u.triop.operand2->u.name);
        }
        else if(ic->u.triop.operand2->kind==CONSTANT)
        {
            fprintf(f,"%s := %s / #%d\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.value);
        }
        else
        {
            fprintf(f,"%s := %s / %s\n",ic->u.triop.result->u.name,\
                    ic->u.triop.operand1->u.name,ic->u.triop.operand2->u.name);
        }
        break;
    case ASSIGN_IC:
        //printf("HERE\n");
        if(ic->u.binop.operand->kind==CONSTANT)
        {
            fprintf(f,"%s := #%d\n",ic->u.binop.result->u.name,\
                    ic->u.binop.operand->u.value);
        }
        else
        {

            //printf("%dvvv%s\n", ic->u.binop.operand->kind, ic->u.binop.result->u.name);
            //printf("HERE\n");
            //printf("sss%s\n", ic->u.binop.operand->u.name);
            fprintf(f,"%s := %s\n",ic->u.binop.result->u.name,\
                    ic->u.binop.operand->u.name);
        }
        break;
    default:
        printf("Wrong kind: %d\n", ic->kind);
        break;
    }
}
