#include "syntax_tree.h"
#include <stdio.h>

struct Node* create_node(char *name, char *value, bool is_token, uint32_t line_num, uint32_t children_num, ...)
{
	struct Node *this = (struct Node *)malloc(sizeof(struct Node));
	memset(this, 0, sizeof(struct Node));

	this->name = (char *)malloc(strlen(name) + 1);
	// do not try to assign directly.
	strcpy(this->name, name);
//	printf("node name %s\n", this->name);
	this->value = (char *)malloc(strlen(value) + 1);
	strcpy(this->value, value);
	this->line_num = line_num;
	this->is_token = is_token;
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

void print_tree(struct Node *root, int blank_num)
{
    
	if (strcmp(root->name, "") != 0)
	{
        uint32_t k = 0;
	    for (k = 0; k < blank_num; ++k)
		    printf(" ");
		printf("%s", root->name);
        if (strcmp(root->name, "ID") == 0)
            printf(": %s", root->value);
        else if (strcmp(root->name, "TYPE") == 0)
            printf(": %s", root->value);
        else if (strcmp(root->name, "INT") == 0)
        {
            // should use strtol() instead of atoi() because of oct and hex.
            if (root->value[0] == '0')
            {
                // hex int
                if (root->value[1] == 'x' || root->value[1] == 'X')
                    printf(": %d", strtol(root->value, NULL, 16));
                else
                    // oct int
                    printf(": %d", strtol(root->value, NULL, 8));
            }
            else
                printf(": %d", strtol(root->value, NULL, 10));
        }
        else if (strcmp(root->name, "FLOAT") == 0)
            printf(": %f", atof(root->value));
		if (!root->is_token)
			printf(" (%d)\n", root->line_num);
		else
			printf("\n");
	}

	uint32_t i = 0;
	for (i = 0; i < root->children_num; ++i)
	{
		if (root->children[i] != NULL)
			print_tree(root->children[i], blank_num + 2);
	}

}
