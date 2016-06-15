#include "inc_mtime.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32 //platform dependent defintions begin

void set_mtime(mtime_t *mtime)
{
	gettimeofday(mtime, NULL);
}

double diffmtime_fromnow(mtime_t *begin)
{
	mtime_t  now;
    set_mtime(&now);
	return diffmtime(&now, begin);
}

double diffmtime(mtime_t *begin, mtime_t *end)
{
    double result;
    result = (begin->tv_sec) * 1000 + (begin->tv_usec) / 1000 ; // convert tv_sec & tv_usec to milliseconds
    result -= (end->tv_sec) * 1000 + (end->tv_usec) / 1000 ;
    return result;
}

void print_mtime(mtime_t* time)
{
	//DEBUGPRINT("%s %ld.%06ld", text, (long int)(time->tv_sec), (long int)(time->tv_usec));
	gettimeofday(time, NULL);

	double time_in_mill =
	         (time->tv_sec) * 1000 + (time->tv_usec) / 1000;
	printf("Timestamp: %f\n", time_in_mill);
}

#else //WIN32 begin 

void set_mtime(mtime_t *mtime)
{
	ftime(mtime);
}

double diffmtime_fromnow(mtime_t *end)
{
	mtime_t now;
	set_mtime(&now);
	return diffmtime(&now, end);
}

double diffmtime(mtime_t *begin, mtime_t *end)
{
	double  result;
    result = (1000.0 * (begin->time - end->time)
        + (begin->millitm - end->millitm));
	return result;
}
#endif //platform dependent defintions end

