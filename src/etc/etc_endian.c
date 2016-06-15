#include "etc_endian.h"
#include "lib_defs.h"
#include "inc_texts.h"
#include "lib_descs.h"
#include <limits.h>

endian_types_t get_system_endianness(void)
{
	uint32_t value;
	uint8_t *buffer = (uint8_t *)&value;

	buffer[0] = 0x00;
	buffer[1] = 0x01;
	buffer[2] = 0x02;
	buffer[3] = 0x03;

	switch (value)
	{
	case UINT32_C(0x00010203): return ENDIAN_BIG;
	case UINT32_C(0x03020100): return ENDIAN_LITTLE;
	case UINT32_C(0x02030001): return ENDIAN_BIG_WORD;
	case UINT32_C(0x01000302): return ENDIAN_LITTLE_WORD;
	default:                   return ENDIAN_UNKNOWN;
	}
}


bool_t set_bytes_for_sending(void *dst, void *src, int32_t size)
{
	byte_t *destination = (byte_t*) dst;
	byte_t *source = (byte_t*) src;
	int32_t i,j;
	endian_types_t endian = get_system_endianness();
	if(endian == ENDIAN_BIG_WORD || endian == ENDIAN_LITTLE_WORD || endian == ENDIAN_UNKNOWN)
	{
		ERRORPRINT("get_nbytes: not implemented endian convert");
		return BOOL_FALSE;
	}
	if(endian == ENDIAN_BIG){
		memcpy(dst, src, size);
		return BOOL_TRUE;
	}

	for (j = size - 1, i = 0; i < size; ++i, --j) {
		destination[j] = source[i];
	}

	return BOOL_TRUE;
}

bool_t set_bytes_for_receiving(void *dst, void *src, int32_t size)
{
	byte_t *destination = (byte_t*) dst;
	byte_t *source = (byte_t*) src;
	int32_t i,j;
	endian_types_t endian = get_system_endianness();
	if(endian == ENDIAN_BIG_WORD || endian == ENDIAN_LITTLE_WORD || endian == ENDIAN_UNKNOWN)
	{
		ERRORPRINT("get_nbytes: not implemented endian convert");
		return BOOL_FALSE;
	}
	if(endian == ENDIAN_BIG){
		memcpy(dst, src, size);
		return BOOL_TRUE;
	}
	//DEBUGPRINT("checkpoint : %d", size);
	for (j = 0, i = size - 1; i >= 0; --i, ++j) {
		destination[j] = source[i];
		//DEBUGPRINT("i: %d j: %d", i, j );
	}
	return BOOL_TRUE;
}

