#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <string.h>
#include <dirent.h>
#include "lib_debuglog.h"
#include "dmap.h"
#include "inc_texts.h"

#include <stdarg.h>
#include <stdio.h>

static pcap_listener_t* _listener_from_conf(dictionary *conf, char_t *section);

static void _add_feature(feature_t *feature)
{
  dmap_add_features(feature);
  INFOPRINT("Feature %s (ID:%s) is installed", feature->name, feature->identifier);
}

static void _add_features (feature_t* first, ... )
{
    va_list arguments;
    feature_t* feature = NULL;
    va_start ( arguments, first );
    for(feature = first; feature; feature = va_arg ( arguments, feature_t )){
      _add_feature(feature);
    }
    va_end ( arguments );                  // Cleans up the list

}


void features_load()
{
  //Add all features the program have
  _add_features(
      make_feature_udp_packet_num(), //UDP_PACKET_NUM
      NULL);

}

void conf_load(dictionary *conf)
{
  int32_t i;
  dmap_get_sysdat()->pcap_listeners_num =
    iniparser_getint(conf, "global:pcap_listeners_num", 0);

  dmap_get_sysdat()->sniff_size =
    iniparser_getint(conf, "global:sniff_size");

  dmap_get_sysdat()->sampling_rate =
    iniparser_getint(conf, "global:sampling_rate");

  for(i=0; i<dmap_get_sysdat()->pcap_listeners_num; ++i){
    pcap_listener_t* pcap_listener;
    char_t section[255];
    sprintf(section, "pcap_%d", i);
    pcap_listener = _listener_from_conf(conf, section);
    dmap_add_pcapls(pcap_listener);
  }
}

pcap_listener_t* _listener_from_conf(dictionary *conf, char_t *section)
{
	char_t  conf_key[255];
	char_t *read_str;
	pcap_listener_t *result;
	int32_t i;

	result = pcap_listener_ctor();

	sprintf(conf_key, "%s:device", section);
	read_str = iniparser_getstring(conf, conf_key, "veth0");
	strcpy(result->device, read_str);

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:feature_num", section);
        result->feature_num = iniparser_getint(conf, conf_key, 0);

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:output", section);
        read_str = iniparser_getstring(conf, conf_key, "output.csv");
        strcpy(result->output, read_str);

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:pcap_filter", section);
        read_str = iniparser_getstring(conf, conf_key, "\0");
        strcpy(result->pcap_filter, read_str);

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:append_output", section);
        result->append_output = iniparser_getint(conf, conf_key, 0);

        for(i=0; i<result->feature_num; ++i){
          feature_t *feature;
          memset(conf_key, 0, strlen(conf_key));
          sprintf(conf_key, "%s:feature_%d", section, i);
          read_str = iniparser_getstring(conf, conf_key, "\0");
          feature = dmap_get_feature_by_identifier(read_str);
          if(!feature){
            continue;
          }
          datchain_add_item(result->features, feature);
        }

	return result;
}

void _load_dir(char_t* dir, void*(*loader)(char_t*))
{
	DIR *dp;
	char_t file[255];
	char_t ext[8];
	char_t path[255];
	struct dirent *ep;
	dp = opendir (dir);
	if (dp == NULL){
		ERRORPRINT("Couldn't open the directory %s", dir);
		return;
	}

	while ((ep = readdir(dp))){
		strcpy(ext, &ep->d_name[strlen(ep->d_name) - 3]);
		if(strcmp(ext, "ini") != 0){
			continue;
		}
		sprintf(path, "%s/%s", dir, ep->d_name);
		loader(path);
	}

	(void) closedir (dp);
}
