#ifndef INCGUARD_LIB_DEBUG_H_
#define INCGUARD_LIB_DEBUG_H_
#include <stdio.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <string.h>
#include <errno.h>        /* errno */
#include "lib_defs.h"
#include "lib_descs.h"

extern int errno;

bool_t* dwbv_; //debugging watched boolean variable
#define DEBUG_FLOWS
//#define LOGGING_ENABLED
#define RUNTIME_WARNINGS
//#define DEBUG_LOCKS;

#ifdef NDEBUG

#define debug_packetflow(packet, state)
#define debug_flow(cmp, ...)
#define debug_lock(lockname, ...)
#define debug_lockcall(function_name, ...)
#define debug_rdlock(lockname)
#define debug_rdunlock(lockname)
#define debug_wrlock(lockname)
#define debug_wrunlock(lockname)
#define debug_unlock(lockname)
#define runtime_warning(M, ...)
#define logging(M, ...)

#else

#define DEBUGLOG(TYPE, M, ...) sysio->print_stdlog("[DEBUG]"TYPE" %s:%d: "M"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define FLOWDEBUG(M,...) DEBUGLOG("[FLOW]", M, __VA_ARGS__)
#define MEMDEBUG(M,...)  DEBUGLOG("[MEM]", M, __VA_ARGS__)
#define PERFDEBUG(M,...) DEBUGLOG("[PERF]", M, __VA_ARGS__)
#define VARDEBUG(M,...) DEBUGLOG("[VAR]", M, __VA_ARGS__)
#define LOCKDEBUG(M,...) DEBUGLOG("[LOCKS]", M, __VA_ARGS__)
#define TYPEDEBUG(M,...) DEBUGLOG("[TYPE]", M, __VA_ARGS__)

#define WARNDEBUG(M,...) DEBUGLOG(" Warning: ", M, __VA_ARGS__)
#define LOGDEBUG(M,...) DEBUGLOG(" Log: ", M, __VA_ARGS__)

#if (defined DEBUG_FLOWS && defined DEBUG_PACKET) || defined DEBUG_ALL
#define debug_packetflow(packet, state) _d_pflow(packet, state)
void _d_pflow(packet_t *packet,  packet_state_t new_state);
#else
#define debug_packetflow(packet)
#endif


#if defined DEBUG_FLOWS || defined DEBUG_ALL
#define debug_flow(M, ...) FLOWDEBUG(M, __VA_ARGS__)
#else
#define debug_flow(M, ...)
#endif

#if defined DEBUG_LOCKS || defined DEBUG_ALL
#define debug_rdlock(lockname) LOCKDEBUG("%s is locked for reading", lockname)
#define debug_rdunlock(lockname) LOCKDEBUG("%s is unlocked from reading", lockname)
#define debug_wrlock(lockname) LOCKDEBUG("%s is locked for writing", lockname)
#define debug_wrunlock(lockname) LOCKDEBUG("%s is unlocked from writing", lockname)
#define debug_lock(lockname) LOCKDEBUG("%s is locked", lockname)
#define debug_unlock(lockname) LOCKDEBUG("%s is unlocked", lockname)
#else
#define debug_rdlock(lockname)
#define debug_rdunlock(lockname)
#define debug_wrlock(lockname)
#define debug_wrunlock(lockname)
#define debug_lock(lockname)
#define debug_unlock(lockname)
#endif

#if defined DEBUG_LOCKCALLS || defined DEBUG_ALL
#define debug_lockcall(function_name) LOCKDEBUG("%s is called", function_name)
#else
#define debug_lockcall(function_name)
#endif

#if defined(RUNTIME_WARNINGS) || defined(DEBUG_ALL)
#define runtime_warning(M, ...) WARNINGPRINT(M, __VA_ARGS__)
#else
#define runtime_warning(M, ...)
#endif

#if defined LOGGING_ENABLED || defined DEBUG_ALL
#define logging(M, ...) LOGDEBUG(M, __VA_ARGS__)
#else
#define logging(M, ...)
#endif

/*
#if defined DEBUG_FLOWS || defined DEBUG_ALL
#define debug_flow(M, ...) FLOWDEBUG(M, __VA_ARGS__)
#else
#define debug_flow(M, ...)
#endif
*/

#endif //NDEBUG
#endif /* INCGUARD_LIB_DEBUG_H_ */
