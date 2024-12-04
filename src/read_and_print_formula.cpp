#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "table_of_var.h"
#include "read_and_print_formula.h"

#define WRITE_WORD_IN_OP_(operation, name)     \
	if (strcmp (word, name) == 0)               \
	{                                                 \
		(tokens -> tokens_array)[tokens -> index_free_token].type           = OP;  \
		(tokens -> tokens_array)[tokens -> index_free_token].value.value_op = operation;               \
		return true;                                    \
	}

static diff_error_t lexical_analysis 	   (char* str_formula, tokens_array* tokens, table_t* table);
static diff_error_t create_tokens    	   (tokens_array* tokens);
static diff_error_t delete_tokens    	   (tokens_array* tokens);
static diff_error_t realloc_tokens         (tokens_array* tokens);
static diff_error_t dump_tokens            (tokens_array* tokens);
static bool         find_word_in_operation (char* word, tokens_array* tokens);

//-------------------------------------------------------------------------------------------------------------

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
				NAME_OPERATION_(LOG,  "log");
				NAME_OPERATION_(LN,   "ln");
				NAME_OPERATION_(DEG,  "^");

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

	tokens_array tokens = {};
	diff_error_t status = create_tokens (&tokens);
	if (status) {return status;}

	status = lexical_analysis (str_formula, &tokens, table);
	if (status) {return status;}

	dump_tokens (&tokens);
	getchar ();

	size_t index_str = 0;

	 status = create_tree (node, str_formula, &index_str, table);

	delete_tokens (&tokens);

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

			*ptr_index_str += strlen (name_operation);
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
				WRITE_NAME_OPERATION_(LOG,  "log");
				WRITE_NAME_OPERATION_(LN,   "ln");
				WRITE_NAME_OPERATION_(DEG,  "^");
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

			sscanf (str_formula + index_str, "%[^() ]%n", word, &shift_index_str);

			index_str += shift_index_str;

			if (shift_index_str == 0)    //str_formula[index_str] == '(' || str_formula[index_str] == ')'
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

	printf ("!\n");

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

static diff_error_t dump_tokens (tokens_array* tokens)
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

