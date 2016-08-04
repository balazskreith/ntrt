#ifndef INCGUARD_NTRT_LIBRARY_SWPLUGINS_H_
#define INCGUARD_NTRT_LIBRARY_SWPLUGINS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "lib_puffers.h"
#include "lib_bintree.h"

void slidingwindow_test(void);

void swprinter_int32(ptr_t data, char_t* string);
swplugin_t* make_swprinter(void (*sprint)(ptr_t,char_t*));

typedef struct swminmaxdat_struct_t{
  ptr_t min;
  ptr_t max;
}swminmaxstat_t;

typedef struct swpercentilecandidates_struct_t{
  bool_t  processed;
  ptr_t   left;
  ptr_t   right;
}swpercentilecandidates_t;


swplugin_t* make_swminmax(bintreecmp cmp,
                          void (*minmax_pipe)(ptr_t,swminmaxstat_t*),
                          ptr_t minmax_data
                          );


swplugin_t* make_swpercentile(
                              int32_t     percentile,
                              bintreecmp  cmp,
                              void      (*percentile_pipe)(ptr_t,swpercentilecandidates_t*),
                              ptr_t       percentile_data
                              );

swplugin_t* make_swint32_stater(void (*pipe)(ptr_t,int32_t),ptr_t pipe_data);

#endif /* INCGUARD_NTRT_LIBRARY_SWPLUGINS_H_ */
