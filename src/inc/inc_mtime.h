#ifndef _WIN32 //platform dependent defintions begin
#include <sys/time.h>
#else
#include <sys\timeb.h>
#endif //platform dependent defintions end

#ifndef INGUARD_MPT_INC_MTIME_H_
#define INGUARD_MPT_INC_MTIME_H_


#ifndef _WIN32 //platform dependent defintions begin
typedef struct timeval mtime_t;
#else
typedef struct timeb mtime_t;
#endif //platform dependent defintions end


void set_mtime(mtime_t*);
double diffmtime(mtime_t *begin, mtime_t *end);
void print_mtime(mtime_t* time);
double diffmtime_fromnow(mtime_t *end);



#endif //INGUARD_MPT_INCLUDE_TIME_H_
