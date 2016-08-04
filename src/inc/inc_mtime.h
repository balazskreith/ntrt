#ifndef _WIN32 //platform dependent defintions begin
#include <sys/time.h>
#else
#include <sys\timeb.h>
#endif //platform dependent defintions end

#ifndef INGUARD_NTRT_INC_MTIME_H_
#define INGUARD_NTRT_INC_MTIME_H_


#ifndef _WIN32 //platform dependent defintions begin
typedef struct timeval mtime_t;
#else
typedef struct timeb mtime_t;
#endif //platform dependent defintions end


void set_mtime(mtime_t*);
double diffmtime(mtime_t *begin, mtime_t *end);
void print_mtime(mtime_t* time);
double diffmtime_fromnow(mtime_t *begin);

#define PROFILING(msg,func) \
{  \
  mtime_t start; \
  double_t elapsed; \
  set_mtime(&start); \
  func; \
  elapsed = diffmtime_fromnow(&start); \
  if(1. < elapsed) WARNINGPRINT(msg" elapsed time in ms: %f\n", elapsed); \
} \


#endif //INGUARD_NTRT_INCLUDE_TIME_H_
