#ifndef OPERATIONS_WITH_TREE_H
#define OPERATIONS_WITH_TREE_H

double       evaluate_tree (node_t* node, table_t* table);
double       calculator    (node_t* node, table_t* table);
diff_error_t copy_tree     (node_t* old_node, node_t* new_node);

#endif