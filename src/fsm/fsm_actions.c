#include "fsm_actions.h"
#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <dirent.h>
#include "etc_iniparser.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "etc_utils.h"
#include <time.h>

void con_load_from_file(char_t *filename)
{
	dictionary    *conf ;
	int32_t        index;
	char_t         section[128];
	char_t        *read_str;
	FILE          *fd;
	char_t         line[128];
	int32_t        conn_id;

	fd = fopen(filename, "r");
	if (fd == NULL)
	{   sprintf(line , "File read open error: %s\n",filename);
		EXERROR(line, 3);
	}
	fclose(fd);

	conf = iniparser_load(filename);
	if (conf==NULL) {
		fprintf(stdout, "cannot parse file: %s\n", filename);
		EXERROR("Critical error.", 3);
	}
	//iniparser_dump(conf, stderr);

	dmap_lock_sysdat();
	dmap_wrlock_table_pcapls();

	conf_load(conf);

	iniparser_freedict(conf);

	dmap_wrunlock_table_pcapls();
	dmap_unlock_sysdat();
}

