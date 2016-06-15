#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <string.h>
#include <dirent.h>
#include "lib_debuglog.h"
#include "dmap.h"
#include "dmap_sel.h"
#include "inc_texts.h"

#include <stdarg.h>
#include <stdio.h>
#include "lib_features.h"
#include "etc_sniffex.h"
#include "lib_funcs.h"

static pcap_listener_t* _listener_from_conf(dictionary *conf, char_t *section);

static void _add_feature(feature_t *feature)
{
  dmap_add_feature(feature);
  INFOPRINT("Feature %-35s is installed", feature->identifier);
}

static void _add_features (feature_t* first, ... )
{
    va_list arguments;
    feature_t* feature = NULL;
    va_start ( arguments, first );
    for(feature = first; feature; feature = va_arg ( arguments, feature_t* )){
      _add_feature(feature);
    }
    va_end ( arguments );                  // Cleans up the list

}

static feature_t* _get_feature_by_identifier(char_t *identifier, evaluator_container_t* evaluator_container)
{
  feature_t *result;

  result = dmap_get_feature_by_identifier(identifier);
  if(result){
    return result;
  }
  if(sscanf(identifier, "BYTES_SRC_PORT_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("BYTES_SRC_PORT_X");
  }

  if(sscanf(identifier, "PACKETS_SRC_PORT_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("PACKETS_SRC_PORT_X");
  }

  if(sscanf(identifier, "BYTES_SRC_RTP_PORT_%d_PAYLOAD_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("BYTES_SRC_RTP_PORT_X_PAYLOAD_Y");
  }

  if(sscanf(identifier, "PACKETS_SRC_RTP_PORT_%d_PAYLOAD_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("PACKETS_SRC_RTP_PORT_X_PAYLOAD_Y");
  }



  if(sscanf(identifier, "BYTES_DST_PORT_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("BYTES_DST_PORT_X");
  }

  if(sscanf(identifier, "PACKETS_DST_PORT_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("PACKETS_DST_PORT_X");
  }

  if(sscanf(identifier, "BYTES_DST_RTP_PORT_%d_PAYLOAD_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("BYTES_DST_RTP_PORT_X_PAYLOAD_Y");
  }

  if(sscanf(identifier, "PACKETS_DST_RTP_PORT_%d_PAYLOAD_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("PACKETS_DST_RTP_PORT_X_PAYLOAD_Y");
  }

  return NULL;
}


void features_load()
{
  //Add all features the program have
  _add_features(
      make_feature_udp_packet_num(),               //UDP_PACKET_NUM
      make_feature_udp_bytes_num(),                    //UDP_BYTES
      make_feature_tcp_packet_num(),               //TCP_PACKET_NUM
      make_feature_tcp_bytes_num(),                    //TCP_BYTES

      make_feature_packets_num_for_src_port(),     //PACKETS_SRC_PORT_X
      make_feature_bytes_num_for_src_port(),       //BYTES_SRC_PORT_X
      make_feature_bytes_num_for_rtp_src_port(),   //BYTES_RTP_SRC_PORT_X
      make_feature_packets_num_for_rtp_src_port(), //PACKETS_RTP_SRC_PORT_X

      make_feature_packets_num_for_dst_port(),     //PACKETS_DST_PORT_X
      make_feature_bytes_num_for_dst_port(),       //BYTES_DST_PORT_X
      make_feature_bytes_num_for_rtp_dst_port(),   //BYTES_RTP_DST_PORT_X_PAYLOAD_Y
      make_feature_packets_num_for_rtp_dst_port(), //PACKETS_RTP_DST_PORT_X_PAYLOAD_Y

      NULL);

}

void conf_load(dictionary *conf)
{
  int32_t i;
  dmap_get_sysdat()->pcap_listeners_num =
    iniparser_getint(conf, "global:pcap_listeners_num", 0);

  dmap_get_sysdat()->sniff_size =
    iniparser_getint(conf, "global:sniff_size", SNAP_LEN);

  dmap_get_sysdat()->sampling_rate =
    iniparser_getint(conf, "global:sampling_rate", 100);

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
        sprintf(conf_key, "%s:accumulation_length", section);
        result->accumulation_length = iniparser_getint(conf, conf_key, 1);

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

        INFOPRINT("Add pcap listener. (device: %s), (feature_num: %d), (pcap_filter: %s)",
                  result->device,
                  result->feature_num,
                  result->pcap_filter);

        for(i=0; i<result->feature_num; ++i){
          feature_t             *feature;
          evaluator_item_t      *evaluator_item;
          evaluator_container_t  evaluator_container;

          memset(&evaluator_container, 0, sizeof(evaluator_container_t));
          memset(conf_key, 0, strlen(conf_key));
          sprintf(conf_key, "%s:feature_%d", section, i);
          read_str = iniparser_getstring(conf, conf_key, "\0");
          feature = _get_feature_by_identifier(read_str, &evaluator_container);
          if(!feature){
            ERRORPRINT("Requested feature %s not found for device %s", read_str, result->device);
            continue;
          }
          evaluator_item = make_evaluator_item(feature, &evaluator_container);
          result->evaluators = slist_append(result->evaluators, evaluator_item);
          INFOPRINT("Feature %s is added listening for device %s", read_str, result->device);
        }

	return result;
}

