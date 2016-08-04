#include "sys_confs.h"
#include "lib_tors.h"
#include "inc_unistd.h"
#include "lib_descs.h"
#include <string.h>
#include <dirent.h>
#include "lib_debuglog.h"
#include "dmap.h"
#include "lib_makers.h"
#include "etc_sniffex.h"
#include "lib_groupcounters.h"
#include "lib_funcs.h"

typedef struct flow_struct_t{
  uint16_t portnum;
  mtime_t  last_activity;
}flow_t;

static flow_t* _flow_ctor()
{
  flow_t* result;
  result = (flow_t*) malloc(sizeof(flow_t));
  memset(result, 0, sizeof(flow_t));
  return result;
}

static void _flow_dtor(ptr_t target)
{
  free(target);
}

//----------------------------------- FLOWCOUNT HELPERS ---------------------------

static slist_t* _flow_obsolator(slist_t* item, int32_t timeout_treshold, uint32_t *counter)
{
  slist_t* next;
  flow_t* flow;

  if(!item){
    return NULL;
  }
  flow = item->data;
  if(diffmtime_fromnow(&flow->last_activity) < timeout_treshold){
    item->next = _flow_obsolator(item->next, timeout_treshold, counter);
    return item;
  }

  _flow_dtor(flow);
  next = item->next;
  free(item);
  *counter -= 1;
  return _flow_obsolator(next, timeout_treshold, counter);
}

static int32_t _flows_obsolate_and_get(groupcounter_t* this)
{
  if(!this->flows){
    this->flowcounts = 0;
    goto done;
  }

  this->flows = _flow_obsolator(this->flows, this->timeout_treshold, &this->flowcounts);

done:
  return this->flowcounts;
}


static void _flows_add_or_refresh(groupcounter_t* this, uint16_t portnum)
{
  flow_t* flow;
  slist_t* it;

  if(!this->flows){
    goto create_and_append;
  }

  for(it = this->flows; it; it = it->next){
    flow = it->data;
    if(flow->portnum != portnum){
      continue;
    }
    set_mtime(&flow->last_activity);
    goto done;
  }

create_and_append:
  flow = _flow_ctor();
  flow->portnum = portnum;
  set_mtime(&flow->last_activity);
  this->flows = slist_append(this->flows, flow);
  ++this->flowcounts;
done:
  return;

}

static void _init_flowcounter(groupcounter_t* this, ptr_t data)
{

  this->flowcounts = 0;
  this->flows = NULL;

  dmap_lock_sysdat();
  this->timeout_treshold = dmap_get_sysdat()->accumulation_time;
  dmap_unlock_sysdat();

}

static void _deinit_flowcounter(groupcounter_t* this)
{
  slist_dtor(this->flows, _flow_dtor);
}

//-------------------------SRC_UDP_FLOWS ------------------------

static void _src_udp_flows_add_sniff(groupcounter_t* this, sniff_t* sniff)
{
  if(!sniff_is_udp_packet(sniff)){
    goto done;
  }
  _flows_add_or_refresh(this, ntohs(sniff->port_src));

done:
  return;

}

groupcounter_prototype_t *make_groupcounter_prototype_src_udp_flows()
{
  groupcounter_prototype_t* result;
  result = make_groupcounter_prototype(
      "SRC_UDP_FLOWS",
      _init_flowcounter,
      _src_udp_flows_add_sniff,
      _flows_obsolate_and_get,
      _deinit_flowcounter);

  return result;
}



//-------------------------DST_UDP_FLOWS ------------------------

static void _dst_udp_flows_add_sniff(groupcounter_t* this, sniff_t* sniff)
{
  if(!sniff_is_udp_packet(sniff)){
    goto done;
  }
  _flows_add_or_refresh(this, ntohs(sniff->port_dst));

done:
  return;

}

groupcounter_prototype_t *make_groupcounter_prototype_dst_udp_flows()
{
  groupcounter_prototype_t* result;
  result = make_groupcounter_prototype(
      "DST_UDP_FLOWS",
      _init_flowcounter,
      _dst_udp_flows_add_sniff,
      _flows_obsolate_and_get,
      _deinit_flowcounter);

  return result;
}



//-------------------------SRC_TCP_FLOWS ------------------------

static void _src_tcp_flows_add_sniff(groupcounter_t* this, sniff_t* sniff)
{
  if(!sniff_is_tcp_packet(sniff)){
    goto done;
  }
  _flows_add_or_refresh(this, ntohs(sniff->port_src));

done:
  return;

}

groupcounter_prototype_t *make_groupcounter_prototype_src_tcp_flows()
{
  groupcounter_prototype_t* result;
  result = make_groupcounter_prototype(
      "SRC_TCP_FLOWS",
      _init_flowcounter,
      _src_tcp_flows_add_sniff,
      _flows_obsolate_and_get,
      _deinit_flowcounter);

  return result;
}


//-------------------------DST_TCP_FLOWS ------------------------

static void _dst_tcp_flows_add_sniff(groupcounter_t* this, sniff_t* sniff)
{
  if(!sniff_is_tcp_packet(sniff)){
    goto done;
  }
  _flows_add_or_refresh(this, ntohs(sniff->port_dst));

done:
  return;

}

groupcounter_prototype_t *make_groupcounter_prototype_dst_tcp_flows()
{
  groupcounter_prototype_t* result;
  result = make_groupcounter_prototype(
      "DST_TCP_FLOWS",
      _init_flowcounter,
      _dst_tcp_flows_add_sniff,
      _flows_obsolate_and_get,
      _deinit_flowcounter);

  return result;
}


