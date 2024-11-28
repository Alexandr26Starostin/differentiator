#include <stdio.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "delete_tree.h"
#include "dump_tree.h"
#include "read_and_print_formula.h"
#include "table_of_var.h"
#include "operations_with_tree.h"
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

	node_t* node_1 = create_new_node (NUM, 0, NULL, NULL, NULL, __FILE__, __LINE__);
	if (node_1 == NULL) {fclose (diff_html); delete_table_var (&table); return NOT_MEMORY_FOR_NEW_NODE;}

	status = read_formula (node_1, &table);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return status;}

	//dump_table_var (&table);

	status = dump_tree (node_1, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_table_var (&table); return status;}

	double number_1 = evaluate_tree (node_1, &table);
	printf ("number_1 = %lg\n", number_1);

	print_formula (node_1);
	printf ("\n\n");

	double num_1 = calculator (node_1, &table);
	printf ("num_1 = %lg\n", num_1);

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

	//---------------------------------------------------------------------------------------------------------------------

	size_t quantity_vars = 0;
	count_vars (node_1, &quantity_vars);
	printf ("\nquantity_vars in node_1 = %ld\n", quantity_vars);

	//----------------------------------------------------------------------------------------------------------------------

	simplify_tree (node_3);

	status = dump_tree (node_3, str_for_system, &index_picture, diff_html);
	if (status) {fclose (diff_html); delete_tree (node_1); delete_tree (node_2); delete_tree (node_3); delete_table_var (&table); return status;}

	//-------------------------------------------------------------------------------------------------------------------------

	delete_tree      (node_1);
	delete_tree      (node_2);
	delete_tree      (node_3);
	delete_table_var (&table);
	fclose           (diff_html);

	return status;
}

//govnocode
//todo yf[eq cnthtnm dc` ] z rfgif b gjnjve rfgie rfgijq cjcfnm [eq hn hn hn hn hn hn hn hn hn h nh n h n hhhhnhnhhnhnn]