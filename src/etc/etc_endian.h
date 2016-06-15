/*
 * etc_endian.h
 *
 *  Created on: Apr 24, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_ETC_ENDIAN_H_
#define INCGUARD_ETC_ENDIAN_H_

#include "dmap_defs.h"
#include "lib_descs.h"

typedef enum {
  ENDIAN_UNKNOWN,
  ENDIAN_BIG,
  ENDIAN_LITTLE,
  ENDIAN_BIG_WORD,   /* Middle-endian, Honeywell 316 style */
  ENDIAN_LITTLE_WORD /* Middle-endian, PDP-11 style */
}endian_types_t;

endian_types_t get_system_endianness(void);
bool_t set_bytes_for_receiving(void *dst, void *src, int32_t size);
bool_t set_bytes_for_sending(void *dst, void *src, int32_t size);

#endif /* INCGUARD_ETC_ENDIAN_H_ */
