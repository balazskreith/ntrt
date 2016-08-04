#ifndef INCGUARD_NTRT_LIBRARY_BINTREE_H_
#define INCGUARD_NTRT_LIBRARY_BINTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "lib_funcs.h"
#include "lib_makers.h"
#include "lib_descs.h"
#include "lib_puffers.h"

typedef struct _bintreenode{
  ptr_t   data;
  struct _bintreenode *left;
  struct _bintreenode *right;
  int32_t ref;
}bintreenode_t;

typedef int32_t (*bintreecmp)(ptr_t,ptr_t);
typedef void    (*bintreesprint)(ptr_t,char_t*);
typedef void    (*bintreedupnotifier)(ptr_t,ptr_t);

typedef struct _bintree {
  bintreenode_t     *root,*bottom,*top;
  datapuffer_t*      recycle;
  bintreecmp         cmp;
  bintreesprint      sprint;
  int32_t            node_counter;
  int32_t            duplicate_counter;
  bintreedupnotifier duplicate_notifier;
  ptr_t              duplicate_notifier_data;
} bintree_t;

int32_t bintreecmp_int32(ptr_t a,ptr_t b);

bintree_t *make_bintree(bintreecmp cmp);
void bintree_setsprint(bintree_t* this, bintreesprint sprint);
void bintree_test(void);
void bintree_dtor(ptr_t target);
void bintree_reset(bintree_t *this);
void bintree_setup_duplicate_notifier(bintree_t *this, bintreedupnotifier duplicate_notifier, ptr_t duplicate_notifier_data);
bintreenode_t *bintree_pop_top_node(bintree_t *this);
bintreenode_t *bintree_pop_bottom_node(bintree_t *this);
ptr_t bintree_pop_top_data(bintree_t *this);
ptr_t bintree_pop_bottom_data(bintree_t *this);
ptr_t bintree_get_top_data(bintree_t *this);
ptr_t bintree_get_bottom_data(bintree_t *this);
bool_t bintree_has_value(bintree_t *this, ptr_t data);
void bintree_insert_node(bintree_t* this, bintreenode_t* node);
bintreenode_t *make_bintreenode(bintree_t *this, ptr_t data);
void trash_bintreenode(bintree_t *this, bintreenode_t *node);
void bintree_insert_data(bintree_t* this, ptr_t data);
bintreenode_t* bintree_pop_node(bintree_t* this, ptr_t data);
void bintree_delete_value(bintree_t* this, ptr_t data);
void bintree_trash_node(bintree_t *this, bintreenode_t *node);
int32_t bintree_get_nodenum(bintree_t *this);
int32_t bintree_get_refnum(bintree_t *this);


#endif /* INCGUARD_NTRT_LIBRARY_VECTOR_H_ */
