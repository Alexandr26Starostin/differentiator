#ifndef CONST_IN_DIFF_H
#define CONST_IN_DIFF_H

//--------------------------------------------------------------

const size_t INDEX_NUMBER_OF_PICTURE = 36;
const size_t MAX_LEN_STR_FORMULA     = 70;
const size_t MIN_SIZE_TABLE_VAR      = 15;
const size_t MAX_LEN_NAME_VAR        = 10;

//-------------------------------------------

enum type_arg_t
{
	NUM = 1,
	VAR = 2,
	OP  = 3
};

enum operation_t
{
	ADD  = 1,
	SUB  = 2,
	MUL  = 3,
	DIV  = 4,

	SIN  = 5,
	COS  = 6,
	SH   = 9,
	CH   = 10,

	SQRT = 11
};

union value_t
{
	double      value_num;
	operation_t value_op;
	char        value_var[MAX_LEN_NAME_VAR];
};

struct node_t
{
	value_t     value;
	type_arg_t  type;
	node_t*     left;
	node_t*     right;
	node_t*     parent;
};

//---------------------------------------------------------

struct var_t
{
	char   name_var[MAX_LEN_NAME_VAR];
	double number_in_var;
};

struct table_t
{
	var_t* table_var;
	size_t size_table_var;
	size_t index_free_var;
};

//------------------------------------------------------------

enum diff_error_t
{
	NOT_ERROR 		          = 0,
	NOT_FIND_DIFF_DOT         = 1,
	NOT_FIND_DIFF_HTML        = 2,
	NOT_FIND_TYPE_ARG         = 3,
	NOT_FIND_OP               = 4,
	NOT_MEMORY_FOR_TABLE_VAR  = 5,
	NOT_MEMORY_FOR_NEW_NODE   = 6
};

#endif