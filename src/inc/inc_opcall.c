#include "inc_opcall.h"
#include "lib_descs.h"
#include <stdio.h>


void* opcall_cmd_system(char_t *str)
{
	sysio->opcall(str);
	free(str);
	return NULL;
}
