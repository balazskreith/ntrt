#include "sys_ctrls.h"
#include "lib_makers.h"
#include "lib_funcs.h"
#include "etc_utils.h"
#include "sys_prints.h"
#include "dmap.h"
#include "lib_makers.h"
#include "lib_tors.h"
#include "dmap_sel.h"
#include "sys_confs.h"
#include "inc_opcall.h"
#include <dirent.h>

#include "../cmp/cmp_evaluator.h"
#include "../cmp/cmp_recorder.h"
#include "../cmp/cmp_recorder.h"
#include "fsm.h"

void* _cmd_rcall(void (*action)())
{
	action();
	return NULL;
}
