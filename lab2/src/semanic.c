#include "../include/semanic.h"

/*int kind in this file:
	0 = int
	1 = float
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

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Var_* Var;
typedef struct Func_* Func;
typedef struct Args_* Args;
typedef struct Item_* Item;
typedef struct ExpReturnType_ ExpReturnType_;

struct Type_ typeInt;
struct Type_ typeFloat;

Item hashTable[HASHTABLELENGTH];

Type thisReturnType;

uint32_t hash_pjw(char* name)
{
	uint32_t val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~HASHTABLELENGTH) val = (val ^ (i >> 12)) & HASHTABLELENGTH;
	}
	return val;
}

void init_hash_table()
{
	int i = 0;
	for (i = 0; i < HASHTABLELENGTH; i++)
	{
		hashTable[i] = (Item)malloc(sizeof(struct Item_));
		hashTable[i]->next = 0;
	}
	return 0;
}

void insert_hash_table(Item item)
{
	//Insert in the hash table.
	int index = hash_pjw(item->name);

	Item p = hashTable[index];
	item->next = p->next;
	p->next = item;
}

int checkIfUsed(char* n)
{
	//printf("In checkIfUsed\n");
	//printf("I am checking %s\n", n);
	int index = hash_pjw(n);
	Item p = hashTable[index];
	if(p == 0)
		return -1;
	while(p != 0)
	{
		if (p->name != 0 && 0 == strcmp(p->name, n) != 0)
			return 1;
		p = p->next;
	}
	if(p == 0)
		return 0;
}

Item getFromHashTable(char* n)
{
	int index = hash_pjw(n);
	Item p = hashTable[index];
	while (p != 0)
	{
		if (p->name != 0 && 0 == strcmp(p->name, n) != 0)
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
	typeInt.kind = BASIC;
	typeInt.u.basic = 0;
	typeFloat.kind = BASIC;
	typeFloat.u.basic = 1;
	init_hash_table();
	//Program -> ExtDefList
	
	extDefList(node->children[0]);
	return;
}

void extDefList(struct Node* node)
{
	//printf("In ExtDefList\n");
	//ExtDefList -> EMPTY
	//			  | ExfDef ExtDefList

	if(0 == node->children_num)//EMPTY
		return;
	else
	{
		extDef(node->children[0]);
		extDefList(node->children[1]);
		return;
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
  		Item item = (Item)malloc(sizeof(Item_));
  		item->name = f->name;//Just a pointer.
  		item->kind = 4;//Function.
  		f->returnType = thisType;
  		item->u.f = f;

  		insert_hash_table(item);

  		thisReturnType = thisType;

  		compSt(node->children[2]);
  	}
  	return;
}

void extDecList(struct Node* node, Type t)
{
	//printf("In ExtDecList\n");
	//ExtDecList -> VarDec
  	//	 		  | VarDec COMMA ExtDecList
  	if (node->children_num == 1)
  	{
          //printf("name is %s", node->children[0]->name);
  		Var v = varDec(node->children[0]);
  		if (checkIfUsed(v->name) == 1)
  			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->name);
  		else
  		{
  			//Insert in the hash table.
  			Item item = (Item)malloc(sizeof(Item_));
	  		item->name = v->name;
	  		item->kind = 6;//Var.
	  		item->u.v->type = t;
	  		item->u.v = v;

	  		insert_hash_table(item);
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
  			typeInt.kind = BASIC,
			typeInt.u.basic = 0;
  			return &typeInt;
  		}
  		else
  		{
  			typeFloat.kind = BASIC,
			typeFloat.u.basic = 1;
  			return &typeFloat;
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
	//StructSpecifier -> STRUCT OptTag LC DefList RC
	//				   | STRUCT Tag
	if (node->children_num == 5)
	{
		//STRUCT OptTag LC DefList RC
		Item item = optTag(node->children[1]);

		item->u.type->u.structure->tail = defList(node->children[3], 1);

		return item->u.type;
	}
	else
	{
		//STRUCT Tag
		Item item = getFromHashTable(node->children[1]->children[0]->value);


		//PRINT HASH TABLE
		/*int i=0;
		for (i=0;i<HASHTABLELENGTH; i++)
		{
			Item it = hashTable[i];
			while(it != NULL && it->name != NULL)
				printf("%s          ", it->name);
			printf("\n");
		}*/

		if (item == 0)
		{
			printf("Error type 17 at Line %d: Undefined structure.\n", node->children[1]->line_num);
				return 0;
		}
		else
		{
			if (item->kind == 5)
				return item->u.type;
			else
			{
				printf("Error type 17 at Line %d: Undefined structure.\n", node->children[1]->line_num);
				return 0;
			}
		}
	}
}

Item optTag(struct Node* node)
{
	//printf("In OptTag\n");
	//OptTag -> EMPTY
  	//    	  | ID

  	if (node->children_num == 1)
  	{
  		//ID
  		if(checkIfUsed(node->children[0]->value) == 1)
  			printf("Error type 16 at Line %d: Name is used by other structures \"%s\"\n", node->children[0]->line_num, node->children[0]->value);

  		//Insert in the hash table.
  		FieldList fl = (FieldList)malloc(sizeof(FieldList_));
  		fl->name = node->children[0]->value;
  		fl->type = 0;
  		fl->tail = 0;
  		Type t = (Type)malloc(sizeof(Type_));
  		t->kind = STRUCTURE;
  		t->u.structure = fl;
  		Item item = (Item)malloc(sizeof(Item_));
  		item->name = fl->name;
  		item->kind = 5;
  		item->u.type = t;

  		insert_hash_table(item);

  		return item;
  	}
  	else
  	{

  		FieldList fl = (FieldList)malloc(sizeof(FieldList_));
  		fl->name = 0;
  		fl->type->kind = STRUCTURE;
  		fl->tail = 0;
  		Type t = (Type)malloc(sizeof(Type_));
  		t->kind = STRUCTURE;
  		t->u.structure = fl;
  		Item item = (Item)malloc(sizeof(Item_));
  		item->name = item->u.type->u.structure->name;
  		return item;

  	}
}

void tag(struct Node* node)
{
	//printf("In Tag\n");
	//Tag -> ID
	if(checkIfUsed(node->children[0]->value) == 1)
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
	if (checkIfUsed(node->children[0]->value) == 1)//Check ID.
		printf("Error type 4 at Line %d: Redefined function \"%s\"\n", node->children[0]->line_num,node->children[0]->value);
	if (node->children_num == 4)
	{
		//ID LP VarList RP
		Func f = (Func)malloc(sizeof(Func_));
		f->name = node->children[0]->value;
		f->returnType = 0;
		f->args = varList(node->children[2]);
		return f;
	}
	else
	{
		Func f = (Func)malloc(sizeof(Func_));
		f->name = node->children[0]->value;
		f->returnType = 0;
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

	if (checkIfUsed(v->name) == 1)
		printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[1]->line_num, node->children[1]->name);

	//Insert in the hash table.
	Item item = (Item)malloc(sizeof(Item_));
	item->name = v->name;
	item->kind = 6;//var.
	item->u.v = v;

	insert_hash_table(item);

	Args a = (Args)malloc(sizeof(Args_));
	a->v = v;
	a->next = 0;
	return a;
}

//------------------Statements
void compSt(struct Node* node)
{
	//printf("In CompSt\n");
	//CompSt -> LC DefList StmtList RC
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
	//printf("In Stmt\n");
	//Stmt -> Exp SEMI
  	//		| CompSt
  	//		| RETURN Exp SEMI
  	//		| IF LP Exp RP Stmt
  	//		| IF LP Exp RP Stmt ELSE Stmt
  	//		| WHILE LP Exp RP Stmt
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
  			printf("Error type 8 at Line %d: Different types in the return.\n", node->children[1]->line_num);

  	}
  	else if (node->children_num == 7)
  	{
  		//IF LP Exp RP Stmt ELSE Stmt
  		ExpReturnType_ ert = exp(node->children[2]);
  		if (ert.kind != 7 && ert.type != &typeInt)
  			printf("Error type 7 at Line %d: Mismatched operands\n",node->children[0]->line_num);

  		stmt(node->children[4]);
  		stmt(node->children[6]);
  	}
  	else
  	{

  		//WHILE LP Exp RP Stmt
  		//IF LP Exp RP Stmt
  		exp(node->children[2]);

  		ExpReturnType_ ert = exp(node->children[2]);
  		if (ert.kind != 7 && ert.type != &typeInt)
  			printf("Error type 7 at Line %d: Mismatched operands\n",node->children[0]->line_num);
  		stmt(node->children[4]);

  	}
}

//------------------Local Definitions
FieldList defList(struct Node* node, int varOrStruct)
{
    if (strcmp(node->name, "") ==0)
    return 0;
	//printf("In DefList\n");
	//DefList : EMPTY
 	// 		  | Def DefList

 	if (node->children_num == 0)
 	{
 		//printf("EMPTY in DefList\n");
 		return 0;
 	}
 	else
 	{
 		FieldList fl = def(node->children[0], varOrStruct);

 		//printf("IDONTKNOW %d\n", varOrStruct);

 		if (varOrStruct == 0)
 			defList(node->children[1], varOrStruct);
 		else
 		{

 			if (fl != 0)
 			{
 				FieldList flfl = fl;
 				while (flfl->tail != 0)
 					flfl =flfl->tail;
 				flfl->tail = defList(node->children[1], varOrStruct);
 			}
 		}
 		return fl;
 	}
}

FieldList def(struct Node* node, int varOrStruct)
{
	//printf("In Def\n");
	//Def -> Specifier DecList SEMI
	Type t = specifier(node->children[0]);
	FieldList fl = decList(node->children[1], t, varOrStruct);

	return fl;
}

FieldList decList(struct Node* node, Type t, int varOrStruct)
{
	//printf("In DecList\n");
	//DecList -> Dec
  	//		   | Dec COMMA DecList
  	FieldList fl =dec(node->children[0], 0, varOrStruct);
  	if (node->children_num == 1)

 		return fl;
 	else
 	{
 		if (varOrStruct == 1)
 			fl->tail = decList(node->children[2], t, varOrStruct);
 		else
 		{
 			decList(node->children[2], t, varOrStruct);
 		}
 		return fl;
 	}
}

FieldList dec(struct Node* node, Type t, int varOrStruct)
{
	//printf("In Dec\n");
	//Dec -> VarDec
 	// 	   | VarDec ASSIGNOP Exp

 	Var v = varDec(node->children[0]);
 	if (checkIfUsed(v->name) == 1)
 	{
 		if (varOrStruct == 1)
 			printf("Error type 15 at Line %d: Redefined field in the struct.\n", node->children[0]->line_num);
 		else
 		{
 			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->children[0]->line_num, v->name);
 		}
 	}

 	v->type = t;

 	FieldList fl = 0;

 	if (varOrStruct == 1)
 	{
 		//Insert in the hash table.
 		fl = (FieldList)malloc(sizeof(FieldList_));
 		fl->name = v->name;
 		fl->type = v->type;
 		fl->tail = 0;

 		Item item = (Item)malloc(sizeof(Item_));
 		item->name = fl->name;
 		item->kind = 9;
 		item->u.fl = fl;

 		insert_hash_table(item);
 	}
 	else
 	{
 		//Insert in the hash table.
 		Item item = (Item)malloc(sizeof(Item_));
 		item->name = v->name;
 		item->kind = 6;
 		item->u.v = v;

 		insert_hash_table(item);
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

 	return fl;
}

ExpReturnType_ exp(struct Node* node)
{
	//printf("In Exp\n");
	//Exp -> Exp ASSIGNOP Exp
  	//	  	| Exp AND Exp
  	//	  	| Exp OR Exp
  	//	  	| Exp RELOP Exp
  	//	  	| Exp PLUS Exp
  	//	  	| Exp MINUS Exp
  	//	  	| Exp STAR Exp
  	//	  	| Exp DIV Exp
  	//	  	| LP Exp RP
  	//	  	| MINUS Exp
  	//	  	| NOT Exp
  	//	  	| ID LP Args RP
  	//	  	| ID LP RP
  	//	  	| Exp LB Exp RB
  	//	  	| Exp DOT ID
  	//	  	| ID
  	//	  	| INT
  	//	  	| FLOAT

  	ExpReturnType_ ert;

	if (strcmp(node->children[0]->name, "Exp") == 0)
	{
		//printf("In the first is Exp\n");
		ExpReturnType_ ert1 = exp(node->children[0]);
		if (strcmp(node->children[1]->name, "ASSIGNOP") == 0)
		{
			//printf("In assignop\n");
			//Exp ASSIGNOP Exp
			if (ert1.kind != 7 && ert1.kind != 6) {
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
			} else
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
					printf("Error type 7 at Line %d: Mismatched operands.\n", node->children[1]->line_num);

				if (ert2.type->kind != BASIC || ert2.type->u.basic != 1)
					printf("Error type 7 at Line %d: Mismatched operands.\n", node->children[1]->line_num);
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
				ert.type = &typeInt;
				ert.flag = ert1.flag;
			}
			else
			{
				//printf("ert1.kind  %d    ert2.kind  %d \n", ert1.kind, ert2.kind);

				ert.kind = 7;
				printf("Error type 7 at Line %d: Mismatched operands.\n", node->children[1]->line_num);
			}
			return ert;
		}
		else if (strcmp(node->children[1]->name, "LB") == 0)
		{
			//Exp LB Exp RB
			//printf("In LB\n");
			ExpReturnType_ ert2 = exp(node->children[2]);

			if (ert1.kind == 7 || ert2.kind == 7)
				ert.kind = 7;
			else if (ert1.flag == 2 && ert2.type == &typeInt)
			{
				//Array
				ert.kind = ert1.kind;
				ert.type = ert1.type->u.array.elem;

				if (ert.type == 0)
					ert.flag = -1;
				else if (ert.type  == &typeInt)
					ert.flag = 0;
				else if (ert.type  == &typeFloat)
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
					printf("Error type 10 at Line %d: Not an array.\n", node->children[1]->line_num);

				if (ert2.type != &typeInt)
					printf("Error type 12 at Line %d: Not an int.\n", node->children[1]->line_num);
			}
			return ert;
		}
		else
		{
			//Exp DOT ID

			//printf("ffffff%d\n", ert1.flag);
			if (ert1.kind == 7)
				return ert1;
			else if (ert1.flag == 3)
			{
				//printf("why i'm here\n");
				FieldList fl = ert1.type->u.structure->tail;
				while (fl != 0)
				{
					if (strcmp(fl->name, node->children[2]->name) == 0)
					{
						ert1.type = fl->type;
						if (ert1.type == 0)
							ert1.flag = -1;
						else if (ert1.type  == &typeInt)
							ert1.flag = 0;
						else if (ert1.type  == &typeFloat)
							ert1.flag = 1;
						else if (ert1.type->kind == ARRAY)
							ert1.flag = 2;
						else
						{
							ert1.flag = 3;
						}
						break;
					}
					fl = fl->tail;
				}

				if (fl == 0)
				{
					ert1.kind = 7;
					printf("Error type 14 at Line %d: Non-existent field\n", node->children[1]);
				}
			}

			else
			{
				ert1.kind = 7;
				printf("Error type 13 at Line %d: Illegal use of \'.\'\n", node->children[1]->line_num);
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
		//printf("In first is ID\n");
		Item item = getFromHashTable(node->children[0]->value);
		if (item == 0)
			ert.kind = 7;
		else
		{
			ert.kind = item->kind;
			switch (item->kind)
			{
				case 6: ert.type = item->u.v->type; break;//var
				case 4: ert.type = item->u.f->returnType; break;//function
				case 9: ert.type = 0; break;
			}

			//printf("ok\n");
			if (ert.type == 0)
				ert.flag = -1;
			else if (ert.type  == &typeInt)
				ert.flag = 0;
			else if (ert.type  == &typeFloat)
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
			if (item == 0)
				printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", node->children[0]->line_num, node->children[0]->value);

			return ert;
		}
		else
		{
			//ID LP RP
			//ID LP Args RP
			if (item == 0)
			{
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", node->children[0]->line_num, node->children[0]->value);
				return ert;
			}
			if (item->kind != 4) {
				printf("Error type 11 at Line %d: \"%s\" is not a function\n", node->children[0]->line_num, item->name);
				ert.kind = 7;
				return ert;
			}
			ert.kind = 10;
			ert.type = item->u.f->returnType;



			Args a = item->u.f->args;
			if (node->children_num == 4)
			{	//ID LP Args RP
				if (args(node->children[2], a) == 0)
					ert.kind = 7;
			}
			return ert;

		}
	}
	else if (strcmp(node->children[0]->name, "INT") == 0)
	{
		ert.type = &typeInt;
		ert.flag = 0;
		return ert;
	}
	else
	{
		ert.type = &typeFloat;
		ert.flag = 1;
		return ert;
	}

}

int args(struct Node* node, Args a)
{
	//printf("In Args\n");
	//Args -> Exp COMMA Args
  	//    	| Exp
  	ExpReturnType_ ert = exp(node->children[0]);

  	if(node->children_num == 1)
  	{
  		if (ert.kind == 7)
  			return 0;
  		else if (a == 0 || ert.type == a->v->type)
  		{
  			printf("Error type 9 at Line %d: Different arguments to function.\n", node->children[0]->line_num);
  			return 0;
  		}
  		else
  		{
  			printf("Error type 9 at Line %d: Different arguments to function.\n", node->children[0]->line_num);
  			return 0;
  		}
  	}
  	else
  	{

  		return args(node->children[2], a->next);
  	}
}
