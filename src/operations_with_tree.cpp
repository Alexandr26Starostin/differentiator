#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "const_in_diff.h"
#include "table_of_var.h"
#include "new_node.h"
#include "operations_with_tree.h"

static diff_error_t read_new_number_in_var (table_t* table, size_t index);

double evaluate_tree (node_t* node, table_t* table)
{
	assert (node);
	assert (table);

	switch (node -> type)
	{
		case NUM:
		{
			return (node -> value).value_num;
			break;
		}

		case VAR:
		{
			return *find_var_in_table (table, (node -> value).value_var);
			break;
		}

		case OP:
		{
			switch ((node -> value).value_op)
			{
				case ADD:
				{
					return evaluate_tree (node -> left, table) + evaluate_tree (node -> right, table);
					break;
				}

				case SUB:
				{
					return evaluate_tree (node -> left, table) - evaluate_tree (node -> right, table);
					break;
				}

				case MUL:
				{
					return evaluate_tree (node -> left, table) * evaluate_tree (node -> right, table);
					break;
				}

				case DIV:
				{
					return evaluate_tree (node -> left, table) / evaluate_tree (node -> right, table);
					break;
				}

				case SIN:
				{
					return sin (evaluate_tree (node -> right, table));
					break;
				}

				case COS:
				{
					return cos (evaluate_tree (node -> right, table));
					break;
				}

				case SH:
				{
					return sinh (evaluate_tree (node -> right, table));
					break;
				}

				case CH:
				{
					return cosh (evaluate_tree (node -> right, table));
					break;
				}

				case SQRT:
				{
					return sqrt (evaluate_tree (node -> right, table));
					break;
				}

				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					break;
				}
			}

			break;
		}

		default:
		{
			printf ("type = %d not find\n", node -> type);
			break;
		}
	}

	return 0;
}

double calculator (node_t* node, table_t* table)
{
	assert (node);
	assert (table);

	for (size_t index = 0; index < table -> index_free_var; index++)
	{
		read_new_number_in_var (table, index);
	}

	return evaluate_tree (node, table);
}

static diff_error_t read_new_number_in_var (table_t* table, size_t index)
{
	assert (table);

	printf ("%s = ", ((table -> table_var)[index]).name_var);

	if (scanf ("%lg", &(((table -> table_var)[index]).number_in_var)) == EOF)
	{
		char symbol = '\0';
		while ((symbol = (char) getchar ()) != '\n') {}

		printf ("Некорректный ввод значения. Пожалуйста повтори:\n");

		read_new_number_in_var (table, index);
		return NOT_ERROR;
	}

	if (getchar () != '\n')
	{
		char symbol = '\0';
		while ((symbol = (char) getchar ()) != '\n') {}

		printf ("Некорректный ввод значения. Пожалуйста повтори:\n");

		read_new_number_in_var (table, index);
		return NOT_ERROR;
	}

	return NOT_ERROR;
}

diff_error_t copy_tree (node_t* old_node, node_t* new_node)
{
	assert (old_node);
	assert (new_node);

	diff_error_t status = NOT_ERROR;

	new_node -> type  = old_node -> type;
	new_node -> value = old_node -> value;
	
	if (old_node -> left != NULL)
	{
		new_node -> left = create_new_node (NUM, 0, NULL, NULL, new_node, __FILE__, __LINE__);
		if (new_node -> left == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

		status = copy_tree (old_node -> left, new_node -> left);
		if (status) {return status;}
	}

	if (old_node -> right != NULL)
	{
		new_node -> right = create_new_node (NUM, 0, NULL, NULL, new_node, __FILE__, __LINE__);
		if (new_node -> right == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

		status = copy_tree (old_node -> right, new_node -> right);
		if (status) {return status;}
	}

	return status;
}