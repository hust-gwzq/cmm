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
	bool is_token;
	struct Node **children;
};

typedef struct Node Node;

//typedef struct Node* node_ptr;

struct Node* create_node(char *name, char *value, bool is_token, uint32_t line_num, uint32_t children_num, ...);

void print_tree(struct Node *root, int blank_num);

#endif
