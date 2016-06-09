#ifndef INCGUARD_MPT_INC_TEXTS //guard MPT_INCGUARD_PROGTEXTS
#define INCGUARD_MPT_INC_TEXTS
#include "lib_debuglog.h"

#define PRINTING(TYPE, ...) INFOPRINT(GET_TEXT_##TYPE(__VA_ARGS__))

#define GET_TEXT_STARTING_SG(sg, ...) "Start "sg
#define PRINTING_STARTING_SG(sg, ...) PRINTING(STARTING_SG, sg, __VA_ARGS__)

#define PRINTING_STARTING_SG_FORSG(sg, FORSG) INFOPRINT("%s (%s)", GET_TEXT_STARTING_SG(sg), FORSG)

#define GET_TEXT_SG_IS_STARTED(sg) sg" is started."
#define PRINTING_SG_IS_STARTED(sg) INFOPRINT("%s", GET_TEXT_SG_IS_STARTED(sg))

#define GET_TEXT_STOPPING_SG(sg) "STOP "sg"."
#define PRINTING_STOPPING_SG(sg) INFOPRINT("%s", GET_TEXT_STOPPING_SG(sg))

#define PRINTING_STOPPING_SG_FORSG(sg, FORSG) INFOPRINT("%s (%s)", GET_TEXT_STOPPING_SG(sg), FORSG)

#define GET_TEXT_SG_IS_STOPPED(sg) sg" is stopped."
#define PRINTING_SG_IS_STOPPED(sg) INFOPRINT("%s", GET_TEXT_SG_IS_STOPPED(sg))

#define GET_TEXT_CONSTRUCTING_SG(sg) "Constructing "sg"."
#define PRINTING_CONSTRUCTING_SG(sg) INFOPRINT("%s", GET_TEXT_CONSTRUCTING_SG(sg))

#define GET_TEXT_SG_IS_CONSTRUCTED(sg) sg" is constructed"
#define PRINTING_SG_IS_CONSTRUCTED(sg) INFOPRINT("%s", GET_TEXT_SG_IS_CONSTRUCTED(sg))

#define GET_TEXT_DESTRUCTING_SG(sg) "Destructing "sg"."
#define PRINTING_DESTRUCTING_SG(sg) INFOPRINT("%s", GET_TEXT_DESTRUCTING_SG(sg))

#define GET_TEXT_SG_IS_DESTRUCTED(sg) sg" is destructed."
#define PRINTING_SG_IS_DESTRUCTED(sg) INFOPRINT("%s", GET_TEXT_SG_IS_DESTRUCTED(sg))

#define GET_TEXT_TESTING_SG(sg) "Testing "sg"."
#define PRINTING_TESTING_SG(sg) logging("%s", GET_TEXT_TESTING_SG(sg))

#define GET_TEXT_SG_NOTEXISTS(sg) sg" is not exists."
#define PRINTING_SG_NOTEXISTS(sg) runtime_warning("%s",GET_TEXT_SG_NOTEXISTS(sg))

#define GET_TEXT_CONNECTION_NOTEXISTS(A,B) "Missing connection between "A" and "B"."
#define GET_TEXT_ASSIGN_NOTEXISTS(SG) "Assign is missing at "SG"."

#define GET_TEXT_SG_ALREADY_EXISTS(sg) sg" is already exists."
#define PRINTING_SG_ALREADY_EXISTS(sg) runtime_warning("%s", GET_TEXT_SG_ALREADY_EXISTS(sg))

#define GET_TEXT_INIT_SG(sg) "Initializing "sg"."
#define PRINTING_INIT_SG(sg) logging("%s", GET_TEXT_INIT_SG(sg))

#define GET_TEXT_DEINIT_SG(sg) "Deinitializing "sg"."
#define PRINTING_DEINIT_SG(sg) logging("%s", GET_TEXT_DEINIT_SG(sg))

#define GET_TEXT_ASSIGN_COMPONENTS "Assign components"
#define PRINTING_ASSIGN_COMPONENTS logging("%s", GET_TEXT_ASSIGN_COMPONENTS)

#define PRINTING_BIND_SG_TO_PROC(SG,PROC) logging("Bind %s(%p) to %s(%p)", #SG, SG, #PROC, PROC)

#define PRINTING_CONNECT_SG1_TO_SG2(SG1,SG2) logging("Connect %s(%p) to %s(%p)", #SG1, SG1, #SG2, SG2)

#define PRINTING_ASSIGNING_SG1_TO_SG2(SG1,SG2) logging("Assign %s(%p) to %s(%p)", #SG1, SG1, #SG2, SG2)

#define GET_TEXT_CONNECT_COMPONENTS "Connect components"
#define PRINTING_CONNECT_COMPONENTS logging("%s", GET_TEXT_CONNECT_COMPONENTS)

#define GET_TEXT_INIT_COMPLETED "Initialization process is completed"
#define PRINTING_INIT_COMPLETED logging("%s", GET_TEXT_INIT_COMPLETED)

#define GET_TEXT_DEINIT_COMPLETED "Deinitialization process is completed"
#define PRINTING_DEINIT_COMPLETED logging("%s", GET_TEXT_DEINIT_COMPLETED)

#define GET_TEXT_CHECK_SG(sg) "Check "sg"."
#define PRINTING_CHECK_SG(sg) logging("%s", GET_TEXT_CHECK_SG(sg))

#define GET_TEXT_CHECK_IS_COMPLETED "Check is completed"
#define PRINTING_CHECK_IS_COMPLETED logging("%s", GET_TEXT_CHECK_IS_COMPLETED)

#define GET_TEXT_TEST_IS_COMPLETED "Test is completed"
#define PRINTING_TEST_IS_COMPLETED logging("%s", GET_TEXT_CHECK_IS_COMPLETED)

#endif //INCGUARD_MPT_INC_TEXTS end
