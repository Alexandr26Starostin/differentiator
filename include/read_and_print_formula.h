#ifndef READ_AND_PRINT_FORMULA_H
#define READ_AND_PRINT_FORMULA_H

diff_error_t print_formula (FILE* pdf_file, node_t* node);
diff_error_t read_formula  (node_t** ptr_node, table_t* table);

#endif