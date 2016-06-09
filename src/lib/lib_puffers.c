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
	result->length = size;
	result->start = 0;
	result->end = 0;
	result->count = 0;
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
	index = 0;
	if(puffer == NULL){
		return;
	}
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
}//# datapuffer_dtor end

void datapuffer_write(datapuffer_t* puffer, void* item)
{
	puffer->items[puffer->end++] = item;
	++puffer->count;
	if(puffer->length <= puffer->end){
		puffer->end = 0;
	}
}//# datapuffer_write end

void* datapuffer_read(datapuffer_t* puffer)
{
	puffer->read = puffer->items[puffer->start];
	puffer->items[puffer->start] = NULL;
	if(puffer->length <= ++puffer->start){
		puffer->start = 0;
	}
	--puffer->count;
	return puffer->read;
}//# datapuffer_read end

int32_t datapuffer_readcapacity(datapuffer_t *datapuffer)
{
	return datapuffer->count;
}

int32_t datapuffer_writecapacity(datapuffer_t *datapuffer)
{
	return datapuffer->length - datapuffer->count;
}

bool_t datapuffer_isfull(datapuffer_t *datapuffer)
{
	return datapuffer->count == datapuffer->length ? BOOL_TRUE : BOOL_FALSE;
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
