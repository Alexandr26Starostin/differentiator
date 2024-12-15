#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "table_of_var.h"
#include "read_and_print_formula.h"

#define WRITE_WORD_IN_OP_(operation, name)                                                           \
	if (strcmp (word, name) == 0)                 													 \
	{                                                 												 \
		(tokens -> tokens_array)[tokens -> index_free_token].type           = OP;  					 \
		(tokens -> tokens_array)[tokens -> index_free_token].value.value_op = operation;             \
		return true;                                   												 \
	}

static diff_error_t lexical_analysis 	   (char* str_formula, tokens_array* tokens, table_t* table);
static diff_error_t create_tokens    	   (tokens_array* tokens);
static diff_error_t delete_tokens    	   (tokens_array* tokens);
static diff_error_t realloc_tokens         (tokens_array* tokens);
       diff_error_t dump_tokens            (tokens_array* tokens);
static bool         find_word_in_operation (char* word, tokens_array* tokens);

#define READ_FUNC_IN_GET_FUNC_(operation)                                      							\
	if ((tokens -> tokens_array)[*ptr_index].value.value_op == operation)       						\
	{                                                                           						\
		(*ptr_index)++;                                                        			 				\
																										\
 		diff_error_t status = get_p (tokens, ptr_index, &new_node);               						\
		if (status) {return status;}                                            						\
                                                                                                        \
		*ptr_node = create_new_node (OP, operation, NULL, new_node, NULL, __FILE__, __LINE__);         	\
		return status;                                                          						\
	}

static diff_error_t get_g    (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_e    (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_t    (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_deg  (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_func (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_p    (tokens_array* tokens, size_t* ptr_index, node_t** node);
static diff_error_t get_n    (tokens_array* tokens, size_t* ptr_index, node_t** node);

//-------------------------------------------------------------------------------------------------------------

#define NAME_OPERATION_(operation, name) 		\
	case operation:                             \
	{                                      		\
		name_operation = name;   				\
		break;                               	\
	}

#define WRITE_NAME_OPERATION_(operation, name)     		\
	if (strcmp (name_operation, name) == 0)             \
	{                                                   \
		(node -> value).value_op = operation;           \
		continue;                                    	\
	}

// static diff_error_t create_tree (node_t* node, char* str_formula, size_t* ptr_index_str, table_t* table);

//-----------------------------------------------------------------------------------------------------

diff_error_t print_formula (FILE* pdf_file, node_t* node)
{
	assert (node);
	assert (pdf_file);

	diff_error_t status = NOT_ERROR;

	if (node -> type == OP)
	{
		fprintf (pdf_file, "(");
	}

	//-------------------------------------------------------------

	if (node -> left != NULL)
	{
		status = print_formula (pdf_file, node -> left);

		if (status) {return status;}
	}

	//-------------------------------------------------------------

	switch (node -> type)
	{
		case NUM:
		{
			fprintf (pdf_file, "(%lg)", (node -> value).value_num);
			break;
		}

		case VAR:
		{
			fprintf (pdf_file, "(%s)", (node -> value).value_var);
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
				NAME_OPERATION_(SIN,  "sin(deg(");
				NAME_OPERATION_(COS,  "cos(deg(");
				NAME_OPERATION_(SH,   "sh");
				NAME_OPERATION_(CH,   "ch");
				NAME_OPERATION_(SQRT, "sqrt");
				NAME_OPERATION_(LOG,  "log");
				NAME_OPERATION_(LN,   "ln");
				NAME_OPERATION_(DEG,  "^");

				default:
				{
					printf ("Not find operation = %d", (node -> value).value_op);
					return NOT_FIND_OP;
				}
			}

			fprintf (pdf_file, "%s", name_operation);

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
		status = print_formula (pdf_file, node -> right);

		if (status) {return status;}
	}

	//-------------------------------------------------------------

	if (node -> type == OP)
	{
		if ((node -> value).value_op == SIN || (node -> value).value_op == COS)
		{
			fprintf (pdf_file, "))");
		}

		fprintf (pdf_file, ")");
	}

	return NOT_ERROR;
}

//----------------------------------------------------------------------------------------------------------

diff_error_t read_formula (node_t** ptr_node, table_t* table)
{
	assert (table);

	char str_formula[MAX_LEN_STR_FORMULA] = "";

	printf ("Введите формулу:\n");

	scanf ("%[^'\n']", str_formula);
	getchar ();
	//printf ("%s\n", str_formula);

	tokens_array tokens = {};
	diff_error_t status = create_tokens (&tokens);
	if (status) {return status;}

	status = lexical_analysis (str_formula, &tokens, table);
	if (status) {return status;}

	//dump_tokens (&tokens);

	size_t index = 0;

	status = get_g (&tokens, &index, ptr_node);
	if (status) {return status;};

	//size_t index_str = 0;

	//status = create_tree (node, str_formula, &index_str, table);

	delete_tokens (&tokens);

	return status;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------

static diff_error_t lexical_analysis (char* str_formula, tokens_array* tokens, table_t* table)
{
	assert (str_formula);
	assert (tokens);
	assert (table);

	diff_error_t status = create_tokens (tokens);
	if (status) {return status;}

	size_t index_str = 0;

	while (str_formula[index_str] != '\0')
	{
		while (str_formula[index_str] == ' ') {index_str++;}

		if ('0' <= str_formula[index_str] && str_formula[index_str] <= '9')
		{
			char* new_position = NULL;

			(tokens -> tokens_array)[tokens -> index_free_token].type            = NUM;
			(tokens -> tokens_array)[tokens -> index_free_token].value.value_num = strtod (str_formula + index_str, &new_position);

			index_str = new_position - str_formula;

			tokens -> index_free_token += 1;

			if (tokens -> index_free_token == tokens -> size_tokens)
			{
				status = realloc_tokens (tokens);
				if (status) {return status;}
			}
		}

		else
		{
			char word[MAX_LEN_STR_FORMULA] = "";

			int shift_index_str = 0;

			sscanf (str_formula + index_str, "%[^()+-^/* ]%n", word, &shift_index_str);

			index_str += shift_index_str;

			if (shift_index_str == 0)    //str_formula[index_str] in '(^()+-^/*'
			{
				word[0] = str_formula[index_str];

				index_str += 1;
			}

			if (find_word_in_operation (word, tokens))
			{
				tokens -> index_free_token += 1; 

				if (tokens -> index_free_token == tokens -> size_tokens)   //verbose -v
				{
					status = realloc_tokens (tokens);
					if (status) {return status;}
				}
			}

			else
			{
				(tokens -> tokens_array)[tokens -> index_free_token].type = VAR;  

				if (word[0] != '(' && word[0] != ')')
				{
					status = add_var_in_table (table, word, 0);
					if (status) {return status;}
				}

				strcpy ((tokens -> tokens_array)[tokens -> index_free_token].value.value_var, word);

				tokens -> index_free_token += 1;

				if (tokens -> index_free_token == tokens -> size_tokens)
				{
					status = realloc_tokens (tokens);
					if (status) {return status;}
				}
			}
		}
	}

	return status;
}

static diff_error_t create_tokens (tokens_array* tokens)
{
	assert (tokens);

	tokens -> tokens_array = (token_t*) calloc (MIN_SIZE_TOKENS_ARRAY, sizeof (token_t));

	if (tokens -> tokens_array == NULL)
	{
		printf ("Error in %s:%d\n", __FILE__, __LINE__);
		printf ("Not memory for tokens_array\n");
		return NOT_MEMORY_FOR_TOKENS_ARRAY;
	}

	tokens -> size_tokens      = MIN_SIZE_TOKENS_ARRAY;
	tokens -> index_free_token = 0;

	return NOT_ERROR;
}

static diff_error_t delete_tokens (tokens_array* tokens)
{
	assert (tokens);

	free (tokens -> tokens_array);

	tokens -> size_tokens      = 0;
	tokens -> index_free_token = 0;

	return NOT_ERROR;
}

static diff_error_t realloc_tokens (tokens_array* tokens)
{
	assert (tokens);

	tokens -> size_tokens *= 2;

	tokens -> tokens_array = (token_t*) realloc (tokens -> tokens_array, sizeof (token_t) * tokens -> size_tokens);

	if (tokens -> tokens_array == NULL)
	{
		printf ("Error in %s:%d\n", __FILE__, __LINE__);
		printf ("Not memory for tokens_array\n");
		return NOT_MEMORY_FOR_TOKENS_ARRAY;
	}

	return NOT_ERROR;
}

static bool find_word_in_operation (char* word, tokens_array* tokens)
{
	assert (word);
	assert (tokens);

	WRITE_WORD_IN_OP_(ADD, "+");
	WRITE_WORD_IN_OP_(SUB, "-");
	WRITE_WORD_IN_OP_(MUL, "*");
	WRITE_WORD_IN_OP_(DIV, "/");

	WRITE_WORD_IN_OP_(SIN, "sin");
	WRITE_WORD_IN_OP_(COS, "cos");
	WRITE_WORD_IN_OP_(SH,  "sh");
	WRITE_WORD_IN_OP_(CH,  "ch");

	WRITE_WORD_IN_OP_(SQRT, "sqrt");
	WRITE_WORD_IN_OP_(LOG,  "log");
	WRITE_WORD_IN_OP_(DEG,  "^");
	WRITE_WORD_IN_OP_(LN,   "ln");
	
	return false;
}

diff_error_t dump_tokens (tokens_array* tokens)
{
	assert (tokens);

	printf ("--------------------------------------------------------------------------------------------------------------\n");
	printf ("Dump tokens:\n\n");

	printf ("tokens -> size_tokens      == %ld\n",   tokens -> size_tokens);
	printf ("tokens -> index_free_token == %ld\n\n", tokens -> index_free_token);
	printf ("tokens -> tokens_array:\n\n");
	printf ("index     type    value\n");

	for (size_t index = 0; index < tokens -> index_free_token; index++)
	{
		printf ("%7ld %7d ", index, (tokens -> tokens_array)[index].type);

		if ((tokens -> tokens_array)[index].type == NUM)
		{
			printf ("%7lg \n", (tokens -> tokens_array)[index].value.value_num);
		}

		else if ((tokens -> tokens_array)[index].type == OP)
		{
			printf ("%7d \n", (tokens -> tokens_array)[index].value.value_op);	
		}

		else //(tokens -> tokens_array)[index].type == VAR
		{
			printf ("%7s \n", (tokens -> tokens_array)[index].value.value_var);
		}
	}

	printf ("--------------------------------------------------------------------------------------------------------------\n");

	return NOT_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static diff_error_t get_g (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	diff_error_t status = get_e (tokens, ptr_index, ptr_node);
	if (status) {return status;}

	if (*ptr_index != tokens -> index_free_token)
	{
		printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
		printf ("Error from 'get_g' wait position: %ld, "
		        "but get position: '%ld'\n\n", 
				tokens -> index_free_token, *ptr_index);

		return ERROR_IN_GET_G;
	}

	(*ptr_index)++;

	return status;
}

static diff_error_t get_n (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	if ((tokens -> tokens_array)[*ptr_index].type == NUM)
	{
		*ptr_node = create_new_node (NUM, (tokens -> tokens_array)[*ptr_index].value.value_num, NULL, NULL, NULL, __FILE__, __LINE__);
		(*ptr_index)++;

		return NOT_ERROR;
	}

	else if ((tokens -> tokens_array)[*ptr_index].type == VAR)
	{
		*ptr_node = create_new_node (VAR, 0, NULL, NULL, NULL, __FILE__, __LINE__);

		if (*ptr_node == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

		strcpy (((*ptr_node) -> value).value_var, (tokens -> tokens_array)[*ptr_index].value.value_var);

		(*ptr_index)++;

		return NOT_ERROR;
	}

	else
	{
		printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
		printf ("Error from 'get_n' on position: %ld\n\n"
		        "'get_n' wait: num (type == 1) or var (type == 2), but find OP (type == %d)\n\n", 
				*ptr_index, (tokens -> tokens_array)[*ptr_index].type);

		return ERROR_IN_GET_N;
	}
}

static diff_error_t get_e (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	size_t old_index = 0;

	node_t* new_node = NULL;

	diff_error_t status = get_t (tokens, ptr_index, ptr_node);
	if (status) {return status;}

	while ((tokens -> tokens_array)[*ptr_index].type == OP && 
	      ((tokens -> tokens_array)[*ptr_index].value.value_op == ADD || 
		   (tokens -> tokens_array)[*ptr_index].value.value_op == SUB))
	{
		old_index = *ptr_index;

		(*ptr_index)++;

		status = get_t (tokens, ptr_index, &new_node);
		if (status) {return status;}

		if ((tokens -> tokens_array)[old_index].value.value_op == ADD)
		{
			*ptr_node = create_new_node (OP, ADD, *ptr_node, new_node, NULL, __FILE__, __LINE__);
		}

		else  //(tokens -> tokens_array)[old_index].value.value_op == SUB)
		{
			*ptr_node = create_new_node (OP, SUB, *ptr_node, new_node, NULL, __FILE__, __LINE__);
		}
	}

	return status;
}

static diff_error_t get_t (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	node_t* new_node = NULL;
	size_t old_index = 0;

	diff_error_t status = get_deg (tokens, ptr_index, ptr_node);
	if (status) {return status;}

	while ((tokens -> tokens_array)[*ptr_index].type == OP &&
	      ((tokens -> tokens_array)[*ptr_index].value.value_op == MUL || 
		   (tokens -> tokens_array)[*ptr_index].value.value_op == DIV))
	{
		old_index = *ptr_index;

		(*ptr_index)++;

		status = get_deg (tokens, ptr_index, &new_node);
		if (status) {return status;}

		if ((tokens -> tokens_array)[old_index].value.value_op == MUL)
		{
			*ptr_node = create_new_node (OP, MUL, *ptr_node, new_node, NULL, __FILE__, __LINE__);
		}

		else  //(tokens -> tokens_array)[old_index].value.value_op == DIV)
		{
			*ptr_node = create_new_node (OP, DIV, *ptr_node, new_node, NULL, __FILE__, __LINE__);
		}
	}

	return status;
}

static diff_error_t get_p (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	diff_error_t status = NOT_ERROR;

	//printf ("<%s>\n", (tokens -> tokens_array)[*ptr_index].value.value_var);

	if ((tokens -> tokens_array)[*ptr_index].type == VAR && 
	    (tokens -> tokens_array)[*ptr_index].value.value_var[0] == '(')
	{
		(*ptr_index)++;

		status = get_e (tokens, ptr_index, ptr_node);
		if (status) {return status;}

		if ((tokens -> tokens_array)[*ptr_index].type == VAR && 
		    (tokens -> tokens_array)[*ptr_index].value.value_var[0] != ')')
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_p' in str on position: %ld\n\n"
			        "'get_p' wait: ')', but find '%c'\n\n", 
					*ptr_index, (tokens -> tokens_array)[*ptr_index].value.value_var[0]);

			return ERROR_IN_GET_P;
		}

		(*ptr_index)++;

		return status;
	}

	else
	{
		status = get_n (tokens, ptr_index, ptr_node);

		return status;
	}
}

static diff_error_t get_deg (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	node_t* new_node = NULL;

	diff_error_t status = get_func (tokens, ptr_index, ptr_node);
	if (status) {return status;}

	while ((tokens -> tokens_array)[*ptr_index].type == OP &&
	      (tokens -> tokens_array)[*ptr_index].value.value_op == DEG)
	{
		(*ptr_index)++;

		status = get_func (tokens, ptr_index, &new_node);
		if (status) {return status;}
		
		*ptr_node = create_new_node (OP, DEG, *ptr_node, new_node, NULL, __FILE__, __LINE__);
	}

	return status;
}

static diff_error_t get_func (tokens_array* tokens, size_t* ptr_index, node_t** ptr_node)
{
	assert (tokens);
	assert (ptr_index);
	assert (ptr_node);

	node_t* new_node = NULL;

	if ((tokens -> tokens_array)[*ptr_index].type == OP)
	{
		READ_FUNC_IN_GET_FUNC_(SIN);
		READ_FUNC_IN_GET_FUNC_(COS);
		READ_FUNC_IN_GET_FUNC_(SH);
		READ_FUNC_IN_GET_FUNC_(CH);
		READ_FUNC_IN_GET_FUNC_(SQRT);
		READ_FUNC_IN_GET_FUNC_(LN);

		if ((tokens -> tokens_array)[*ptr_index].value.value_op == LOG)
		{
			(*ptr_index)++;

			diff_error_t status = get_p (tokens, ptr_index, ptr_node);
			if (status) {return status;}

			status = get_p (tokens, ptr_index, &new_node);
			if (status) {return status;}

			*ptr_node = create_new_node (OP, LOG, *ptr_node, new_node, NULL, __FILE__, __LINE__);

			return status;
		}
	}

	else
	{
		diff_error_t status = get_p (tokens, ptr_index, ptr_node);
		if (status) {return status;}
	}

	return NOT_ERROR;
}


// static diff_error_t create_tree (node_t* node, char* str_formula, size_t* ptr_index_str, table_t* table)
// {
// 	assert (node);
// 	assert (str_formula);
// 	assert (ptr_index_str);
// 	assert (table);

// 	diff_error_t status = NOT_ERROR;

// 	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 	if (str_formula[*ptr_index_str] == '(')
// 	{
		
// 		*ptr_index_str += 1;

// 		node -> left = create_new_node (NUM, 0, NULL, NULL, node, __FILE__, __LINE__);
// 		if (node -> left == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

// 		status = create_tree ((node -> left), str_formula, ptr_index_str, table);
// 		if (status) {return status;}

// 		while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 		if (str_formula[*ptr_index_str] == ')') {*ptr_index_str += 1;}
// 	}

// 	//------------------------------------------------------------------------------------------------------  

// 	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 	char name_operation[MAX_LEN_STR_FORMULA] = "";
// 	sscanf (str_formula + *ptr_index_str, "%[^0123456789]", name_operation);

// 	if (strlen (name_operation) == 0)
// 	{
// 		sscanf (str_formula + *ptr_index_str, "%[0123456789]", name_operation);

// 		//printf ("%s\n", name_operation);

// 		node -> type = NUM;
// 		(node -> value).value_num = strtod (name_operation, NULL);

// 		*ptr_index_str += strlen (name_operation);
// 	}
// 	else
// 	{
// 		sscanf (str_formula + *ptr_index_str, "%[^)]", name_operation);

// 		//printf ("%s\n", name_operation);

// 		if (strchr (name_operation, '(') == NULL)   //VAR
// 		{
// 			//printf ("\n\n%s\n\n", name_operation);

// 			node -> type = VAR;

// 			status = add_var_in_table (table, name_operation, 0);
// 			if (status) {return status;}

// 			strcpy ((node -> value).value_var, name_operation);

// 			*ptr_index_str += strlen (name_operation);
// 		}
// 		else         //OP
// 		{
// 			sscanf (str_formula + *ptr_index_str, "%[^ (]", name_operation);

// 			//printf ("!-%s\n", name_operation);

// 			node -> type = OP;

// 			for (size_t quantity_operations = 1; quantity_operations == 1; quantity_operations++)
// 			{
// 				WRITE_NAME_OPERATION_(ADD,  "+");
// 				WRITE_NAME_OPERATION_(SUB,  "-");
// 				WRITE_NAME_OPERATION_(MUL,  "*");
// 				WRITE_NAME_OPERATION_(DIV,  "/");
// 				WRITE_NAME_OPERATION_(SIN,  "sin");
// 				WRITE_NAME_OPERATION_(COS,  "cos");
// 				WRITE_NAME_OPERATION_(SH,   "sh");
// 				WRITE_NAME_OPERATION_(CH,   "ch");
// 				WRITE_NAME_OPERATION_(SQRT, "sqrt");
// 				WRITE_NAME_OPERATION_(LOG,  "log");
// 				WRITE_NAME_OPERATION_(LN,   "ln");
// 				WRITE_NAME_OPERATION_(DEG,  "^");
// 			}

// 			*ptr_index_str += strlen (name_operation);
// 		}
// 	}

// 	//--------------------------------------------------------------------------------------------------------

// 	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 	if (str_formula[*ptr_index_str] == '(')
// 	{
// 		*ptr_index_str += 1;

// 		node -> right = create_new_node (NUM, 0, NULL, NULL, node, __FILE__, __LINE__);
// 		if (node -> right == NULL) {return NOT_MEMORY_FOR_NEW_NODE;}

// 		status = create_tree ((node -> right), str_formula, ptr_index_str, table);
// 		if (status) {return status;}

// 		while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 		if (str_formula[*ptr_index_str] == ')') {*ptr_index_str += 1;}
// 	}

// 	while (str_formula[*ptr_index_str] == ' ') {(*ptr_index_str)++;}

// 	return status;
// }