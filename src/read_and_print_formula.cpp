#include <stdio.h>
#include <assert.h>

#include "const_in_diff.h"
#include "read_and_print_formula.h"

#define NAME_OPERATION_(operation, name) \
	case operation:                             \
	{                                      \
		name_operation = name;   \
		break;                               \
	}

diff_error_t print_formula (node_t* node)
{
	assert (node);

	diff_error_t status = NOT_ERROR;

	if (node -> type == OP)
	{
		printf ("(");
	}

	//-------------------------------------------------------------

	if (node -> left != NULL)
	{
		status = print_formula (node -> left);

		if (status) {return status;}
	}

	//-------------------------------------------------------------

	if (node -> type == OP)
	{
		const char* name_operation = NULL;

		switch (node -> value)
		{
			NAME_OPERATION_(ADD,  "+");
			NAME_OPERATION_(SUB,  "-");
			NAME_OPERATION_(DIV,  "/");
			NAME_OPERATION_(SIN,  "sin");
			NAME_OPERATION_(COS,  "cos");
			NAME_OPERATION_(SH,   "sh");
			NAME_OPERATION_(CH,   "ch");
			NAME_OPERATION_(SQRT, "sqrt");

			default:
			{
				printf ("Not find operation = %ld", node -> value);
				return NOT_FIND_OP;
			}
		}

		printf ("%s", name_operation);
	}
	
	else if (node -> type == VAR)
	{
		printf ("%c", (char) node -> value);
	}

	else if (node -> type == NUM)
	{
		printf ("%ld", node -> value);
	}

	else 
	{
		printf ("type = %d not find\n", node -> type);
		return NOT_FIND_TYPE_ARG;
	}

	//-------------------------------------------------------------

	if (node -> right != NULL)
	{
		status = print_formula (node -> right);

		if (status) {return status;}
	}

	//-------------------------------------------------------------

	if (node -> type == OP)
	{
		printf (")");
	}

	return NOT_ERROR;
}