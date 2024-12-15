#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "const_in_diff.h"
#include "operations_with_tree.h"
#include "read_and_print_formula.h"
#include "delete_tree.h"
#include "pdf_file.h"

#define WRITE_MUL_IN_PDF_(operation, str_with_operation)                                                           											\
	case operation:                                                                                                    										\
	{                                                                                                                  										\
		if (node -> left -> type == OP && (node -> left -> value.value_op == ADD || node -> left -> value.value_op == SUB)) {fprintf (pdf_file, "(");}   	\
																																							\
		write_node_in_pdf (pdf_file, node -> left);   																										\
																																						    \
		if (node -> left -> type == OP && (node -> left -> value.value_op == ADD || node -> left -> value.value_op == SUB)) {fprintf (pdf_file, ")");}   	\
																																							\
		fprintf (pdf_file, str_with_operation);           																									\
																																							\
		if (node -> right -> type == OP && (node -> right -> value.value_op == ADD || node -> left -> value.value_op == SUB)) {fprintf (pdf_file, "(");}  	\
																																							\
		write_node_in_pdf (pdf_file, node -> right);    																									\
																																							\
		if (node -> right -> type == OP && (node -> right -> value.value_op == ADD || node -> left -> value.value_op == SUB)) {fprintf (pdf_file, ")");}  	\
																																							\
		return NOT_ERROR;                               																									\
	}	

#define WRITE_ADD_SUB_IN_PDF_(operation, str_with_operation)  \
	case operation:                                           \
	{                                                         \
		write_node_in_pdf (pdf_file, node -> left);           \
		fprintf (pdf_file, str_with_operation);               \
		write_node_in_pdf (pdf_file, node -> right);          \
		return NOT_ERROR;                                     \
	}

#define WRITE_DIV_IN_PDF_(operation, str_with_operation)           \
	case operation:                                     			\
	{																\
		fprintf (pdf_file, str_with_operation);                     \
		write_node_in_pdf (pdf_file, node -> left);     			\
		fprintf (pdf_file, "}{");   								\
		write_node_in_pdf (pdf_file, node -> right);    			\
		fprintf (pdf_file, "} ");   								\
		return NOT_ERROR;                               			\
	}

#define WRITE_LOG_IN_PDF_(operation, str_with_operation)            \
	case operation:                                     			\
	{																\
		fprintf (pdf_file, str_with_operation);                     \
		write_node_in_pdf (pdf_file, node -> left);     			\
		fprintf (pdf_file, "}{(");   								\
		write_node_in_pdf (pdf_file, node -> right);    			\
		fprintf (pdf_file, ")} ");   								\
		return NOT_ERROR;                               			\
	}

#define WRITE_FUNC_OP_IN_PDF_(operation, str_with_operation)    \
	case operation:                                             \
	{                                                           \
		fprintf (pdf_file, str_with_operation);                 \
		write_node_in_pdf (pdf_file, node -> right);            \
		fprintf (pdf_file, ")} ");                               \
		return NOT_ERROR;                                       \
	}

#define WRITE_DEG_IN_PDF_                                       \
	case DEG:                                                   \
	{            												\
		fprintf (pdf_file, " {");                               \
		write_node_in_pdf (pdf_file, node -> left);     		\
		fprintf (pdf_file, "}^{");   							\
		write_node_in_pdf (pdf_file, node -> right);    		\
		fprintf (pdf_file, "} ");   							\
		return NOT_ERROR;                               		\
	}

static diff_error_t write_op_in_pdf        (FILE* pdf_file, node_t* node);
static diff_error_t get_value_for_variable (double* ptr_variable);
static double       factorial              (size_t value);

static diff_error_t print_taylor_formula (FILE* pdf_file, size_t number_of_derivate, double member_of_decomposition, 
                                          double* coefficients_in_taylor, size_t index_of_coefficients , 
										  double position_of_decomposition);
								
static diff_error_t draw_taylor_in_pdf (FILE* pdf_file, node_t* node, size_t number_of_derivate, 
  										double member_of_decomposition, double* coefficients_in_taylor, 
										size_t index_of_coefficients, double position_of_decomposition);

//---------------------------------------------------------------------------------------------------------------------------------------------------------

diff_error_t begin_write_in_pdf (FILE* pdf_file)
{
	assert (pdf_file);

	fprintf (pdf_file, 
			"\\documentclass[a4paper]{article}\n"
			"\\usepackage[utf8]{inputenc}\n"
			"\\usepackage[russian,english]{babel}\n"
			"\\usepackage[T2A]{fontenc}\n"
			"\\usepackage[left=10mm, top=20mm, right=18mm, bottom=15mm, footskip=10mm]{geometry}\n"
			"\\usepackage{indentfirst}\n"
			"\\usepackage{amsmath,amssymb}\n"
			"\\usepackage[italicdiff]{physics}\n"
			"\\usepackage{graphicx}\n"
			"\\graphicspath{{images/}}\n"
			"\\DeclareGraphicsExtensions{.pdf,.png,.jpg}\n"
			"\\usepackage{wrapfig}\n"
			"\\usepackage{pgfplots}\n\n"
			"\\usepackage{caption}\n"
			"\\captionsetup[figure]{name=Рисунок}\n"
			"\\captionsetup[table]{name=Таблица}\n\n"
			"\\title{\\underline{Дифференциатор}}\n"
			"\\author{Старостин Александр, Б01-401}\n"
			"\\date {26 декабря, 2024 год}\n\n\n"
			"\\begin{document}\n\n"
			"\\maketitle\n"
			"\\newpage\n\n");

	return NOT_ERROR;
}

diff_error_t end_write_in_pdf (FILE* pdf_file)
{
	assert (pdf_file);

	fprintf (pdf_file, "\\end{document}\n");

	return NOT_ERROR;
}

diff_error_t write_node_in_pdf (FILE* pdf_file, node_t* node)
{
	assert (pdf_file);
	assert (node);

	switch (node -> type)
	{
		case NUM: fprintf (pdf_file, "%lg", (node -> value).value_num); return NOT_ERROR;

		case VAR: fprintf (pdf_file, "%s",  (node -> value).value_var); return NOT_ERROR;

		case OP: write_op_in_pdf (pdf_file, node); return NOT_ERROR;

		default: printf ("%s:%d\n'write_tree_in_pdf' not find type == %d\n", 
		   				__FILE__, __LINE__, node -> type); return NOT_FIND_TYPE_ARG;
	}
}

diff_error_t write_op_in_pdf (FILE* pdf_file, node_t* node)
{
	assert (pdf_file);
	assert (node);

	switch ((node -> value).value_op)
	{
		WRITE_ADD_SUB_IN_PDF_(ADD, " + ")
		WRITE_ADD_SUB_IN_PDF_(SUB, " - ")
		
		WRITE_MUL_IN_PDF_(MUL, " \\cdot ")

		WRITE_DIV_IN_PDF_(DIV, " \\frac{")
		WRITE_LOG_IN_PDF_(LOG, " \\log_{")

		WRITE_FUNC_OP_IN_PDF_(SIN,  " \\sin{(")
		WRITE_FUNC_OP_IN_PDF_(COS,  " \\cos{(")
		WRITE_FUNC_OP_IN_PDF_(SH,   " \\sinh{(")
		WRITE_FUNC_OP_IN_PDF_(CH,   " \\cosh{(")
		WRITE_FUNC_OP_IN_PDF_(SQRT, " \\sqrt{(")
		WRITE_FUNC_OP_IN_PDF_(LN,   " \\ln{(")

		WRITE_DEG_IN_PDF_

		default: printf ("%s:%d\n'write_op_in_pdf' not find operation == %d\n", 
						  __FILE__, __LINE__, (node -> value).value_op); return NOT_FIND_OP;
	}
}

diff_error_t write_tree_in_pdf (FILE* pdf_file, node_t* node)
{
	assert (pdf_file);
	assert (node);

	write_node_in_pdf (pdf_file, node);

	fprintf (pdf_file, " \\]\n\n");

	return NOT_ERROR;
}

diff_error_t draw_chart_in_pdf (FILE* pdf_file, node_t* node)
{
	assert (pdf_file);
	assert (node);

	double x_max = 0;
	double y_max = 0;

	printf ("from draw_chart_in_pdf:\n\n");

	printf ("x_max = ");
	get_value_for_variable (&x_max);

	printf ("y_max = ");
	get_value_for_variable (&y_max);

	fprintf (pdf_file, "\\begin{figure}[h]\n"
					   "\\centering\n"
					   "\\begin{tikzpicture}\n"
					   "\\begin{axis}\n[\n"
					   "\tlegend pos = north west,\n"
					   "\txlabel = {$x$},\n"
					   "\tylabel = {$f(x)$},\n"
					   "\twidth = 300,\n"
					   "\theight = 300,\n"
					   "\trestrict x to domain=%lg:%lg,\n"
					   "\trestrict y to domain=%lg:%lg,\n"
					   "\tgrid = major,\n"
					   "\tenlargelimits=true,\n]\n", 0 - x_max, 0 + x_max, 0 - y_max, 0 + y_max);

	fprintf (pdf_file, "\\legend{\n"
					   "\t$f(x)$\n}");

	fprintf (pdf_file, "\\addplot[green, samples=750]{");

	print_formula (pdf_file, node);

	fprintf (pdf_file, "};"
					   "\\end{axis}\n"
					   "\\end{tikzpicture}\n"
					   "\\caption{График функции}\n"
					   "\\end{figure}\n");

	return NOT_ERROR;
}

static diff_error_t get_value_for_variable (double* ptr_variable)
{
	assert (ptr_variable);

	if (scanf ("%lg", ptr_variable) == EOF)
	{
		char symbol = '\0';
		while ((symbol = (char) getchar ()) != '\n') {}

		printf ("Некорректный ввод значения. Пожалуйста повтори:\n");

		get_value_for_variable (ptr_variable);
		return NOT_ERROR;
	}

	if (getchar () != '\n')
	{
		char symbol = '\0';
		while ((symbol = (char) getchar ()) != '\n') {}

		printf ("Некорректный ввод значения. Пожалуйста повтори:\n");

		get_value_for_variable (ptr_variable);
		return NOT_ERROR;
	}

	return NOT_ERROR;
}

diff_error_t decompose_by_Taylor (FILE* pdf_file, node_t* node, const char** math_phrases, size_t* ptr_index_math_phrases, table_t* table)
{
	assert (pdf_file);
	assert (node);
	assert (math_phrases);
	assert (ptr_index_math_phrases);
	assert (table);

	double member_of_decomposition   = 0;
	double position_of_decomposition = 0;
	size_t number_of_derivate        = 0;

	node_t* diff_node = NULL;
	node_t* save_node = node;

	printf ("Разложение Тейлора до члена: ");
	get_value_for_variable (&member_of_decomposition);
	printf ("member_of_decomposition = %ld\n", (size_t) member_of_decomposition);

	printf ("Разложение Тейлора в точке x_0 = ");
	get_value_for_variable (&position_of_decomposition);
	printf ("position_of_decomposition = %lg\n", position_of_decomposition);

	size_t index = 0;  //Индекс переменной, доделать поиск

	((table -> table_var)[index]).number_in_var = position_of_decomposition;

	double* coefficients_in_taylor = (double*) calloc ((size_t) member_of_decomposition + 1, sizeof (double));
	if (coefficients_in_taylor == NULL) {printf ("Not memory for coefficients_in_taylor\n"); return NOT_MEMORY_IN_COEFFICIENTS_IN_TAYLOR;}

	size_t index_of_coefficients = 0;
	coefficients_in_taylor[index_of_coefficients++] = evaluate_tree (node, table);

	fprintf (pdf_file, "\\[ x_0 = %lg\\]\n\n", position_of_decomposition);

	fprintf (pdf_file, "Найдём все производные функции, нужных нам порядков:\n");

	for (; number_of_derivate < (size_t) member_of_decomposition; number_of_derivate++)
	{
		fprintf (pdf_file, "\\[ f^{(%ld)}(x) = ", number_of_derivate);
		write_tree_in_pdf  (pdf_file, node);

		fprintf (pdf_file, "Найдём её производную:\n\n\\newpage\n");

		diff_node = create_derivate_with_write_in_pdf (pdf_file, node, math_phrases, ptr_index_math_phrases);
		if (diff_node == NULL) {delete_tree (node); return NOT_MEMORY_FOR_NEW_NODE;}

		if (number_of_derivate != 0)
			delete_tree (node);

		node = diff_node;

		coefficients_in_taylor[index_of_coefficients++] = evaluate_tree (node, table);
	}

	fprintf (pdf_file, "\\[ f^{(%ld)}(x) = ", number_of_derivate);
	write_tree_in_pdf  (pdf_file, node);

	delete_tree (node);

	fprintf (pdf_file, "\\newpage\n");

	number_of_derivate    = 0;
	index_of_coefficients = 0;

	fprintf (pdf_file, "Тогда разложение функции по Тейлору в точке $x_0$ имеет вид:\n\n"
					   "\\[g(x) = ");

	print_taylor_formula (pdf_file, number_of_derivate, member_of_decomposition, coefficients_in_taylor, index_of_coefficients, position_of_decomposition);
	fprintf (pdf_file, "\\]\n\n");


	fprintf (pdf_file, "График разложение функции по Тейлору в точке $x_0$:\n\n");

	draw_taylor_in_pdf (pdf_file, save_node, number_of_derivate, member_of_decomposition, coefficients_in_taylor, index_of_coefficients, position_of_decomposition);

	free (coefficients_in_taylor);

	return NOT_ERROR;
}

static double factorial (size_t value)
{
	if (value == 1 || value == 0) {return 1;}

	return (double) value * factorial (value - 1);
}

static diff_error_t print_taylor_formula (FILE* pdf_file, size_t number_of_derivate, double member_of_decomposition, 
                                          double* coefficients_in_taylor, size_t index_of_coefficients, 
										  double position_of_decomposition)
{
	assert (pdf_file);
	assert (coefficients_in_taylor);

	for (; number_of_derivate < (size_t) member_of_decomposition; number_of_derivate++)
	{
		fprintf (pdf_file, "%lg * (x-%lg)^%ld + ",    // \\cdot
			coefficients_in_taylor[index_of_coefficients++] / factorial (number_of_derivate), position_of_decomposition, number_of_derivate);
	}

	fprintf (pdf_file, "%lg * (x-%lg)^%ld", 
			coefficients_in_taylor[index_of_coefficients++] / factorial (number_of_derivate), position_of_decomposition, number_of_derivate);

	return NOT_ERROR;
}

static diff_error_t draw_taylor_in_pdf (FILE* pdf_file, node_t* node, size_t number_of_derivate, 
  										double member_of_decomposition, double* coefficients_in_taylor, 
										size_t index_of_coefficients, double position_of_decomposition)
{
	assert (pdf_file);
	assert (node);
	assert (coefficients_in_taylor);

	double x_max = 0;
	double y_max = 0;

	const char*  colors[SIZE_COLORS] = {"red", "yellow", "black", "blue", "orange", "violet", "pink"};
	size_t index_color         = 0;

	printf ("from draw_chart_in_pdf:\n\n");

	printf ("x_width_taylor = ");
	get_value_for_variable (&x_max);

	printf ("y_max_taylor = ");
	get_value_for_variable (&y_max);

	fprintf (pdf_file, "\\begin{figure}[h]\n"
					   "\\centering\n"
					   "\\begin{tikzpicture}\n"
					   "\\begin{axis}\n[\n"
					   "\tlegend pos = north west,\n"
					   "\txlabel = {$x$},\n"
					   "\tylabel = {$f(x)$},\n"
					   "\twidth = 300,\n"
					   "\theight = 300,\n"
					   "\trestrict x to domain=%lg:%lg,\n"
					   "\trestrict y to domain=%lg:%lg,\n"
					   "\tgrid = major,\n"
					   "\tenlargelimits=true,\n]\n", position_of_decomposition - x_max, position_of_decomposition + x_max, 
					   							      0 - y_max, 0 + y_max);

	fprintf (pdf_file, "\\legend{\n"
	                   "\t$f(x)$,\n");  


	for (size_t index = 0; index < (size_t) member_of_decomposition; index++)
	{
		fprintf (pdf_file, "\t$f^{(%ld)}(x)$,\n", index);
	}
	fprintf (pdf_file, "\t$f^{(%ld)}(x)$\n}", (size_t) member_of_decomposition);

	fprintf (pdf_file, "\\addplot[green, samples=750]{");
	print_formula (pdf_file, node);
	fprintf (pdf_file, "};\n");

	for (size_t index = 0; index <= (size_t) member_of_decomposition; index++)
	{
			fprintf (pdf_file, "\\addplot[%s, samples=750]{", colors[index_color++]);

			if (index_color == SIZE_COLORS) {index_color = 0;}

			print_taylor_formula (pdf_file, number_of_derivate, (double) index, coefficients_in_taylor, index_of_coefficients, position_of_decomposition);
			fprintf (pdf_file, "};\n");
	}

	fprintf (pdf_file, "\\end{axis}\n"
					   "\\end{tikzpicture}\n"
					   "\\caption{Разложение по Тейлору}\n"
					   "\\end{figure}\n");

	return NOT_ERROR;
}

