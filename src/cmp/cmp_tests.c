/*
 * cmp_tests.c
 *
 *  Created on: Mar 7, 2014
 *      Author: balazs
 */
#include <stdio.h>
#include <stdlib.h>
#include "lib_defs.h"
#include "inc_texts.h"

static void *_cmp_test_helper = NULL;

static void _cmp_test_set_helper_value(void* value)
{
	_cmp_test_helper = value;
}

static void* _cmp_test_get_helper_value()
{
	return _cmp_test_helper;
}

bool_t cmp_test_nothing(void *cmp)
{
	return BOOL_TRUE;
}
