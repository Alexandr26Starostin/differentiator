#include <stdio.h>

#include "const_in_diff.h"
#include "new_node.h"
#include "delete_tree.h"
#include "dump_tree.h"
#include "read_and_print_formula.h"
#include "launch_diff.h"

// static diff_error_t create_table_var ();

diff_error_t launch_diff ()
{
	diff_error_t status = NOT_ERROR;

	char str_for_system[] = "dot diff.dot -Tsvg -o pictures/diff_0000.svg"; 

	size_t index_picture = 0; 

	FILE* diff_html = fopen ("diff.html", "w");

	if (diff_html == NULL) {printf ("Not find diff.html\n"); return NOT_FIND_DIFF_HTML;}

	printf ("Привет!)\n");

	node_t* node_1 = create_new_node (NUM, 0, NULL, NULL, NULL, __FILE__, __LINE__);
	read_formula (node_1);

	dump_tree (node_1, str_for_system, &index_picture, diff_html);

	print_formula (node_1);
	printf ("\n");

	delete_tree (node_1);

	fclose (diff_html);

	return status;
}