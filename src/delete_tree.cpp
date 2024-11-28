#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "delete_tree.h"

diff_error_t delete_tree (node_t* node)
{
	if (node == NULL)
	{
		return NOT_ERROR;
	}

	delete_tree (node -> left);
	delete_tree (node -> right);
	
	free (node);

	return NOT_ERROR;
}