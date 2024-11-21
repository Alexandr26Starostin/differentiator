#ifndef CONST_IN_DIFF_H
#define CONST_IN_DIFF_H

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

struct node_t
{
	long        value;
	type_arg_t  type;
	node_t*     left;
	node_t*     right;
	node_t*     parent;
};

// struct var_t
// {

// };

enum diff_error_t
{
	NOT_ERROR 		   = 0,
	NOT_FIND_DIFF_DOT  = 1,
	NOT_FIND_DIFF_HTML = 2,
	NOT_FIND_TYPE_ARG  = 3,
	NOT_FIND_OP        = 4
};

const size_t INDEX_NUMBER_OF_PICTURE = 36;

#endif