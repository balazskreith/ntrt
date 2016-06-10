#ifndef INGUARD_NTRT_INC_PREDEFS_H_
#define INGUARD_NTRT_INC_PREDEFS_H_
#include <stdio.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <string.h>
#include <errno.h>        /* errno */
#include <sys/cdefs.h>
extern int errno;

//----------------------------------------------------
//original macros are available at:http://c.learncodethehardway.org/book/ex20.html
#define _CLEAN_ERRNO() (errno == 0 ? "None" : strerror(errno))

#define CRITICALPRINT(M, ...) sysio->print_stdout("[CRITIC] "M"\n", __VA_ARGS__)

#define ERRORPRINT(M, ...) sysio->print_stdout("[ERR] (errno: %s) "M"\n", _CLEAN_ERRNO(), ##__VA_ARGS__)

#define WARNINGPRINT(M, ...)
//sysio->print_stdout("[WARN] (errno: %s) "M"\n",  _CLEAN_ERRNO(), ##__VA_ARGS__)

#define INFOPRINT(M, ...) sysio->print_stdout("[INFO] "M"\n",  ##__VA_ARGS__)

#define DEBUGPRINT(M, ...) sysio->print_stdout("[DEBUG] "M"\n",  ##__VA_ARGS__)

#ifdef TEST_ENABLED
#define ASSERT_TRUE(A, M, ...) if(!(A)) { ERRORPRINT(M, ##__VA_ARGS__); errno=0; EXERROR("Test assertation failed", EXIT_FAILURE); }
#define SENTINEL(M, ...)  { ERRORPRINT(M, ##__VA_ARGS__); errno=0; EXERROR("Program reached a sentinel point.", EXIT_FAILURE); }
#else
#define ASSERT_TRUE(A, M, ...)
#define SENTINEL(M, ...)
#endif

#define EXERROR(exit_text,exit_num)  {ERRORPRINT("%s:%d", exit_text, exit_num); exit(exit_num);}
#define BZERO(mem_addr, size) memset(mem_addr, 0, size)
#define OTUNSETIFF     (('T'<< 8) | 202)

#endif //INCGUARD_NTRT_LIBRARY_PREPROCESSES_H_
