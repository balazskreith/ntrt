#include "lib_bintree.h"
#include "lib_puffers.h"
#include "inc_mtime.h"
#include <math.h>
#include <string.h>

static bintreenode_t * _insert(bintree_t *this, bintreenode_t *actual, bintreenode_t *insert);
static bintreenode_t * _insert_into_tree(bintree_t *this, bintreenode_t *actual, bintreenode_t **insert);
static bintreenode_t *
_search_value(bintree_t *this, ptr_t value, bintreenode_t **parent);
static void
_deref_from_tree (bintree_t * this, ptr_t value);
static void _refresh_top(bintree_t *this);
static void _refresh_bottom(bintree_t *this);
static bintreenode_t *
_pop_from_tree (bintree_t * this, ptr_t value);
static bintreenode_t *_get_rightest_value(bintreenode_t *node, bintreenode_t **parent);
static bintreenode_t *_get_leftest_value(bintreenode_t *node, bintreenode_t **parent);
static bintreenode_t *_make_bintreenode(bintree_t *this, ptr_t value);
static void _trash_bintreenode(bintree_t *this, bintreenode_t *node);
static void _ruin_full(bintree_t *this, bintreenode_t *node);


void static
_print_tree (bintree_t * tree, bintreenode_t* node, int32_t level)
{
  int32_t i;
  char_t string[255];

  if (!node) {
    return;
  }
  memset(string, 0, 255);
  if(tree->top){
      tree->sprint(tree->top->data, string);
  }

  if (!level)
    sysio->print_stdlog ("Tree %p TOP is: %s Counter: %u\n", tree, string, tree->node_counter);
  for (i = 0; i < level && i < 10; ++i)
    sysio->print_stdlog ("--");

  tree->sprint(node->data, string);
  sysio->print_stdlog ("%d->%p->data:%s ->ref: %u ->left:%p ->right:%p\n",
      level, node, string, node->ref, node->left, node->right);
  _print_tree (tree, node->left, level + 1);
  _print_tree (tree, node->right, level + 1);
}


//----------------------------------------------------------------------
//--------- Private functions implementations to SchTree object --------
//----------------------------------------------------------------------


int32_t bintreecmp_int32(ptr_t pa,ptr_t pb)
{
  int32_t a,b;
  a = *((int32_t*)pa);
  b = *((int32_t*)pb);
  return a == b ? 0 : a < b ? -1 : 1;
}



static void _default_sprint(ptr_t node, char_t* string)
{
  strcpy(string, "Unknown");
}

bintree_t *make_bintree(bintreecmp cmp)
{
  bintree_t *result;
  result = malloc (sizeof(bintree_t));
  memset(result, 0, sizeof(bintree_t));
  result->cmp = cmp;
  result->sprint = _default_sprint;
  result->recycle = datapuffer_ctor(32);
  return result;
}

void bintree_setsprint(bintree_t* this, bintreesprint sprint)
{
  this->sprint = sprint;
}

static void test_sprint(ptr_t data, char_t *string)
{
  sprintf(string, "%d", *((int32_t*)data));
}

void bintree_test(void)
{
  bintree_t *tree1;
  bintree_t *tree2;
  bintreenode_t *node;
  int32_t   i;
  int32_t   tree1_values[] = {5,7,7,6,8,8,2,2,3,1};
  int32_t   tree2_values[] = {5,7,7,2,3,1};
  tree1 = make_bintree(bintreecmp_int32);
  tree2 = make_bintree(bintreecmp_int32);

  bintree_setsprint(tree1, test_sprint);
  bintree_setsprint(tree2, test_sprint);

  printf("POP CASE. Insert 5,7,7,6,8,8,2,2,3,1 into tree1\n");
  for(i = 0; i < 10; ++i){
    bintree_insert_data(tree1, &tree1_values[i]);
  }

  printf("POP CASE. Insert 5,7,7,2,3,1 into tree2\n");
  for(i = 0; i < 6; ++i){
    bintree_insert_data(tree2, &tree2_values[i]);
  }

  printf("Tree1:\n");
  _print_tree(tree1, tree1->root, 0);
  printf("Tree2:\n");
  _print_tree(tree2, tree2->root, 0);

  printf("POP TOP FROM tree1 and insert to tree2\n");
  node = bintree_pop_top_node(tree1);
  bintree_insert_node(tree2, node);
  printf("Tree1:\n");
  _print_tree(tree1, tree1->root, 0);
  printf("Tree2:\n");
  _print_tree(tree2, tree2->root, 0);

  //terminate the program
  bintree_reset(tree1);
  tree1->root->data = NULL; //segfault
}

void bintree_dtor(ptr_t target)
{
  bintree_t* this;
  if(!target){
    return;
  }
  this = target;
  bintree_reset(this);
  free(this);
}

void bintree_reset(bintree_t *this)
{
  _ruin_full(this, this->root);
  this->node_counter = 0;
  datapuffer_clear(this->recycle, free);
  this->root = NULL;
}

void bintree_setup_duplicate_notifier(bintree_t *this, bintreedupnotifier duplicate_notifier, ptr_t duplicate_notifier_data)
{
  this->duplicate_notifier = duplicate_notifier;
  this->duplicate_notifier_data = duplicate_notifier_data;
}

void _ruin_full(bintree_t *this, bintreenode_t *node)
{
  if(!node) return;
  _ruin_full(this, node->left);
  _ruin_full(this, node->right);
  _trash_bintreenode(this, node);
  --this->node_counter;
}

bintreenode_t *bintree_pop_top_node(bintree_t *this)
{
  bintreenode_t *result = NULL;
  if(!this->top) result = NULL;
  else result = _pop_from_tree(this, this->top->data);
  return result;
}

bintreenode_t *bintree_pop_bottom_node(bintree_t *this)
{
  bintreenode_t *result = NULL;
  if(!this->bottom) result = NULL;
  else result = _pop_from_tree(this, this->bottom->data);
  return result;
}

ptr_t bintree_pop_top_data(bintree_t *this)
{
  ptr_t result = NULL;
  if(!this->top){
    return NULL;
  }
  result = this->top->data;
  _deref_from_tree(this, this->top->data);
  return result;
}

ptr_t bintree_pop_bottom_data(bintree_t *this)
{
  ptr_t result = NULL;
  if(!this->bottom){
    return NULL;
  }
  result = this->bottom->data;
  _deref_from_tree(this, this->bottom->data);
  return result;
}

ptr_t bintree_get_top_data(bintree_t *this)
{
  if(!this->top) return NULL;
  else return this->top->data;
}

ptr_t bintree_get_bottom_data(bintree_t *this)
{
  if(!this->bottom)
    return NULL;
  return this->bottom->data;
}

bool_t bintree_has_value(bintree_t *this, ptr_t data)
{
  bintreenode_t *node,*parent;
  node = _search_value(this, data, &parent);
  return node != NULL ? BOOL_TRUE : BOOL_FALSE;
}


void bintree_insert_node(bintree_t* this, bintreenode_t* node)
{
  this->root = _insert(this, this->root, node);
}

void bintree_insert_data(bintree_t* this, ptr_t data)
{
  bintreenode_t* node;
  node = _make_bintreenode(this, data);
  node->ref = 1;
  this->root = _insert(this, this->root, node);
}

void bintree_delete_value(bintree_t* this, ptr_t data)
{
  _deref_from_tree(this, data);
}

void bintree_trash_node(bintree_t *this, bintreenode_t *node)
{
  _trash_bintreenode(this, node);
}

int32_t bintree_get_nodenum(bintree_t *this)
{
  return this->node_counter;
}

int32_t bintree_get_refnum(bintree_t *this)
{
  return this->node_counter + this->duplicate_counter;
}

bintreenode_t* bintree_pop_node(bintree_t* this, ptr_t data)
{
  bintreenode_t* result;
  result = _pop_from_tree(this, data);
  return result;
}

bintreenode_t *make_bintreenode(bintree_t *this, ptr_t data)
{
  bintreenode_t *result;
  result = _make_bintreenode(this, data);
  return result;
}

void trash_bintreenode(bintree_t *this, bintreenode_t *node)
{
//  THIS_WRITELOCK (this);
  _trash_bintreenode(this, node);
//  THIS_WRITEUNLOCK (this);
}

bintreenode_t * _insert(bintree_t *this, bintreenode_t *actual, bintreenode_t *insert)
{
  bintreenode_t *result;
  result = _insert_into_tree(this, actual, &insert);
  if(this->node_counter == 1) {
    this->top = this->bottom = insert;
  } else {
    bintreenode_t *top,*bottom;
    top = this->top;
    bottom = this->bottom;
    if(this->cmp(top->data, insert->data) < 0) {
      this->top = insert;
    } else if(this->cmp(insert->data, bottom->data) < 0) {
      this->bottom = insert;
    }
  }
  return result;
}

bintreenode_t * _insert_into_tree(bintree_t *this, bintreenode_t *actual, bintreenode_t **insert)
{
  int32_t cmp_result;
  if (!actual) {++this->node_counter; return *insert;}
  cmp_result = this->cmp (actual->data, (*insert)->data);
  if (!cmp_result) {
    //char_t string[255];
    //this->sprint((*insert)->data, string);
    //DEBUGPRINT("DUPLICATED: %s, %p will be merged and dropped", string, *insert);
    actual->ref+=(*insert)->ref;
    if(this->duplicate_notifier){
      this->duplicate_notifier(this->duplicate_notifier_data, actual->data);
    }
    actual->data = (*insert)->data;
    _trash_bintreenode(this, *insert);
    *insert = actual;
    ++this->duplicate_counter;
  } else if (cmp_result < 0)
    actual->right = _insert_into_tree (this, actual->right, insert);
  else
    actual->left = _insert_into_tree (this, actual->left, insert);
  return actual;
}


void
_deref_from_tree (bintree_t * this, ptr_t data)
{
  bintreenode_t *candidate = NULL, *candidate_parent = NULL;
  bintreenode_t *node = NULL, *parent = NULL;

  node = _search_value(this, data, &parent);
  if(!node)
    goto not_found;
  if(node->ref > 1)
    goto survive;
  if (!node->left && !node->right) {
    candidate = NULL;
    goto remove;
  } else if (!node->left) {
    candidate = node->right;
    goto remove;
  } else if (!node->right) {
    candidate = node->left;
    goto remove;
  } else {
    candidate_parent = node;
    candidate = _get_rightest_value (node->left, &candidate_parent);
    goto replace;
  }

not_found:
  return;
survive:
  --node->ref;
  --this->duplicate_counter;
  return;
remove:
  if (!parent)
    this->root = candidate;
  else if (parent->left == node)
    parent->left = candidate;
  else
    parent->right = candidate;
  _trash_bintreenode(this, node);
  --this->node_counter;
  return;
replace:
  candidate = _pop_from_tree(this, candidate->data);
  {
      //copy values
      node->data = candidate->data;
      node->ref = candidate->ref;
  }
  _trash_bintreenode(this, candidate);
  return;
}



bintreenode_t *
_pop_from_tree (bintree_t * this, ptr_t data)
{
  bintreenode_t *candidate = NULL, *candidate_parent = NULL;
  bintreenode_t *node, *parent;
  node = _search_value(this, data, &parent);
  if(!node)
    goto not_found;
  else if (!node->left && !node->right) {
    candidate = NULL;
    goto remove;
  } else if (!node->left) {
    candidate = node->right;
    goto remove;
  } else if (!node->right) {
    candidate = node->left;
    goto remove;
  } else {
    candidate_parent = node;
    candidate = _get_rightest_value (node->left, &candidate_parent);
    goto replace;
  }
not_found:
//  DEBUGPRINT ("ELEMENT NOT FOUND TO REMOVE: %lu\n", node->value);
  return NULL;
remove:
//  DEBUGPRINT ("ELEMENT FOUND TO REMOVE: %lu\n", node->value);
  if (!parent)
    this->root = candidate;
  else if (parent->left == node)
    parent->left = candidate;
  else
    parent->right = candidate;
  --this->node_counter;
  this->duplicate_counter-=node->ref-1;
  node->left = node->right = NULL;
  if(node == this->top) _refresh_top(this);
  if(node == this->bottom) _refresh_bottom(this);
  return node;
replace:
  //ELEMENT FOUND TO REPLACE
  candidate = _pop_from_tree (this, candidate->data);
  {
      int32_t ref;
      ptr_t data;

      //copy values
      ref = node->ref;
      data = node->data;
      node->data = candidate->data;
      node->ref = candidate->ref;
      candidate->ref = ref;
      candidate->data = data;
      candidate->left = candidate->right = NULL;
  }
  if(candidate == this->top) _refresh_top(this);
  if(candidate == this->bottom) _refresh_bottom(this);
  return candidate;
}


bintreenode_t *_get_rightest_value(bintreenode_t *node, bintreenode_t **parent)
{
  if(!node) return NULL;
  while(node->right) {*parent = node; node = node->right;}
  return node;
}

bintreenode_t *_get_leftest_value(bintreenode_t *node, bintreenode_t **parent)
{
  if(!node) return NULL;
  while(node->left) {*parent = node; node = node->left;}
  return node;
}

bintreenode_t *_search_value(bintree_t *this, ptr_t data, bintreenode_t **parent)
{
  bintreenode_t *node = this->root;
  int32_t cmp;
  *parent = NULL;
  while(node){
    cmp = this->cmp(data, node->data);
    if(!cmp) break;
    *parent = node;
    node =  cmp < 0 ? node->left : node->right;
  }
  return node;
}

bintreenode_t *_make_bintreenode(bintree_t *this, ptr_t data)
{
  bintreenode_t *result;
  if(datapuffer_isempty(this->recycle)){
    result = malloc(sizeof(bintreenode_t));
  }else{
    result = datapuffer_read(this->recycle);
  }
  memset(result, 0, sizeof(bintreenode_t));
  result->data = data;
  result->ref = 1;
  return result;
}

void _trash_bintreenode(bintree_t *this, bintreenode_t *node)
{
  if(!node){
    DEBUGPRINT("No node to trash");
    return;
  }
  if(node == this->top){
    _refresh_top(this);
  }

  if(node == this->bottom){
    _refresh_bottom(this);
  }

  if(datapuffer_isfull(this->recycle)){
    free(node);
  }else{
    datapuffer_write(this->recycle, node);
  }
}

void _refresh_top(bintree_t *this)
{
  bintreenode_t *top, *parent = NULL;
  top = this->top;
  if(top->left) this->top = _get_rightest_value(top->left, &parent);
  else this->top = _get_rightest_value(this->root, &parent);
}

void _refresh_bottom(bintree_t *this)
{
  bintreenode_t *bottom, *parent = NULL;
  bottom = this->bottom;
  if(bottom->right) this->bottom = _get_leftest_value(bottom->right, &parent);
  else this->bottom = _get_leftest_value(this->root, &parent);
}

