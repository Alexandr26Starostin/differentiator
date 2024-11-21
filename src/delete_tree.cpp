#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "delete_tree.h"

diff_error_t delete_tree (node_t* node)
{
	assert (node);

	if (node -> left != NULL)
	{
		delete_tree (node -> left);
	}

	if (node -> right != NULL)
	{
		delete_tree (node -> right);
	}

	free (node);

	return NOT_ERROR;
}