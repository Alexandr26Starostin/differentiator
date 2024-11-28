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
		case NUM: return (node -> value).value_num;

		case VAR: return *find_var_in_table (table, (node -> value).value_var);
	
		case OP:
		{
			switch ((node -> value).value_op)
			{
				case ADD: return evaluate_tree (node -> left, table) + evaluate_tree (node -> right, table);
				case SUB: return evaluate_tree (node -> left, table) - evaluate_tree (node -> right, table);
				case MUL: return evaluate_tree (node -> left, table) * evaluate_tree (node -> right, table);
				case DIV: return evaluate_tree (node -> left, table) / evaluate_tree (node -> right, table);
					
				case SIN:  return sin  (evaluate_tree (node -> right, table));
				case COS:  return cos  (evaluate_tree (node -> right, table));
				case SH:   return sinh (evaluate_tree (node -> right, table));
				case CH:   return cosh (evaluate_tree (node -> right, table));
				case SQRT: return sqrt (evaluate_tree (node -> right, table));
				
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

//-------------------------------------------------------------------------------------------------------------------------

node_t* copy_tree (node_t* old_node, node_t* parent_new_node)
{
	if (old_node == NULL) {return NULL;}

	node_t* new_node = create_new_node (NUM, 0, NULL, NULL, parent_new_node, __FILE__, __LINE__);
	if (new_node == NULL) {return NULL;}

	new_node -> type  = old_node -> type;
	new_node -> value = old_node -> value;
	
	if (old_node -> left != NULL)
	{	
		new_node -> left = copy_tree (old_node -> left, new_node);
		if (new_node -> left == NULL) {return NULL;}
	}

	if (old_node -> right != NULL)
	{
		new_node -> right = copy_tree (old_node -> right, new_node);
		if (new_node -> right == NULL) {return NULL;}
	}

	return new_node;
}

//---------------------------------------------------------------------------------------------------------------

#define dL_ create_derivate (node -> left) 

#define dR_ create_derivate (node -> right)  

#define cL_ copy_tree (node -> left, NULL)

#define cR_ copy_tree (node -> right, NULL)

#define NUM_(value) create_new_node (NUM, value, NULL, NULL, NULL, __FILE__, __LINE__)

#define ADD_(left, right) create_new_node (OP, ADD, left, right, NULL, __FILE__, __LINE__)

#define SUB_(left, right) create_new_node (OP, SUB, left, right, NULL, __FILE__, __LINE__)

#define MUL_(left, right) create_new_node (OP, MUL, left, right, NULL, __FILE__, __LINE__)

#define DIV_(left, right) create_new_node (OP, DIV, left, right, NULL, __FILE__, __LINE__)

#define SIN_(right) create_new_node (OP, SIN, NULL, right, NULL, __FILE__, __LINE__)

#define COS_(right) create_new_node (OP, COS, NULL, right, NULL, __FILE__, __LINE__)

#define SH_(right) create_new_node (OP, SH, NULL, right, NULL, __FILE__, __LINE__)

#define CH_(right) create_new_node (OP, CH, NULL, right, NULL, __FILE__, __LINE__)

#define SQRT_(right) create_new_node (OP, SQRT, NULL, right, NULL, __FILE__, __LINE__)


node_t* create_derivate (node_t* node)
{
	if (node == NULL) {return NULL;}

	switch (node -> type)
	{
		case VAR: return NUM_(1);
		
		case NUM: return NUM_(0);
		
		case OP:
		{
			switch ((node ->value).value_op)
			{
				case ADD: return ADD_(dL_, dR_);
				case SUB: return SUB_(dL_, dR_);

				case MUL: return ADD_(MUL_(dL_, cR_), MUL_(cL_, dR_));

				case DIV: return DIV_(SUB_(MUL_(dL_, cR_), MUL_(cL_, dR_)), MUL_(cR_, cR_));

				case SIN: return MUL_(COS_(cR_), dR_);			

				case COS: return MUL_(NUM_(-1), MUL_(SIN_(cR_), dR_));				

				case SH: return MUL_(CH_(cR_), dR_);			
				case CH: return MUL_(SH_(cR_), dR_);
				
				case SQRT: return NUM_(1);   //Неверно!!!!
			
				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					return NULL;
				}
			}
		}

		default:
		{
			printf ("type = %d not find\n", node -> type);
			return NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------

diff_error_t count_vars (node_t* node, size_t* ptr_quantity_vars)
{
	assert (node);
	assert (ptr_quantity_vars);

	if (node -> type == VAR) 
		*ptr_quantity_vars += 1;

	if (node -> left != NULL)
		count_vars (node -> left, ptr_quantity_vars);

	if (node -> right != NULL)
		count_vars (node -> right, ptr_quantity_vars);

	return NOT_ERROR;
}

//здесь был саня 

