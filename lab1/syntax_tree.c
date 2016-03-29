#include "syntax_tree.h"

struct Node* create_node(char *name, char *value, bool is_leaf, uint32_t line_num, uint32_t children_num, ...)
{
	struct Node *this = (struct Node *)malloc(sizeof(struct Node));
	memset(this, 0, sizeof(struct Node));

	this->name = (char *)malloc(strlen(name) + 1);
	strcpy(this->name, name);
	this->value = (char *)malloc(strlen(value) + 1);
	strcpy(this->value, value);
	this->line_num = line_num;
	this->is_leaf = is_leaf;
	this->children_num = children_num;
	// for multies children nodes.
	va_list arg_ptr;

	if (children_num > 0)
	{
		this->children = (struct Node **)malloc(sizeof(struct Node *) * children_num);
		va_start(arg_ptr, children_num);
		uint32_t index = 0;
		for (index = 0; index < children_num; ++index)
		{
			struct Node *child = va_arg(arg_ptr, struct Node *);
			this->children[index] = child;
		}
		va_end(arg_ptr);
	}
	else
		this->children = NULL;

	return this;
}
