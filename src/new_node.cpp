#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "new_node.h"

node_t* create_new_node (type_arg_t type, double value, node_t* left, node_t* right, node_t* parent, const char* file, int line)
{
	assert (file);

	node_t* new_node = (node_t*) calloc (1, sizeof (node_t));

	if (new_node == NULL)
	{
		printf ("%s:%d Not memory for new_node\n", file, line);
		return NULL;
	}

	new_node -> type   = type;
	new_node -> left   = left;
	new_node -> right  = right;
	new_node -> parent = parent;

	switch (type)
	{
		case NUM:
		{
			(new_node -> value).value_num  = value;
			break;
		}

		case VAR:
		{
			(new_node -> value).value_var = (char) value;
			break;
		}

		case OP:
		{
			(new_node -> value).value_op  = (operation_t) value;
			break;
		}

		default:
		{
			printf ("type = %d not find\n", type);
			return NULL;
			break;
		}
	}

	// if (type == VAR)
	// {

	// }

	return new_node;
}