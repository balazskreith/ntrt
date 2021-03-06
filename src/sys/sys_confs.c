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
#include "lib_groupcounters.h"
#include "etc_sniffex.h"
#include "lib_funcs.h"

static pcap_listener_t* _listener_from_conf(dictionary *conf, char_t *section);

static void _add_feature(feature_t *feature)
{
  dmap_add_feature(feature);
  INFOPRINT("Feature %-35s is installed", feature->identifier);
}


static void _add_groupcounter_prototype(groupcounter_prototype_t *groupcounter)
{
  dmap_add_groupcounter_proto(groupcounter);
  INFOPRINT("Groupcounter %-35s is installed", groupcounter->identifier);
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

static void _add_groupcounter_prototypes (groupcounter_prototype_t* first, ... )
{
    va_list arguments;
    groupcounter_prototype_t* groupcounter = NULL;
    va_start ( arguments, first );
    for(groupcounter = first; groupcounter; groupcounter = va_arg ( arguments, groupcounter_prototype_t* )){
      _add_groupcounter_prototype(groupcounter);
    }
    va_end ( arguments );                  // Cleans up the list

}

static void _add_mapped_var(int32_t map_id)
{
  mapped_var_t *mapped_var;
  dmap_wrlock_table_mapped_var();
  if(dmap_get_mapped_var_by_var_id(map_id) != NULL){
    WARNINGPRINT("Mapped var with id: %d is already added", map_id);
    goto done;
  }
  mapped_var = make_mapped_var(map_id);
  if(dmap_add_mapped_var(mapped_var) < 0){
    WARNINGPRINT("Mapped var with id: %d can not be added to DMAP", map_id);
    mapped_var_dtor(mapped_var);
    goto done;
  }
  INFOPRINT("Mapped var with id %d is successfully addded to DMAP", map_id);
done:
  dmap_wrunlock_table_mapped_var();
}

static feature_t* _get_feature_by_identifier(char_t *identifier, evaluator_container_t* evaluator_container)
{
  feature_t *result;

  result = dmap_get_feature_by_identifier(identifier);
  if(result){
    return result;
  }
  
  {
    uint8_t ip[4];
    if(sscanf(identifier, "IP_PACKETS_BY_SRC_%d_%d_%d_%d", ip, ip+1, ip+2, ip+3) == 4) {
       evaluator_container->src_ipv4 = ip[0] + 
                    (((uint32_t)ip[1]) << 8) +
                    (((uint32_t)ip[2]) << 16) +
                    (((uint32_t)ip[3]) << 24);
       return dmap_get_feature_by_identifier("IP_PACKETS_BY_SRC_X_X_X_X");
    }
  }
  

  if(sscanf(identifier, "RTP_PACKETS_%d",&evaluator_container->port_num) == 1){
     return dmap_get_feature_by_identifier("RTP_PACKETS_X");
   }

   if(sscanf(identifier, "RTP_BYTES_%d",&evaluator_container->port_num) == 1){
     return dmap_get_feature_by_identifier("RTP_BYTES_X");
   }

   if(sscanf(identifier, "LOST_RTP_PACKETS_%d", &evaluator_container->payload_type) == 1){
     return dmap_get_feature_by_identifier("LOST_RTP_PACKETS_X");
   }

   if(sscanf(identifier, "LOST_RTP_FRAMES_%d", &evaluator_container->payload_type) == 1){
     return dmap_get_feature_by_identifier("LOST_RTP_FRAMES_X");
   }

   if(sscanf(identifier, "SRC_UDP_BYTES_%d", &evaluator_container->port_num) == 1){
     return dmap_get_feature_by_identifier("SRC_UDP_BYTES_X");
   }

   if(sscanf(identifier, "SRC_UDP_PACKETS_%d", &evaluator_container->port_num) == 1){
     return dmap_get_feature_by_identifier("SRC_UDP_PACKETS_X");
   }

  if(sscanf(identifier, "SRC_TCP_PACKETS_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("SRC_TCP_PACKETS_X");
  }

  if(sscanf(identifier, "SRC_TCP_BYTES_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("SRC_TCP_BYTES_X");
  }
  
//  if(sscanf(identifier, "SRC_IP_PACKETS_%d_%d_%d_%d", &evaluator_container->port_num) == 1){
//    return dmap_get_feature_by_identifier("SRC_UDP_PACKETS_X");
//  }

  if(sscanf(identifier, "SRC_RTP_PACKETS_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("SRC_RTP_PACKETS_X_Y");
  }

  if(sscanf(identifier, "SRC_RTP_BYTES_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("SRC_RTP_BYTES_X_Y");
  }

  if(sscanf(identifier, "SRC_LOST_RTP_PACKETS_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("SRC_LOST_RTP_PACKETS_X_Y");
  }

  if(sscanf(identifier, "SRC_LOST_RTP_FRAMES_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("SRC_LOST_RTP_FRAMES_X_Y");
  }



  if(sscanf(identifier, "DST_UDP_BYTES_%d", &evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("DST_UDP_BYTES_X");
  }

  if(sscanf(identifier, "DST_UDP_PACKETS_%d", &evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("DST_UDP_PACKETS_X");
  }

  if(sscanf(identifier, "DST_TCP_PACKETS_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("DST_TCP_PACKETS_X");
  }

  if(sscanf(identifier, "DST_TCP_BYTES_%d",&evaluator_container->port_num) == 1){
    return dmap_get_feature_by_identifier("DST_TCP_BYTES_X");
  }

  if(sscanf(identifier, "DST_RTP_PACKETS_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("DST_RTP_PACKETS_X_Y");
  }

  if(sscanf(identifier, "DST_RTP_BYTES_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("DST_RTP_BYTES_X_Y");
  }

  if(sscanf(identifier, "DST_LOST_RTP_PACKETS_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("DST_LOST_RTP_PACKETS_X_Y");
  }

  if(sscanf(identifier, "DST_LOST_RTP_FRAMES_%d_%d",
              &evaluator_container->port_num,
              &evaluator_container->payload_type) == 2){
    return dmap_get_feature_by_identifier("DST_LOST_RTP_FRAMES_X_Y");
  }
  
  
    if(strcmp("IP_PACKETS", identifier) == 0){
      return dmap_get_feature_by_identifier("IP_PACKETS");
    }

  return NULL;
}


void features_load()
{
  //Add all features the program have
  _add_features(

      make_feature_ip_packets(),
      make_feature_ip_packets_by_src_x_x_x_x(),
      make_feature_ip_bytes(),
      make_feature_tcp_packets(),
      make_feature_tcp_bytes(),
      make_feature_udp_packets(),
      make_feature_udp_bytes(),

      make_feature_rtp_packets(),
      make_feature_rtp_bytes(),
      make_feature_lost_rtp_packets(),
      make_feature_lost_rtp_frames(),

      make_feature_src_udp_packets(),
      make_feature_src_udp_bytes(),
      make_feature_src_tcp_packets(),
      make_feature_src_tcp_bytes(),
      make_feature_src_rtp_packets(),
      make_feature_src_rtp_bytes(),
      make_feature_src_lost_rtp_packets(),
      make_feature_src_lost_rtp_frames(),

      make_feature_dst_udp_packets(),
      make_feature_dst_udp_bytes(),
      make_feature_dst_tcp_packets(),
      make_feature_dst_tcp_bytes(),
      make_feature_dst_rtp_packets(),
      make_feature_dst_rtp_bytes(),
      make_feature_dst_lost_rtp_packets(),
      make_feature_dst_lost_rtp_frames(),

      NULL);

}


static groupcounter_prototype_t* _get_groupcounter_prototype_by_identifier(char_t *identifier)
{
  groupcounter_prototype_t *result;
  char_t prototype[255];

  result = dmap_get_groupcounter_prototype_by_identifier(identifier);
  if(result){
    return result;
  }

  return NULL;
}


void groupcounter_prototypes_load()
{
  //Add all features the program have
  _add_groupcounter_prototypes(

      make_groupcounter_prototype_src_udp_flows(),
      make_groupcounter_prototype_dst_udp_flows(),
      make_groupcounter_prototype_src_tcp_flows(),
      make_groupcounter_prototype_dst_tcp_flows(),

      NULL
      );

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

  dmap_get_sysdat()->accumulation_time =
      iniparser_getint(conf, "global:accumulation_time", 1000);

  for(i=0; i<dmap_get_sysdat()->pcap_listeners_num; ++i){
    pcap_listener_t* pcap_listener;
    char_t section[255];
    sprintf(section, "pcap_%d", i);
    pcap_listener = _listener_from_conf(conf, section);
    dmap_add_pcapls(pcap_listener);
  }
  INFOPRINT("Accumulative statistic structure: features,groupcounters,mapped_vars");
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
        if(0 < iniparser_getint(conf, conf_key, 0)){
          WARNINGPRINT("Accumulation length is a depricated and not considered any longer. Use accumulation_time at global section instead");
        }

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:puffer_size", section);
        result->puffer_size = iniparser_getint(conf, conf_key, 2000);

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

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:save_aggregation", section);
        result->save_aggregation = iniparser_getint(conf, conf_key, 0);

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:aggregation_path", section);
        read_str = iniparser_getstring(conf, conf_key, "aggregation.csv");
        strcpy(result->aggregation_path, read_str);

        if(result->save_aggregation){
            INFOPRINT("Save aggregated measurements at path: %s", result->aggregation_path);
        }

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:mapped_vars_num", section);
        result->mapped_vars_num = iniparser_getint(conf, conf_key, 0);

        for(i=0; i<NTRT_MAX_MAPPED_VARS; ++i){
          sprintf(conf_key, "%s:mapped_var_id_%d", section,i);
          result->mapped_var_ids[i] = iniparser_getint(conf, conf_key, 0);
          if(0 < result->mapped_var_ids[i]){
            _add_mapped_var(result->mapped_var_ids[i]);
          }
        }

        memset(conf_key, 0, strlen(conf_key));
        sprintf(conf_key, "%s:groupcounter_num", section);
        result->groupcounter_num = iniparser_getint(conf, conf_key, 0);

        for(i=0; i<result->groupcounter_num; ++i){
            groupcounter_prototype_t            *groupcounter_prototype;

            memset(conf_key, 0, strlen(conf_key));
            sprintf(conf_key, "%s:groupcounter_%d", section, i);
            read_str = iniparser_getstring(conf, conf_key, "\0");
            groupcounter_prototype = _get_groupcounter_prototype_by_identifier(read_str);
            if(!groupcounter_prototype){
              ERRORPRINT("Requested groupcounter %s not found for device %s", read_str, result->device);
              continue;
            }
            result->groupcounter_prototypes = slist_append(result->groupcounter_prototypes, groupcounter_prototype);
            INFOPRINT("Groupcounter %s is listening for device %s", read_str, result->device);
        }

	return result;
}

