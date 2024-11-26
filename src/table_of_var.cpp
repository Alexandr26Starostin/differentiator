#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "const_in_diff.h"
#include "table_of_var.h"

diff_error_t create_table_var (table_t* table)
{
	assert (table);

	table -> table_var = (var_t*) calloc (MIN_SIZE_TABLE_VAR, sizeof (var_t));

	if (table -> table_var == NULL) {printf ("%s:%d Not memory for create table_var\n", __FILE__, __LINE__); return NOT_MEMORY_FOR_TABLE_VAR;}

	table -> size_table_var = MIN_SIZE_TABLE_VAR;
	table -> index_free_var = 0;

	return NOT_ERROR;
}

diff_error_t delete_table_var (table_t* table)
{
	assert (table);

	free (table -> table_var);

	table -> size_table_var = 0;
	table -> index_free_var = 0;

	return NOT_ERROR;
}

diff_error_t realloc_table_var (table_t* table)
{
	assert (table);

	table -> size_table_var *= 2;

	table -> table_var = (var_t*) realloc (table -> table_var, table -> size_table_var);

	if (table -> table_var == NULL) {printf ("%s:%d Not memory for realloc table_var\n",  __FILE__, __LINE__); return NOT_MEMORY_FOR_TABLE_VAR;}

	return NOT_ERROR;
}

double* find_var_in_table (table_t* table, char* name_var)
{
	assert (table);
	assert (name_var);

	for (size_t index_var = 0; index_var < table -> index_free_var; index_var++)
	{
		if (strcmp (((table -> table_var)[index_var]).name_var, name_var) == 0)
		{
			return &(((table -> table_var)[index_var]).number_in_var);
		}
	}

	return NULL;
}

diff_error_t add_var_in_table (table_t* table, char* name_var, double number_var)
{
	assert (table);
	assert (name_var);

	diff_error_t status = NOT_ERROR;

	if (find_var_in_table (table, name_var) != NULL)
	{
		return status;
	}
	
	strcpy (((table -> table_var)[table -> index_free_var]).name_var, name_var);

	((table -> table_var)[table -> index_free_var]).number_in_var = number_var;

	table -> index_free_var += 1;

	if (table -> index_free_var == table -> size_table_var)
	{
		status = realloc_table_var (table);
	}

	return status;
}

diff_error_t dump_table_var (table_t* table)
{
	assert (table);

	printf ("\n-----------------------------------------------------------------------------------------------------\n\n");

	printf ("index   name_var    number_in_var\n");

	for (size_t index = 0; index < table -> size_table_var; index++)
	{
		printf ("%5ld %10s %5lg", index, ((table -> table_var)[index]).name_var, ((table -> table_var)[index]).number_in_var);

		if (index == table -> index_free_var)
		{
			printf (" <--");
		}

		printf ("\n");
	}

	printf ("\n-----------------------------------------------------------------------------------------------------\n\n");

	return NOT_ERROR;
}