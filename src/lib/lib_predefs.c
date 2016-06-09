/*
 * lib_predefs.c
 *
 *  Created on: Mar 9, 2014
 *      Author: balazs
 */
#include "lib_defs.h"

void func_do_nothing()
{
	/*nothing*/
}

void func_with_param_do_nothing(void *param)
{
	/*nothing*/
}

bool_t func_return_true()
{
	return BOOL_TRUE;
}

bool_t func_with_param_return_true(void *param)
{
	return BOOL_TRUE;
}
