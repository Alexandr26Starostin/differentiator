#ifndef OPERATIONS_WITH_TREE_H
#define OPERATIONS_WITH_TREE_H

double       evaluate_tree   (node_t* node, table_t* table);
double       calculator      (node_t* node, table_t* table);
node_t*      copy_tree       (node_t* old_node, node_t* parent_new_node);
node_t*      create_derivate (node_t* node);
diff_error_t count_vars      (node_t* node, size_t* ptr_quantity_vars);

#endif