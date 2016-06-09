#ifndef INCGUARD_MPT_DMAP_DEFINITIONS_H_
#define INCGUARD_MPT_DMAP_DEFINITIONS_H_

#include "lib_defs.h"
#include "lib_threading.h"
#include "dmap_predefs.h"

#define DMAP_NAME "Managed datamap"
#define DMAP_NAME_TABLE_ETC "Etc table"
#define DMAP_NAME_TABLE_CON "Connection table" //16
#define DMAP_NAME_TABLE_TUN "Tunnel table"
#define DMAP_NAME_TABLE_INF "Interface table" //15
#define DMAP_NAME_TABLE_NET "Networks table"
#define DMAP_NAME_TABLE_PTH "Path table"
#define DMAP_NAME_TABLE_THR "Threads tables" //14
#define DMAP_NAME_SYSDAT    "System global data"//18
#define DMAP_NAME_TABLE_CLK_EVENT "Clock events table" //18
#define DMAP_NAME_TABLE_SCH "Scheduler table" //18

typedef struct dmap_row_struct_t{
	void *item;
	void (*dtor)(void*);
}dmap_row_t;

typedef struct dmap_table_struct_t{
	char_t      		name[64];
	int32_t     		size;
	rwmutex_t           *rwmutex;
	dmap_row_t 			**rows;
}dmap_table_t;

#endif //INCGUARD_MPT_DMAP_DEFINITIONS_H_
