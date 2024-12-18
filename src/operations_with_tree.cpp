#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "const_in_diff.h"
#include "table_of_var.h"
#include "new_node.h"
#include "delete_tree.h"
#include "pdf_file.h"
#include "operations_with_tree.h"

#define change_node_(delete_branch, save_branch)                      \
	delete_tree (node -> delete_branch);                              \
                                                                      \
	node -> delete_branch = NULL;                                     \
                                                                      \
	node_t* old_##save_branch = node -> save_branch;                  \
                                                                      \
	node -> type  = old_##save_branch -> type;                        \
	node -> value = old_##save_branch -> value;                       \
                                                                      \
	node -> left  = copy_tree (old_##save_branch -> left,  node);     \
	node -> right = copy_tree (old_##save_branch -> right, node);     \
                                                                      \
	delete_tree (old_##save_branch);                                  \
                                                                      \
	count_of_simplification += 1;                                     \
                                                                      \
	return count_of_simplification;

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

#define LOG_(left, right) create_new_node (OP, LOG, left, right, NULL, __FILE__, __LINE__)

#define DEG_(left, right) create_new_node (OP, DEG, left, right, NULL, __FILE__, __LINE__)

#define LN_(right) create_new_node (OP, LN, NULL, right, NULL, __FILE__, __LINE__)

#define dL_PDF_ create_derivate_with_write_in_pdf (pdf_file, node -> left, math_phrases, ptr_index_math_phrases) 

#define dR_PDF_ create_derivate_with_write_in_pdf (pdf_file, node -> right, math_phrases, ptr_index_math_phrases) 

static diff_error_t read_new_number_in_var    (table_t* table, size_t index);
static bool         compare_doubles           (double num_1, double num_2);
static size_t       simplify_numbers          (node_t* node);
static size_t       simplify_neutral_elements (node_t* node);
static size_t       max_value                 (size_t value_1, size_t value_2);

//-------------------------------------------------------------------------------------------------------------------------

double evaluate_tree (node_t* node, table_t* table)
{
	assert (node);

	switch (node -> type)
	{
		case NUM: return (node -> value).value_num;

		case VAR: 
		{
			if (table != NULL)
				return *find_var_in_table (table, (node -> value).value_var);
			else
				return 0;
		}

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
				case LN:   return log  (evaluate_tree (node -> right, table));

				case DEG:  return pow  (evaluate_tree (node -> left, table), evaluate_tree (node -> right, table));
				case LOG:  return log  (evaluate_tree (node -> right, table)) / log (evaluate_tree (node -> left, table));
				
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
				
				case SQRT: return DIV_(dR_, MUL_(NUM_(2), SQRT_(cR_)));

				case LN: return DIV_(dR_, cR_);

				case LOG:
				{
					size_t quantity_vars_left  = 0;

					count_vars (node -> left, &quantity_vars_left);

					if (quantity_vars_left == 0) 
					{
						return DIV_(dR_, MUL_(LN_(cL_), cR_));
					}

					else
					{
						return DIV_(SUB_(DIV_(MUL_(dR_, LN_(cL_)),cR_), DIV_(MUL_(dL_, LN_(cR_)), cL_)) , DEG_(LN_(cL_), NUM_(2)));
					}
				}

				case DEG: 
				{
					size_t quantity_vars_left  = 0;
					size_t quantity_vars_right = 0;

					count_vars (node -> left, &quantity_vars_left);
					count_vars (node -> right, &quantity_vars_right);

					// printf ("quantity_vars_left  = %ld\n", quantity_vars_left);
					// printf ("quantity_vars_right = %ld\n", quantity_vars_right);

					if (quantity_vars_left == 0 && quantity_vars_right == 0)
					{
						return NUM_(0);
					}

					if (quantity_vars_left == 0)
					{
						return MUL_(DEG_(cL_, cR_), LN_(cL_));
					}

					else if (quantity_vars_right == 0)
					{
						return MUL_(MUL_(cR_, dL_), DEG_(cL_, SUB_(cR_, NUM_(1))));
					}

					else
					{
						return MUL_(DEG_(cL_, cR_), ADD_(MUL_(dR_, LN_(cL_)), MUL_(cR_, DIV_(dL_, cL_))));
					}
				}
			
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

//----------------------------------------------------------------------------------------------------------------------------

diff_error_t simplify_tree (node_t* node)
{
	assert (node);

	size_t count_of_simplification = 0;

	while (true)      
	{
		count_of_simplification = 0;

		count_of_simplification += simplify_numbers          (node);
		count_of_simplification += simplify_neutral_elements (node);

		if (count_of_simplification == 0)
		{
			count_of_simplification += simplify_numbers          (node);
			count_of_simplification += simplify_neutral_elements (node);

			if (count_of_simplification == 0) 
			{
				return NOT_ERROR;
			}
		}
	}
}

static size_t simplify_numbers (node_t* node)
{
	assert (node);

	size_t count_of_simplification = 0;
	size_t quantity_vars           = 0;

	count_vars (node, &quantity_vars);

	if (quantity_vars == 0)
	{
		double old_value = (node -> value).value_num;

		(node -> value).value_num = evaluate_tree (node, NULL);

		if (!compare_doubles ((node -> value).value_num, old_value))
		{
			count_of_simplification += 1;
		}

		node -> type = NUM;

		delete_tree (node -> left);
		delete_tree (node -> right);

		node -> left  = NULL;
		node -> right = NULL;

		return count_of_simplification;
	}

	if (node -> left != NULL) {count_of_simplification += simplify_numbers (node -> left);}

	if (node -> right != NULL) {count_of_simplification += simplify_numbers (node -> right);}

	return count_of_simplification;
}

static size_t simplify_neutral_elements (node_t* node)
{
	assert (node);

	size_t count_of_simplification = 0;

	if (node -> left != NULL) {count_of_simplification += simplify_neutral_elements (node -> left);}

	if (node -> right != NULL) {count_of_simplification += simplify_neutral_elements (node -> right);}

	if (node -> type == OP)
	{
		if ((node -> value).value_op == ADD)
		{
			if (((node -> left) -> type == NUM) && compare_doubles (((node -> left) -> value).value_num, 0)) {change_node_(left, right)}

			else if (((node -> right) -> type == NUM) && compare_doubles (((node -> right) -> value).value_num, 0)) {change_node_(right, left)}
		}

		else if ((node -> value).value_op == SUB)
		{
			if (((node -> left) -> type == NUM) && compare_doubles (((node -> left) -> value).value_num, 0))
			{
				(node -> value).value_op = MUL;

				((node -> left) -> value).value_num = -1;

				count_of_simplification += 1;

				return count_of_simplification;
			}

			else if (((node -> right) -> type == NUM) && compare_doubles (((node -> right) -> value).value_num, 0)) {change_node_(right, left)}
		}

		else if ((node -> value).value_op == MUL)
		{
			if (((node -> left) -> type == NUM) && compare_doubles (((node -> left) -> value).value_num, 1)) {change_node_(left, right)}

			else if (((node -> right) -> type == NUM) && compare_doubles (((node -> right) -> value).value_num, 1)) {change_node_(right, left)}

			else if ((((node -> left) -> type == NUM) && compare_doubles (((node -> left) -> value).value_num, 0)) || (((node -> right) -> type == NUM) && compare_doubles (((node -> right) -> value).value_num, 0)))
			{                                       
				delete_tree (node -> left);
				delete_tree (node -> right);

				node -> left  = NULL;
				node -> right = NULL;

				node -> type = NUM;

				(node -> value).value_num = 0;

				count_of_simplification += 1;

				return count_of_simplification;
			}
		}

		else if ((node -> value).value_op == DIV)
		{  
			if (((node -> right) -> type == NUM) && compare_doubles (((node -> right) -> value).value_num, 1)) {change_node_(right, left)}

			if (((node -> left)  -> type == NUM) && compare_doubles (((node -> left) -> value).value_num, 0))
			{
				delete_tree (node -> left);
				delete_tree (node -> right);

				node -> left  = NULL;
				node -> right = NULL;

				node -> type = NUM;

				(node -> value).value_num = 0;

				count_of_simplification += 1;

				return count_of_simplification;
			}
		}
	}

	return count_of_simplification;
}

static bool compare_doubles (double num_1, double num_2)   //true if num_1 == num_2      false if num_1 != num_2
{
	if (abs (num_1 - num_2) < 0.0001) {return true;}

	else {return false;}
}

size_t count_depth_of_tree (node_t* node)
{
	if (node == NULL) {return 0;}

	size_t depth_of_tree = max_value (count_depth_of_tree (node -> left), count_depth_of_tree (node -> right));

	return depth_of_tree + 1;
}

static size_t max_value (size_t value_1, size_t value_2)
{
	if (value_1 > value_2) {return value_1;}

	return value_2;
}

node_t* create_derivate_with_write_in_pdf (FILE* pdf_file, node_t* node, const char** math_phrases, size_t* ptr_index_math_phrases)
{
	assert (pdf_file);
	assert (math_phrases);
	assert (ptr_index_math_phrases);

	if (node == NULL) {return NULL;}

	node_t* diff_node = NULL;

	size_t depth_of_tree = count_depth_of_tree (node);
	
	switch (node -> type)
	{
		case VAR: diff_node = NUM_(1); break;
		case NUM: diff_node = NUM_(0); break;
		
		case OP:
		{
			switch ((node ->value).value_op)
			{
				case ADD:  diff_node = ADD_(dL_PDF_, dR_PDF_);                                             break;
				case SUB:  diff_node = SUB_(dL_PDF_, dR_PDF_);                                             break;
				case MUL:  diff_node = ADD_(MUL_(dL_PDF_, cR_), MUL_(cL_, dR_PDF_));                       break;
				case DIV:  diff_node = DIV_(SUB_(MUL_(dL_PDF_, cR_), MUL_(cL_, dR_PDF_)), MUL_(cR_, cR_)); break;
				case SIN:  diff_node = MUL_(COS_(cR_), dR_PDF_);			  						   	   break;
				case COS:  diff_node = MUL_(NUM_(-1), MUL_(SIN_(cR_), dR_PDF_));                       	   break;				
				case SH:   diff_node = MUL_(CH_(cR_), dR_PDF_);			                                   break;
				case CH:   diff_node = MUL_(SH_(cR_), dR_PDF_);                                            break;
				case SQRT: diff_node = DIV_(dR_PDF_, MUL_(NUM_(2), SQRT_(cR_)));                           break;
				case LN:   diff_node = DIV_(dR_PDF_, cR_); 											       break; 

				case LOG:
				{
					size_t quantity_vars_left  = 0;

					count_vars (node -> left, &quantity_vars_left);

					if (quantity_vars_left == 0) diff_node = DIV_(dR_PDF_, MUL_(LN_(cL_), cR_));

					else diff_node = DIV_(SUB_(DIV_(MUL_(dR_PDF_, LN_(cL_)),cR_), DIV_(MUL_(dL_PDF_, LN_(cR_)), cL_)) , DEG_(LN_(cL_), NUM_(2)));

					break;
				}

				case DEG: 
				{
					size_t quantity_vars_left  = 0;
					size_t quantity_vars_right = 0;

					count_vars (node -> left, &quantity_vars_left);
					count_vars (node -> right, &quantity_vars_right);

					// printf ("quantity_vars_left  = %ld\n", quantity_vars_left);
					// printf ("quantity_vars_right = %ld\n", quantity_vars_right);

					if (quantity_vars_left == 0 && quantity_vars_right == 0) diff_node = NUM_(0);

					else if (quantity_vars_left == 0) diff_node = MUL_(DEG_(cL_, cR_), LN_(cL_));
					
					else if (quantity_vars_right == 0) diff_node = MUL_(MUL_(cR_, dL_PDF_), DEG_(cL_, SUB_(cR_, NUM_(1))));
					
					else diff_node = MUL_(DEG_(cL_, cR_), ADD_(MUL_(dR_PDF_, LN_(cL_)), MUL_(cR_, DIV_(dL_PDF_, cL_))));
					
					break;
				}
			
				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					return NULL;
				}
			}

			if (depth_of_tree <= MAX_DEPTH_OF_TREE)
			{
				fprintf (pdf_file, "%s", math_phrases[(*ptr_index_math_phrases) % (SIZE_MATH_PHRASES - 1)]);

				(*ptr_index_math_phrases) += 1;

				fprintf (pdf_file, "\\[(");
				write_node_in_pdf  (pdf_file, node);

				fprintf (pdf_file, ")' = ");

				simplify_tree (diff_node);
			
				write_tree_in_pdf  (pdf_file, diff_node);
			}

			break;
		}

		default:
		{
			printf ("type = %d not find\n", node -> type);
			return NULL;
		}
	}	

	return diff_node;
}

