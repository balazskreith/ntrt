/*
 * inc_mpttime.h
 *
 *  Created on: Mar 9, 2014
 *      Author: balazs
 */

#ifndef INC_MPTTIME_H_
#define INC_MPTTIME_H_
/*
typedef struct mpttime_struct_t
{
	uint32_t    source_path[4];                 //<-32
	uint32_t    epoch                     : 32; //<-160
	uint32_t    ms                        : 10; //<-192
	uint32_t    us                  	  : 10;
	uint32_t    west_from_greenwich 	  : 1;
	uint32_t    timezone_from_greenwich   : 4;
	uint32_t    not_used            	  : 7; //<-for byte boundary
	uint32_t    sequence_num              : 16;//<- 1224
	uint32_t    length                    : 16;
	uint32_t    last;                          //<- 256 -> 32byte

}mpttime_t;

void setmpttime(mpttime_t *);
*/

#endif /* INC_MPTTIME_H_ */
