/**
 * @file lib_puffers.c
 * @brief puffers used in the program
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#include "lib_puffers.h"
#include <stdlib.h>
#include "lib_defs.h"
#include "lib_descs.h"
#include <stdarg.h>


mdatapuffer_t* mdatapuffer_ctor(int32_t items_num)
{
	mdatapuffer_t* result;
	int32_t index;
	result = (mdatapuffer_t*) malloc(sizeof(mdatapuffer_t));

	result->items = (void**) malloc(sizeof(void*) * items_num);
	result->length = items_num;
	result->count = 0;
	result->read_index = 0;
	result->end = 0;
	result->start = 0;
	for(index = 0; index < result->length; ++index)
	{
		result->items[index] = NULL;
	}

	return result;
}

void mdatapuffer_dtor(mdatapuffer_t *mdatapuffer)
{
	int32_t index;
	void* item;
	index = 0;
	if(mdatapuffer == NULL){
		//already destroyed
		return;
	}
	for(index = 0; index <  mdatapuffer->length; ++index)
	{
		item = mdatapuffer->items[index];
		if(item == NULL)
		{
			continue;
		}
		free(item);
	}
	free(mdatapuffer->items);
	free(mdatapuffer);
	//spin_unlock(spin);
	//spin_dtor(spin);
}

void mdatapuffer_setreadindex(mdatapuffer_t *puffer, int32_t plus)
{
	int32_t not_null;
	puffer->_plus = plus;
	if(puffer->count == 0){
		return;
	}

	//set the first readable item
	while(puffer->items[puffer->start] == NULL){
		if(++puffer->start == puffer->length){
			puffer->start = 0;
		}
	}

	if(puffer->count - puffer->_plus < 1){
		return;
	}
	puffer->read_index = puffer->start;

	for(not_null = 0; not_null < puffer->_plus; ++not_null){
		do{
			if(++puffer->read_index == puffer->length){
				puffer->read_index = 0;
			}
			if(puffer->read_index == puffer->end){
				//puffer->read_index = -1;
				return;
			}
		}while(puffer->items[puffer->read_index] == NULL);
	}
}

void* mdatapuffer_read(mdatapuffer_t *puffer)
{
	do{
		if(puffer->read_index == puffer->length){
			puffer->read_index = 0;
		}
		puffer->_read = puffer->items[puffer->read_index];
		if(puffer->_read != NULL){
			puffer->items[puffer->read_index++] = NULL;
			break;
		}
		++puffer->read_index;
	}while(puffer->read_index != puffer->end);
	--puffer->count;
	return puffer->_read;
}

void mdatapuffer_write(mdatapuffer_t *mdatapuffer, void *item)
{
	mdatapuffer->items[mdatapuffer->end++] = item;
	if(mdatapuffer->end == mdatapuffer->length){
		mdatapuffer->end = 0;
	}
	++mdatapuffer->count;
}

int32_t mdatapuffer_readcapacity(mdatapuffer_t *puffer)
{
	return puffer->count - puffer->_plus;
}

int32_t mdatapuffer_writecapacity(mdatapuffer_t *puffer)
{
	if(puffer->count == 0){
		return puffer->length;
	}
	if(puffer->start == puffer->end){
		return 0;
	}
	return puffer->start < puffer->end
				? puffer->length - puffer->end + puffer->start + 1
				: puffer->start - puffer->end;

	return puffer->start < puffer->end
			? puffer->length - puffer->end  + puffer->start - 1
	//		? puffer->length - (puffer->end + 1 - puffer->start)
			: puffer->length - puffer->start + puffer->end - 1;
	//		: (puffer->start - puffer->end) - 1;
}

void mdatapuffer_reset(mdatapuffer_t *puffer)
{
	int32_t index;
	mdatapuffer_clear(puffer, NULL);
	puffer->count = 0;
	puffer->read_index = 0;
	puffer->end = 0;
	puffer->start = 0;
	for(index = 0; index < puffer->length; ++index)
	{
		puffer->items[index] = NULL;
	}
}


void mdatapuffer_clear(mdatapuffer_t *puffer, void (*dtor)(void*))
{
	void *item;
	int32_t c,i;
	mdatapuffer_setreadindex(puffer, 0);
	c = mdatapuffer_readcapacity(puffer);
	for(i = 0; i < c; ++i){
		item = mdatapuffer_read(puffer);
		if(dtor == NULL){
			continue;
		}
		dtor(item);
	}
	puffer->read_index = puffer->start;
}


datapuffer_t* datapuffer_ctor(int32_t size)
{
	datapuffer_t* result;
	int32_t index;
	result = (datapuffer_t*) malloc(sizeof(datapuffer_t));
	result->items = (void**) malloc(sizeof(void*) * size);
	result->abs_length = size;
	result->start = 0;
	result->end = 0;
	result->count = 0;
	for(index = 0; index < result->abs_length; index++)
	{
		result->items[index] = NULL;
	}
	return result;
}//# datapuffer_ctor end


void datapuffer_dtor(datapuffer_t* puffer)
{
	int32_t index;
	void* item;
	index = 0;
	if(puffer == NULL){
		return;
	}
	for(index = 0; index <  puffer->abs_length; index++)
	{
		item = puffer->items[index];
		if(item == NULL)
		{
			continue;
		}
		free(item);
	}
	free(puffer->items);
	free(puffer);
}//# datapuffer_dtor end

void datapuffer_write(datapuffer_t* puffer, void* item)
{
	puffer->items[puffer->end++] = item;
	++puffer->count;
	if(puffer->abs_length <= puffer->end){
		puffer->end = 0;
	}
}//# datapuffer_write end

void* datapuffer_read(datapuffer_t* puffer)
{
        puffer->read = puffer->items[puffer->start];
        puffer->items[puffer->start] = NULL;
        if(puffer->abs_length <= ++puffer->start){
                puffer->start = 0;
        }
        --puffer->count;
        return puffer->read;
}//# datapuffer_read end

void* datapuffer_peek_first(datapuffer_t* puffer)
{
        return puffer->items[puffer->start];
}//# datapuffer_read end

int32_t datapuffer_readcapacity(datapuffer_t *datapuffer)
{
	return datapuffer->count;
}

int32_t datapuffer_writecapacity(datapuffer_t *datapuffer)
{
	return datapuffer->abs_length - datapuffer->count;
}

bool_t datapuffer_isfull(datapuffer_t *datapuffer)
{
	return datapuffer->count == datapuffer->abs_length ? BOOL_TRUE : BOOL_FALSE;
}

bool_t datapuffer_isempty(datapuffer_t *datapuffer)
{
	return datapuffer->count == 0 ? BOOL_TRUE : BOOL_FALSE;
}

void datapuffer_clear(datapuffer_t *puffer, void (*dtor)(void*))
{
	int32_t i,c;
	void *item;
	for(i = 0, c = datapuffer_readcapacity(puffer); i < c; ++i){
		item = datapuffer_read(puffer);
		if(dtor == NULL){
			continue;
		}
		dtor(item);
	}
}
/*
datapuffer_t* datapuffer_ctor(int32_t size)
{
	datapuffer_t* result;
	int32_t index;
	result = (datapuffer_t*) malloc(sizeof(datapuffer_t));
	//result->spin = spin_ctor();

	result->items = (void**) malloc(sizeof(void*) * size);
	result->length = size;
	result->read_index = 0;
	result->write_index = 0;
	result->is_empty = BOOL_TRUE;
	result->is_full = BOOL_FALSE;
	result->items = (void**) malloc(sizeof(void*) * result->length);
	for(index = 0; index < result->length; index++)
	{
		result->items[index] = NULL;
	}
	return result;
}//# datapuffer_ctor end


void datapuffer_dtor(datapuffer_t* puffer)
{
	int32_t index;
	void* item;
	//spin_t* spin;
	index = 0;
	if(puffer == NULL){
		//already destroyed
		return;
	}
	//spin = puffer->spin;
	//spin_lock(spin);
	for(index = 0; index <  puffer->length; index++)
	{
		item = puffer->items[index];
		if(item == NULL)
		{
			continue;
		}
		free(item);
	}
	free(puffer->items);
	free(puffer);
	//spin_unlock(spin);
	//spin_dtor(spin);
}//# datapuffer_dtor end

void datapuffer_write(datapuffer_t* puffer, void* item)
{
	int32_t write_index;
	if(puffer->is_full == BOOL_TRUE){
		return;
	}
	//spin_lock(puffer->spin);
	write_index = puffer->write_index;
	puffer->items[write_index] = item;
	write_index = puffer->write_index + 1;
	if(puffer->length <= write_index)
	{
		write_index = 0;
	}
	puffer->write_index = write_index;
	puffer->is_full = write_index == puffer->read_index ? BOOL_TRUE : BOOL_FALSE;
	puffer->is_empty = BOOL_FALSE;
	//spin_unlock(puffer->spin);
}//# datapuffer_write end

void* datapuffer_read(datapuffer_t* puffer)
{
	int32_t read_index;
	void* result = NULL;
	if(puffer->is_empty == BOOL_TRUE){
		return result;
	}
	
	//spin_lock(puffer->spin);
	read_index = puffer->read_index;
	result = puffer->items[read_index];
	puffer->items[read_index] = NULL;
	read_index++;
	if(puffer->length <= read_index)
	{
		read_index = 0;
	}
	puffer->read_index = read_index;
	puffer->is_empty = read_index == puffer->write_index ? BOOL_TRUE : BOOL_FALSE;
	puffer->is_full = BOOL_FALSE;
	//spin_unlock(puffer->spin);
	return result;
}//# datapuffer_read end


*/





slidingwindow_t* slidingwindow_ctor(int32_t num_limit, double time_limit, swstorage_t* (*storage_maker)(int32_t))
{
  slidingwindow_t* result;
  result = malloc(sizeof(slidingwindow_t));
  memset(result, 0, sizeof(slidingwindow_t));
  if(!num_limit){
    WARNINGPRINT("Num limit can not be zero");
    num_limit = 32;
  }
  result->trackeditems  = datapuffer_ctor( num_limit );
  result->num_limit     = num_limit;
  result->time_limit    = time_limit;
  result->recycle       = datapuffer_ctor( MIN(1024,num_limit) );
  result->storage       = storage_maker ? storage_maker(num_limit) : NULL;
  return result;
}

void slidingwindow_dtor(ptr_t target)
{
  slidingwindow_t* this;
  slist_t *it;

  if(!target){
    return;
  }
  this = target;
  datapuffer_clear(this->trackeditems, NULL);
  datapuffer_dtor(this->trackeditems);
  datapuffer_clear(this->recycle, free);
  datapuffer_dtor(this->recycle);
  if(this->storage){
    swstorage_disposer(this->storage);
  }
  slist_dtor(this->plugins, swplugin_dtor);
  free(this);
}

static void _slidingwindow_rem(slidingwindow_t* this)
{
  slidingwindowitem_t *item;
  slist_t* it;
  item = datapuffer_read(this->trackeditems);

  for(it = this->plugins; it; it = it->next){
      swplugin_t *swplugin;
      swplugin = it->data;
      if(swplugin->rem_pipe){
        swplugin->rem_pipe(swplugin->rem_data, item->data);
      }
  }

  item->data = NULL;

  if(datapuffer_isfull(this->recycle) == BOOL_TRUE){
    free(item);
  }else{
    datapuffer_write(this->recycle, item);
  }
}

void slidingwindow_clear(slidingwindow_t* this)
{
  while(datapuffer_isempty(this->trackeditems) == BOOL_FALSE){
      _slidingwindow_rem(this);
  }
}

static void _slidingwindow_obsolate_num_limit(slidingwindow_t* this)
{
again:
  if(datapuffer_isfull(this->trackeditems) == BOOL_FALSE){
    return;
  }
  _slidingwindow_rem(this);
  goto again;
}

static void _slidingwindow_obsolate_time_limit(slidingwindow_t* this)
{
  slidingwindowitem_t *item;
  if(this->time_limit == 0.){
    return;
  }
again:
  item = datapuffer_peek_first(this->trackeditems);
  if(!item || diffmtime_fromnow(&item->pushed) < this->time_limit){
    return;
  }
  _slidingwindow_rem(this);
  goto again;
}

void slidingwindow_refresh(slidingwindow_t *this)
{
  _slidingwindow_obsolate_time_limit(this);
  _slidingwindow_obsolate_num_limit(this);
}

void slidingwindow_add_data(slidingwindow_t* this, ptr_t data)
{
  slidingwindowitem_t *item;
  slist_t* it;
  slidingwindow_refresh(this);
  if(datapuffer_isempty(this->recycle)){
    item = malloc(sizeof(slidingwindowitem_t));
  }else{
    item = datapuffer_read(this->recycle);
  }
  memset(item, 0, sizeof(slidingwindowitem_t));

  if(this->storage){
    data = this->storage->store(this->storage, data);
  }

  item->data = data;
  set_mtime(&item->pushed);

  datapuffer_write(this->trackeditems, item);

  for(it = this->plugins; it; it = it->next){
      swplugin_t *swplugin;
      swplugin = it->data;
      if(swplugin->add_pipe){
        swplugin->add_pipe(swplugin->add_data, data);
      }
  }
}

void slidingwindow_add_plugin(slidingwindow_t* this, swplugin_t *swplugin)
{
  this->plugins = slist_append(this->plugins, swplugin);
}


void slidingwindow_add_plugins (slidingwindow_t* this, ... )
{
    va_list arguments;
    swplugin_t* swplugin = NULL;
    va_start ( arguments, this );
    for(swplugin = va_arg( arguments, swplugin_t*); swplugin; swplugin = va_arg(arguments, swplugin_t*)){
        slidingwindow_add_plugin(this, swplugin);
    }
    va_end ( arguments );
}


void slidingwindow_add_pipes(slidingwindow_t* this, void (*rem_pipe)(ptr_t,ptr_t),ptr_t rem_data, void (*add_pipe)(ptr_t,ptr_t),ptr_t add_data)
{
  swplugin_t *swplugin;
  swplugin = malloc(sizeof(swplugin_t));
  memset(swplugin, 0, sizeof(swplugin_t));
  swplugin->add_pipe = add_pipe;
  swplugin->add_data = add_data;
  swplugin->rem_pipe = rem_pipe;
  swplugin->rem_data = rem_data;
  swplugin->disposer = free;
  swplugin->priv     = NULL;
  slidingwindow_add_plugin(this, swplugin);
}

bool_t slidingwindow_is_empty(slidingwindow_t* this)
{
  return datapuffer_isempty(this->trackeditems);
}

#define SWSTORAGE_STOREFNC(name,type) \
static ptr_t name(swstorage_t *this, ptr_t src) \
{    \
  type *dst = this->storage; \
  dst += this->index;        \
  memcpy(dst, src, sizeof(type)); \
  if(++this->index == this->length){ \
    this->index = 0; \
  } \
  return dst; \
} \

#define SWSTORAGE_MAKER(name,storefnc,type) \
swstorage_t* make_swstorage_int32(int32_t num_limit) \
{ \
  swstorage_t *this; \
  this = malloc(sizeof(swstorage_t)); \
  memset(this, 0, sizeof(swstorage_t)); \
  this->length = num_limit + 1; \
  this->storage = malloc(sizeof(type) * this->length); \
  this->store = storefnc; \
  return this; \
} \

#define SLIDINGWINDOW_TYPE_ADDER(name, type) \
void name(slidingwindow_t* this, type item) \
{ \
  slidingwindow_add_data(this, &item); \
} \

SWSTORAGE_STOREFNC(_swstorage_store_int32,int32_t)
SWSTORAGE_MAKER(make_swstorage_int32, _swstorage_store_int32, int32_t)
SLIDINGWINDOW_TYPE_ADDER(slidingwindow_add_int, int32_t)

void swstorage_disposer(ptr_t target)
{
  swstorage_t *swstorage;
  swstorage = target;
  free(swstorage->store);
  free(swstorage);
}

swplugin_t* swplugin_ctor()
{
  swplugin_t* this;
  this = malloc(sizeof(swplugin_t));
  memset(this, 0, sizeof(swplugin_t));
  return this;
}

void swplugin_dtor(ptr_t target)
{
  swplugin_t* swplugin;
  if(!target){
    return;
  }
  swplugin = target;
  swplugin->disposer(swplugin);
}

