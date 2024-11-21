#include <stdio.h>

#include "const_in_diff.h"
#include "launch_diff.h"

int main ()
{
	diff_error_t status = launch_diff ();
	
	return status;
}
