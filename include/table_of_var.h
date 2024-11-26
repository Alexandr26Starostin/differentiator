#ifndef TABLE_OF_VAR_H
#define TABLE_OF_VAR_H

diff_error_t create_table_var  (table_t* table);
diff_error_t delete_table_var  (table_t* table);
diff_error_t realloc_table_var (table_t* table);
double*      find_var_in_table (table_t* table, char* name_var);
diff_error_t add_var_in_table  (table_t* table, char* name_var, double number_var);
diff_error_t dump_table_var    (table_t* table);

#endif