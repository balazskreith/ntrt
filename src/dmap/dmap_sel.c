#include "dmap_sel.h"
#include "dmap.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "inc_texts.h"
#include "lib_debuglog.h"

/*
void dmap_rem_pths_byconid(int32_t con_dmap_id)
{
        int32_t    index;
        path_t    *path = NULL;
        for(index = 0; dmap_itr_table_pth(&index, &path) == BOOL_TRUE; ++index){
                if(path->con_dmap_id != con_dmap_id){
                        continue;
                }
                dmap_rem_pth_byindex(index);
        }
}
*/


feature_t* dmap_get_feature_by_identifier(char_t* identifier)
{
    int32_t    index;
    feature_t* result = NULL;
    for(index = 0; dmap_itr_table_features(&index, &result) == BOOL_TRUE; ++index){
        if(strcmp(identifier, result->identifier) != 0){
          continue;
        }
        break;
    }

    return result;
}


int32_t dmap_get_id_by_pcapls(pcap_listener_t* pcap_listener)
{
  int32_t result,index;
  pcap_listener_t* it = NULL;
  for(index = 0; dmap_itr_table_pcapls(&index, &it) == BOOL_TRUE; ++index){
      if(it == pcap_listener){
        return index;
      }
  }
  return DEVCLEGO_DMAP_ERROR_ITEM_NOT_FOUND;
}
