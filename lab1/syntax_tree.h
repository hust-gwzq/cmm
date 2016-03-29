#ifndef __SYNTAX_TREE_H__
#define __SYNTAX_TREE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

extern int yylineno;
extern int errorA_num;

struct Node
{
	char *name;
	char *value;
	uint32_t line_num;
	uint32_t children_num;
	bool is_leaf;
	struct Node **children;
};

typedef struct Node Node;

//typedef struct Node* node_ptr;

//struct Node* create_node(char *, char *, uint32_t , bool , uint32_t , ...);

//void print_tree(node_ptr root);

#endif
