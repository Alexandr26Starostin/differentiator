#ifndef PDF_FILE_H
#define PDF_FILE_H

diff_error_t begin_write_in_pdf  (FILE* pdf_file);
diff_error_t write_tree_in_pdf   (FILE* pdf_file, node_t* node);
diff_error_t write_node_in_pdf   (FILE* pdf_file, node_t* node);
diff_error_t end_write_in_pdf    (FILE* pdf_file);
diff_error_t draw_chart_in_pdf   (FILE* pdf_file, node_t* node);
diff_error_t decompose_by_Taylor (FILE* pdf_file, node_t* node, const char** math_phrases, size_t* ptr_index_math_phrases, table_t* table);

#endif