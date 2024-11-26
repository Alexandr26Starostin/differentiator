#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "table_of_var.h"
#include "read_and_print_formula.h"

#define NAME_OPERATION_(operation, name) \
	case operation:                             \
	{                                      \
		name_operation = name;   \
		break;                               \
	}

#define WRITE_NAME_OPERATION_(operation, name)     \
	if (strcmp (name_operation, name) == 0)               \
	{                                                 \
		(node -> value).value_op = operation;               \
		continue;                                    \
	}

static diff_error_t create_tree (node_t* node, char* str_formula, size_t* ptr_index_str, table_t* table);

//-----------------------------------------------------------------------------------------------------

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

	switch (node -> type)
	{
		case NUM:
		{
			printf ("(%lg)", (node -> value).value_num);
			break;
		}

		case VAR:
		{
			printf ("(%s)", (node -> value).value_var);
			break;
		}

		case OP:
		{
			const char* name_operation = NULL;

			switch ((node -> value).value_op)
			{
				NAME_OPERATION_(ADD,  "+");
				NAME_OPERATION_(SUB,  "-");
				NAME_OPERATION_(DIV,  "/");
				NAME_OPERATION_(MUL,  "*");
				NAME_OPERATION_(SIN,  "sin");
				NAME_OPERATION_(COS,  "cos");
				NAME_OPERATION_(SH,   "sh");
				NAME_OPERATION_(CH,   "ch");
				NAME_OPERATION_(SQRT, "sqrt");

				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					return NOT_FIND_OP;
				}
			}

			printf ("%s", name_operation);

			break;
		}

		default:
		{
			printf ("type = %d not find\n", node -> type);
			return NOT_FIND_TYPE_ARG;
			break;
		}
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

//----------------------------------------------------------------------------------------------------------

diff_error_t read_formula (node_t* node, table_t* table)
{
	assert (node);
	assert (table);

	char str_formula[MAX_LEN_STR_FORMULA] = "";

	printf ("Введите формулу:\n");

	scanf ("%[^'\n']", str_formula);
	getchar ();
	//printf ("%s\n", str_formula);

	size_t index_str = 0;

	diff_error_t status = create_tree (node, str_formula, &index_str, table);

	return status;
}

static diff_error_t create_tree (node_t* node, char* str_formula, size_t* ptr_index_str, table_t* table)
{
	assert (node);
	assert (str_formula);
	assert (ptr_index_str);
	assert (table);

	diff_error_t status = NOT_ERROR;

	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

	if (str_formula[*ptr_index_str] == '(')
	{
		
		*ptr_index_str += 1;

		node -> left = create_new_node (NUM, 0, NULL, NULL, node, __FILE__, __LINE__);
		if (node -> left == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

		status = create_tree ((node -> left), str_formula, ptr_index_str, table);
		if (status) {return status;}

		while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

		if (str_formula[*ptr_index_str] == ')') {*ptr_index_str += 1;}
	}

	//------------------------------------------------------------------------------------------------------  

	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

	char name_operation[MAX_LEN_STR_FORMULA] = "";
	sscanf (str_formula + *ptr_index_str, "%[^0123456789]", name_operation);

	if (strlen (name_operation) == 0)
	{
		sscanf (str_formula + *ptr_index_str, "%[0123456789]", name_operation);

		//printf ("%s\n", name_operation);

		node -> type = NUM;
		(node -> value).value_num = strtod (name_operation, NULL);

		*ptr_index_str += strlen (name_operation);
	}
	else
	{
		sscanf (str_formula + *ptr_index_str, "%[^)]", name_operation);

		//printf ("%s\n", name_operation);

		if (strchr (name_operation, '(') == NULL)   //VAR
		{
			//printf ("\n\n%s\n\n", name_operation);

			node -> type = VAR;

			status = add_var_in_table (table, name_operation, 0);
			if (status) {return status;}

			strcpy ((node -> value).value_var, name_operation);

			*ptr_index_str += strlen (name_operation); //Временно +1, Но после появления таблицы переменных длина их имён будет произвольной.
		}
		else         //OP
		{
			sscanf (str_formula + *ptr_index_str, "%[^ (]", name_operation);

			//printf ("!-%s\n", name_operation);

			node -> type = OP;

			for (size_t quantity_operations = 1; quantity_operations == 1; quantity_operations++)
			{
				WRITE_NAME_OPERATION_(ADD,  "+");
				WRITE_NAME_OPERATION_(SUB,  "-");
				WRITE_NAME_OPERATION_(MUL,  "*");
				WRITE_NAME_OPERATION_(DIV,  "/");
				WRITE_NAME_OPERATION_(SIN,  "sin");
				WRITE_NAME_OPERATION_(COS,  "cos");
				WRITE_NAME_OPERATION_(SH,   "sh");
				WRITE_NAME_OPERATION_(CH,   "ch");
				WRITE_NAME_OPERATION_(SQRT, "sqrt");
			}

			*ptr_index_str += strlen (name_operation);
		}
	}

	//--------------------------------------------------------------------------------------------------------

	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

	if (str_formula[*ptr_index_str] == '(')
	{
		*ptr_index_str += 1;

		node -> right = create_new_node (NUM, 0, NULL, NULL, node, __FILE__, __LINE__);
		if (node -> right == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

		status = create_tree ((node -> right), str_formula, ptr_index_str, table);
		if (status) {return status;}

		while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

		if (str_formula[*ptr_index_str] == ')') {*ptr_index_str += 1;}
	}

	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

	return status;
}