#include <stdio.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "delete_tree.h"
#include "dump_tree.h"
#include "read_and_print_formula.h"
#include "table_of_var.h"
#include "operations_with_tree.h"
#include "pdf_file.h"
#include "launch_diff.h"

diff_error_t launch_diff ()
{
	diff_error_t status = NOT_ERROR;

	char str_for_system[] = "dot diff.dot -Tsvg -o pictures/diff_0000.svg"; 

	size_t index_picture = 0; 

	FILE* diff_html = fopen ("diff.html", "w");

	if (diff_html == NULL) {printf ("Not find diff.html\n"); return NOT_FIND_DIFF_HTML;}

	//--------------------------------------------------------------------------------------------------------

	table_t table = {};

	status = create_table_var (&table);

	if (status) {fclose (diff_html); delete_table_var (&table); return status;}

	//--------------------------------------------------------------------------------------------------------

	printf ("Привет!)\n");

	node_t* node_1 = NULL;

	status = read_formula (&node_1, &table);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return status;}

	//dump_table_var (&table);

	status = dump_tree (node_1, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return status;}

	// double number_1 = evaluate_tree (node_1, &table);
	// printf ("number_1 = %lg\n", number_1);

	// print_formula (node_1);
	// printf ("\n\n");

	double num_1 = calculator (node_1, &table);
	printf ("answer = %lg\n", num_1);

	//---------------------------------------------------------------------------------------------------------------

	node_t* node_2 = copy_tree (node_1, NULL);
	if (node_2 == NULL) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return NOT_MEMORY_FOR_NEW_NODE;}

	status = dump_tree (node_2, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_table_var (&table); return status;}

	//---------------------------------------------------------------------------------------------------------------

	node_t* node_3 = create_derivate (node_1);
	if (node_3 == NULL) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_table_var (&table); return NOT_MEMORY_FOR_NEW_NODE;}

	status = dump_tree (node_3, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_tree (node_3); delete_table_var (&table); return status;}

	//----------------------------------------------------------------------------------------------------------------------

	simplify_tree (node_3);

	status = dump_tree (node_3, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_tree (node_3); delete_table_var (&table); return status;}

	//-------------------------------------------------------------------------------------------------------------------------

	FILE* pdf_file = fopen ("pdf_diff.txt", "w");
	if (pdf_file == NULL) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return status;}

	const char* math_phrases[SIZE_MATH_PHRASES] =  {"Из здравого смысла следует:\n", 
													"Советские сперматозоиды установили, что:\n",
													"Очевидно (иначе тебе следует научиться думать!!!):\n",
													"Школьникам известно:\n",
													"Знамка говорила:\n",
													"Аналогично:\n",
													"Отвечаю, что:\n",
													"Зуб даю:\n",
													"Ну.... Поверь:\n",
													"Поверь, что это так:\n",
													"Только тёмные не знают, что:\n",
													"Неужели сам не можешь взять...... Зачем на ФизТех пошёл?... Очевидно:\n",
													"Древние греки знали, а ты, видимо, - нет, что:\n"
													"Смотри и учись:"};

	size_t index_math_phrases = 0;

	begin_write_in_pdf (pdf_file);

	fprintf (pdf_file, "\\section{Взятие производной функции}\n");

	fprintf (pdf_file, "Нам дана функция:\n");

	fprintf (pdf_file, "\\[ f(x) = ");

	write_tree_in_pdf  (pdf_file, node_2);

	fprintf (pdf_file, "Её производная равна:\n");

	fprintf (pdf_file, "\\[ f'(x) = ");

	write_tree_in_pdf  (pdf_file, node_3);

	fprintf (pdf_file, "Посмотрим, как мы её взяли.\n");

	delete_tree (node_2);
	delete_tree (node_3);

	node_2 = copy_tree (node_1, NULL);
	if (node_2 == NULL) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return NOT_MEMORY_FOR_NEW_NODE;}

	node_3 = create_derivate_with_write_in_pdf (pdf_file, node_2, math_phrases, &index_math_phrases);
	if (node_3 == NULL) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_table_var (&table); return NOT_MEMORY_FOR_NEW_NODE;}

	fprintf (pdf_file, "\\section{Построение графика функции}\n\n");

	draw_chart_in_pdf (pdf_file, node_2);

	fprintf (pdf_file, "\\section{Разложение функции по Тейлору в точке x_0}\n\n");

	decompose_by_Taylor (pdf_file, node_2, math_phrases, &index_math_phrases, &table);

	end_write_in_pdf   (pdf_file);

	fclose (pdf_file);

	//-------------------------------------------------------------------------------------------------------------------------

	delete_tree      (node_1);
	delete_tree      (node_2);
	delete_tree      (node_3);
	delete_table_var (&table);
	fclose           (diff_html);

	return status;
}