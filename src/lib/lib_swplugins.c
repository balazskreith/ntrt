#include "lib_swplugins.h"

static void _minmaxpipe(ptr_t udata, swminmaxstat_t* stat)
{
  if(!stat->min || !stat->max){
    DEBUGPRINT("MinMaxPipe min: %p max: %p", stat->min,stat->max);
    return;
  }
  DEBUGPRINT("MinMaxPipe min: %d max: %d", *(int32_t*) stat->min,*(int32_t*) stat->max);
}

static void _percentilepipe(ptr_t udata, swpercentilecandidates_t* candidates)
{
  if(candidates->processed == BOOL_FALSE){
    DEBUGPRINT("PercentilePipe NO DATA");
    return;
  }
  DEBUGPRINT("PercentilePipe left: %p->%d right: %p->%d",
             candidates->left, candidates->left ? *(int32_t*) candidates->left : 0,
             candidates->right, candidates->right ? *(int32_t*) candidates->right : 0
             );
}

static void sumpipe(ptr_t udata, int32_t sum)
{
  DEBUGPRINT("sliding window sum: %d", sum);
}

void slidingwindow_test()
{
  slidingwindow_t* sw;
  int32_t i;
  int32_t values2[] = {1,2,3,4,4,5,5,5,6};
  sw = slidingwindow_int32_ctor(5, 100.);
  slidingwindow_add_plugins(sw,
                            make_swprinter(swprinter_int32),
                            make_swminmax(bintreecmp_int32, _minmaxpipe, NULL),
                            make_swpercentile(80, bintreecmp_int32, _percentilepipe, NULL),
                            make_swint32_stater(sumpipe, NULL),
                            NULL);

  DEBUGPRINT("Sliding window num limit test");
  DEBUGPRINT("Test case: Insert 1-10 into a 5 limited sw. Expected behavior: remove 1,2,3,4,5");
  for(i = 1; i < 11; ++i){
    slidingwindow_add_int(sw, i);
  }

  slidingwindow_clear(sw);
  DEBUGPRINT("Test case: Insert 6,2,9,7,3,5,10,1,4,8 into a 5 limited sw. Expected behavior: remove 1,2,3,4,5");
  for(i = 0; i < 10; ++i){
    slidingwindow_add_data(sw, &values2[i]);
  }
  slidingwindow_dtor(sw);

//  sw->num_limit = 1;
}

//----------------- SWPrinter plugin --------------------------------------

void swprinter_int32(ptr_t data, char_t* string)
{
  sprintf(string, "%d", *(int32_t*)data);
}


static void _swprinter_add_pipe(ptr_t dataptr, ptr_t itemptr)
{
  char_t string[255];
  void (*sprint)(ptr_t,char_t*);
  memset(string, 0, 255);

  sprint = dataptr;
  sprint(itemptr, string);
  sysio->print_stdout("Added item %s\n", string);
}

static void _swprinter_rem_pipe(ptr_t dataptr, ptr_t itemptr)
{
  char_t string[255];
  void (*sprint)(ptr_t,char_t*);
  memset(string, 0, 255);

  sprint = dataptr;
  sprint(itemptr, string);
  sysio->print_stdout("Removed item %s\n", string);
}

swplugin_t* make_swprinter(void (*sprint)(ptr_t,char_t*))
{
  swplugin_t* this;
  this = swplugin_ctor();
  this->add_pipe = _swprinter_add_pipe;
  this->add_data = sprint;
  this->rem_pipe = _swprinter_rem_pipe;
  this->rem_data = sprint;
  this->disposer = free;
  this->priv     = NULL;
  return this;
}

//-----------------------------------------------------------------------------------

typedef struct _swminmax{
  bintree_t     *tree;
  void         (*minmax_pipe)(ptr_t,swminmaxstat_t*);
  ptr_t          minmax_data;
  swminmaxstat_t stat;
}swminmax_t;


static swminmax_t* _swminmaxpriv_ctor(bintreecmp cmp, void (*minmax_pipe)(ptr_t,swminmaxstat_t*), ptr_t minmax_data)
{
  swminmax_t* this;
  this = malloc(sizeof(swminmax_t));
  memset(this, 0, sizeof(swminmax_t));
  this->tree = make_bintree(cmp);
  this->minmax_pipe = minmax_pipe;
  this->minmax_data = minmax_data;
  return this;
}

static void _swminmaxpriv_disposer(ptr_t target)
{
  swminmax_t* this = target;
  if(!target){
    return;
  }

  bintree_dtor(this->tree);
  free(this);
}

static void _swminmax_disposer(ptr_t target)
{
  swplugin_t* this = target;
  if(!target){
    return;
  }

  _swminmaxpriv_disposer(this->priv);
  this->priv = NULL;
  free(this);
}

static void _swminmax_add_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swminmax_t* this;
  this = dataptr;
  bintree_insert_data(this->tree, itemptr);
  this->stat.min = bintree_get_bottom_data(this->tree);
  this->stat.max = bintree_get_top_data(this->tree);
  if(this->minmax_pipe){
    this->minmax_pipe(this->minmax_data, &this->stat);
  }
}

static void _swminmax_rem_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swminmax_t* this;
  this = dataptr;
  bintree_delete_value(this->tree, itemptr);
  this->stat.min = bintree_get_bottom_data(this->tree);
  this->stat.max = bintree_get_top_data(this->tree);
  if(this->minmax_pipe){
    this->minmax_pipe(this->minmax_data, &this->stat);
  }
}

swplugin_t* make_swminmax(bintreecmp cmp, void (*minmax_pipe)(ptr_t,swminmaxstat_t*), ptr_t minmax_data)
{
  swplugin_t* this;
  this = swplugin_ctor();
  this->priv = _swminmaxpriv_ctor(cmp, minmax_pipe, minmax_data);
  this->add_pipe = _swminmax_add_pipe;
  this->add_data = this->priv;
  this->rem_pipe = _swminmax_rem_pipe;
  this->rem_data = this->priv;
  this->disposer = _swminmax_disposer;
  return this;
}




//-----------------------------------------------------------------------------------

typedef struct _swpercentile{
  bintree_t         *mintree;
  bintree_t         *maxtree;
  void             (*percentile_pipe)(ptr_t,swpercentilecandidates_t*);
  ptr_t              percentile_data;
  int32_t            percentile;
  double             ratio;
  int32_t            required;
  bintreecmp         cmp;
  swpercentilecandidates_t  candidates;
  int32_t            Mxc,Mnc;
}swpercentile_t;


static swpercentile_t* _swpercentilepriv_ctor(
                                              int32_t    percentile,
                                              bintreecmp cmp,
                                              void     (*percentile_pipe)(ptr_t,swpercentilecandidates_t*),
                                              ptr_t      percentile_data)
{
  swpercentile_t* this;
  this = malloc(sizeof(swpercentile_t));
  memset(this, 0, sizeof(swpercentile_t));

  this->percentile      = CONSTRAIN(10,90,percentile);
  this->ratio           = (double)this->percentile / (double)(100-this->percentile);
  this->cmp             = cmp;
  this->mintree = make_bintree(cmp);
  this->maxtree = make_bintree(cmp);
  this->percentile_pipe = percentile_pipe;
  this->percentile_data = percentile_data;
  this->Mxc = this->Mnc = 0;

  if(this->ratio < 1.){
    this->required = (1./this->ratio) + 1;
  }else if(1. < this->ratio){
    this->required = this->ratio + 1;
  }else{
    this->required = 2;
  }
  return this;
}

static void _swpercentilepriv_disposer(ptr_t target)
{
  swpercentile_t* this = target;
  if(!target){
    return;
  }

  bintree_dtor(this->mintree);
  free(this);
}

static void _swpercentile_disposer(ptr_t target)
{
  swplugin_t* this = target;
  if(!target){
    return;
  }

  _swpercentilepriv_disposer(this->priv);
  this->priv = NULL;
  free(this);
}


static void _swpercentile_pipe(swpercentile_t *this)
{
  double ratio;
  if(this->Mnc + this->Mxc < this->required){
    this->candidates.processed  = BOOL_FALSE;
    this->candidates.right = this->candidates.left = NULL;
    goto done;
  }
  this->candidates.processed  = BOOL_TRUE;
  ratio = (double) this->Mxc / (double) this->Mnc;

  if(this->ratio < ratio){
    this->candidates.left  = bintree_get_top_data(this->maxtree);
    this->candidates.right = NULL;
  }else if(ratio < this->ratio){
    this->candidates.left  = NULL;
    this->candidates.right = bintree_get_bottom_data(this->mintree);
  }else{
    this->candidates.left  = bintree_get_top_data(this->maxtree);
    this->candidates.right = bintree_get_bottom_data(this->mintree);
  }

done:
  this->percentile_pipe(this->percentile_data, &this->candidates);
}


static void _swpercentile_balancer(swpercentile_t *this)
{
  double ratio;
  ptr_t data;

  if(this->Mxc + this->Mnc  < this->required){
    goto done;
  }

  if(!this->Mnc){
    goto reduce_maxtree;
  }
  if(!this->Mxc){
    goto reduce_mintree;
  }

  ratio = (double) this->Mxc / (double) this->Mnc;
  DEBUGPRINT("ratio: %f, this->ratio: %f", ratio, this->ratio);
  if(this->ratio < ratio)
    goto reduce_maxtree;
  else
    goto reduce_mintree;

reduce_mintree:
  ratio = (double) (this->Mxc + 1) / (double) (this->Mnc - 1);
  if(this->ratio < ratio || this->Mnc < 1) goto done;
  data = bintree_get_bottom_data(this->mintree);
  bintree_delete_value(this->mintree, data);
  bintree_insert_data(this->maxtree, data);
  --this->Mnc; ++this->Mxc;
  DEBUGPRINT("Reduced Mintree, ratio: %f, this->ratio: %f Mxc: %d Mnc: %d, data: %d", ratio, this->ratio, this->Mxc, this->Mnc, *(int32_t*)data);
  goto reduce_mintree;

reduce_maxtree:
  ratio = (double) (this->Mxc - 1) / (double) (this->Mnc + 1);
  if(ratio < this->ratio || this->Mxc < 1) goto done;
  data = bintree_get_top_data(this->maxtree);
  bintree_delete_value(this->maxtree, data);
  bintree_insert_data(this->mintree, data);
  --this->Mxc; ++this->Mnc;
  DEBUGPRINT("Reduced Maxtree, ratio: %f, this->ratio: %f Mxc: %d Mnc: %d, data: %d", ratio, this->ratio, this->Mxc, this->Mnc, *(int32_t*)data);
  goto reduce_maxtree;

done:
  return;
}

static void _swpercentile_add_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swpercentile_t* this;
  this = dataptr;

  if(this->Mxc + this->Mnc < this->required){
    bintree_insert_data(this->maxtree, itemptr);
    ++this->Mxc;
    goto done;
  }

  if(this->cmp(itemptr, bintree_get_top_data(this->maxtree)) <= 0){
    bintree_insert_data(this->maxtree, itemptr);
    ++this->Mxc;
  }else{
    bintree_insert_data(this->mintree, itemptr);
    ++this->Mnc;
  }

done:
  _swpercentile_balancer(this);
  _swpercentile_pipe(this);
}

static void _swpercentile_rem_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swpercentile_t* this;
  this = dataptr;

  bintree_delete_value(this->maxtree, itemptr);
  bintree_delete_value(this->mintree, itemptr);
  this->Mxc = bintree_get_refnum(this->maxtree);
  this->Mnc = bintree_get_refnum(this->mintree);
  if(this->Mxc + this->Mnc < this->required){
    while(0 < this->Mnc){
      ptr_t data;
      data = bintree_pop_bottom_data(this->mintree);
      bintree_insert_data(this->maxtree, data);
      ++this->Mxc;
      --this->Mnc;
    }
  }

  _swpercentile_balancer(this);
  _swpercentile_pipe(this);
}

swplugin_t* make_swpercentile(
                              int32_t     percentile,
                              bintreecmp  cmp,
                              void      (*percentile_pipe)(ptr_t,swpercentilecandidates_t*),
                              ptr_t       percentile_data
                              )
{

  swplugin_t* this;
  this = swplugin_ctor();
  this->priv = _swpercentilepriv_ctor(
                                      percentile,
                                      cmp,
                                      percentile_pipe,
                                      percentile_data);
  this->add_pipe = _swpercentile_add_pipe;
  this->add_data = this->priv;
  this->rem_pipe = _swpercentile_rem_pipe;
  this->rem_data = this->priv;
  this->disposer = _swpercentile_disposer;
  return this;

}



typedef struct _swint32summer{
  int32_t  sum;
  void   (*pipe)(ptr_t,int32_t);
  ptr_t    pipe_data;
}swint32summer;



static swint32summer* _swint32summer_ctor(void (*pipe)(ptr_t,int32_t),ptr_t pipe_data)
{
  swint32summer* this;
  this = malloc(sizeof(swint32summer));
  memset(this, 0, sizeof(swint32summer));
  this->pipe = pipe;
  this->pipe_data = pipe_data;
  this->sum = 0;
  return this;
}


static void _swint32summer_disposer(ptr_t target)
{
  swplugin_t* this = target;
  if(!target){
    return;
  }

  free(this->priv);
  this->priv = NULL;
  free(this);
}

static void _swint32sum_add_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swint32summer* this;
  this = dataptr;
  this->sum += *(int32_t*)itemptr;
  if(this->pipe){
      this->pipe(this->pipe_data, this->sum);
    }
}

static void _swint32sum_rem_pipe(ptr_t dataptr, ptr_t itemptr)
{
  swint32summer* this;
  this = dataptr;
  this->sum -= *(int32_t*)itemptr;
  if(this->pipe){
    this->pipe(this->pipe_data, this->sum);
  }
}

swplugin_t* make_swint32_stater(void (*pipe)(ptr_t,int32_t),ptr_t pipe_data)
{
  swplugin_t* this;
  this = swplugin_ctor();
  this->priv = _swint32summer_ctor(pipe, pipe_data);
  this->add_pipe = _swint32sum_add_pipe;
  this->add_data = this->priv;
  this->rem_pipe = _swint32sum_rem_pipe;
  this->rem_data = this->priv;
  this->disposer = _swint32summer_disposer;
  return this;
}

