#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "new_node.h"

node_t* create_new_node (type_arg_t type, long value, node_t* left, node_t* right, node_t* parent, const char* file, int line)
{
	assert (file);

	node_t* new_node = (node_t*) calloc (1, sizeof (node_t));

	if (new_node == NULL)
	{
		printf ("%s:%d Not memory for new_node\n", file, line);
		return NULL;
	}

	new_node -> type   = type;
	new_node -> value  = value;
	new_node -> left   = left;
	new_node -> right  = right;
	new_node -> parent = parent;

	// if (type == VAR)
	// {

	// }

	return new_node;
}