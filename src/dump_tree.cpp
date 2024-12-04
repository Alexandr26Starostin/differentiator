#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "dump_tree.h"

#define NAME_OPERATION_(operation, name)    \
	case operation:                         \
	{                                       \
		name_operation = name;   			\
		break;                              \
	}

static diff_error_t print_edge (node_t* node, FILE* file_diff);

diff_error_t dump_tree (node_t* node, char* str_for_system, size_t* ptr_index_picture, FILE* diff_html)
{
	assert (node);
	assert (str_for_system);
	assert (ptr_index_picture);
	assert (diff_html);

	FILE* file_diff = fopen ("diff.dot", "w");
	if (file_diff == NULL) {printf ("Not find diff.dot\n"); return NOT_FIND_DIFF_DOT;}

	fprintf (file_diff, "digraph\n{\n\tnode[fontsize=9]\n\n\t");

	diff_error_t status = print_edge (node, file_diff);
	if (status) {fclose (file_diff); return status;}

	fprintf (file_diff, "}");

	fclose (file_diff);

	system (str_for_system);

	fprintf (diff_html, "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n"
						"<img src=\"pictures/diff_%c%c%c%c.svg\" style=\"width: 100%%\">\n", 
						str_for_system[INDEX_NUMBER_OF_PICTURE + 0], str_for_system[INDEX_NUMBER_OF_PICTURE + 1], str_for_system[INDEX_NUMBER_OF_PICTURE + 2], str_for_system[INDEX_NUMBER_OF_PICTURE + 3]);

	*ptr_index_picture += 1;
	size_t copy_index = *ptr_index_picture;

	str_for_system[INDEX_NUMBER_OF_PICTURE + 0] = '0' + (char) (copy_index / 1000);
	copy_index                                 %= 1000;

	str_for_system[INDEX_NUMBER_OF_PICTURE + 1] = '0' + (char) (copy_index / 100);
	copy_index                                 %= 100;

	str_for_system[INDEX_NUMBER_OF_PICTURE + 2] = '0' + (char) (copy_index / 10);
	copy_index                                 %= 10;

	str_for_system[INDEX_NUMBER_OF_PICTURE + 3] = '0' + (char) (copy_index / 1);
	copy_index                                 %= 1;

	return status;
}

static diff_error_t print_edge (node_t* node, FILE* file_diff)
{
	assert (node);
	assert (file_diff);

	switch (node -> type)
	{
		case NUM:
		{
			fprintf (file_diff, "node_%p [shape=record, label = \"{%lg| {type = %d (NUM) | left = %p | node = %p | parent = %p | right = %p}}\" style=\"filled\",fillcolor=\"blue\"]\n\n\t", node, (node -> value).value_num, node -> type, node -> left, node, node -> parent, node -> right);
			break;
		}

		case VAR:
		{
			fprintf (file_diff, "node_%p [shape=record, label = \"{%s| {type = %d (VAR) | left = %p | node = %p | parent = %p | right = %p}}\" style=\"filled\",fillcolor=\"yellow\"]\n\n\t", node, (node -> value).value_var, node -> type, node -> left, node, node -> parent, node -> right);
			break;
		}

		case OP:
		{
			const char* name_operation = NULL;

			switch ((node -> value).value_op)
			{
				NAME_OPERATION_(ADD,  "+");
				NAME_OPERATION_(SUB,  "-");
				NAME_OPERATION_(MUL,  "*");
				NAME_OPERATION_(DIV,  "/");
				NAME_OPERATION_(SIN,  "sin");
				NAME_OPERATION_(COS,  "cos");
				NAME_OPERATION_(SH,   "sh");
				NAME_OPERATION_(CH,   "ch");
				NAME_OPERATION_(SQRT, "sqrt");
				NAME_OPERATION_(LOG, "log");
				NAME_OPERATION_(DEG, "^");
				NAME_OPERATION_(LN, "ln");

				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					return NOT_FIND_OP;
				}
			}

			fprintf (file_diff, "node_%p [shape=record, label = \"{%s| {type = %d (OP) | left = %p | node = %p | parent = %p | right = %p}}\" style=\"filled\",fillcolor=\"#BDEF9E\"]\n\n\t", node, name_operation, node -> type, node -> left, node, node -> parent, node -> right);
			break;
		}

		default:
		{
			printf ("type = %d not find\n", node -> type);
			return NOT_FIND_TYPE_ARG;
		}
	}

	if (node -> parent != NULL)
	{
		fprintf (file_diff, "edge[color=\"black\",fontsize=12]\n\tnode_%p -> node_%p\n\n\t", node -> parent, node);
	}

	diff_error_t status = NOT_ERROR;

	if (node -> left != NULL)
	{
		status = print_edge (node -> left, file_diff);
		if (status) {return status;}
	}

	if (node -> right != NULL)
	{
		status = print_edge (node -> right, file_diff);
		if (status) {return status;}
	}

	return status;
}
