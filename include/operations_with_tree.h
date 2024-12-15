#ifndef OPERATIONS_WITH_TREE_H
#define OPERATIONS_WITH_TREE_H

double       evaluate_tree                     (node_t* node, table_t* table);
double       calculator                        (node_t* node, table_t* table);
node_t*      copy_tree                         (node_t* old_node, node_t* parent_new_node);
node_t*      create_derivate 				   (node_t* node);
node_t*      create_derivate_with_write_in_pdf (FILE* pdf_file, node_t* node, const char** math_phrases, size_t* ptr_index_math_phrases);
diff_error_t count_vars                        (node_t* node, size_t* ptr_quantity_vars);
diff_error_t simplify_tree                     (node_t* node);
size_t       count_depth_of_tree               (node_t* node);

#endif