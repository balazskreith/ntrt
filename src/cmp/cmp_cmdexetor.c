#include "cmp_cmdexetor.h"
#include "lib_tors.h"
#include "dmap.h"
#include "inc_mtime.h"
#include "lib_funcs.h"
#include "inc_predefs.h"
#include "inc_texts.h"
#include "lib_dispers.h"
#include "lib_makers.h"
#include "lib_debuglog.h"
#include "cmp_predefs.h"
#include "lib_descs.h"
#include "lib_puffers.h"

static void _cmp_cmdexetor_init();
static void _cmp_cmdexetor_deinit();
static void* _cmp_cmdexetor_start();
static void* _cmp_cmdexetor_stop();
#define CMP_NAME_CMDEXETOR "Command exectutor component"
CMP_DEF(, 			    /*type of definitions*/
		cmp_cmdexetor_t,          /*type of component*/
		CMP_NAME_CMDEXETOR,   /*name of the component*/
		 _cmp_cmdexetor,           /*variable name of the component*/
		 cmp_cmdexetor_ctor,      /*name of the constructor function implemented automatically*/
		 cmp_cmdexetor_dtor,      /*name of the destructor function implemented automatically*/
		 _cmp_cmdexetor_init,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 _cmp_cmdexetor_deinit      /*name of the external function called before destruction*/
		);

CMP_DEF_GET_PROC(get_cmp_cmdexetor, cmp_cmdexetor_t, _cmp_cmdexetor);

static void _cmd_dtor(command_t *command);
//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents type definitions --------------------------------------------
//----------------------------------------------------------------------------------------------------

typedef struct _cmp_cmdrelayer_struct_t
{
	command_t*   (*demand)();
	void         (*send2exe)(command_t*);
}_cmp_cmdrelayer_t;

typedef struct _cmp_cmdnext_struct_t
{
	command_t*   (*demand)();
	command_t*   (*supplier)();
	void         (*receiver)(command_t*);
	command_t*     command;
}_cmp_cmdnext_t;

typedef struct _cmp_cmdexe_struct_t
{
	void         (*receiver)(command_t*);
	void         (*send2nxt)(command_t*);
	void         (*send2res)(cmdres_t*);
	void         (**send2req)(request_t*);
}_cmp_cmdexe_t;

typedef struct _cmp_cmdresevalor_struct_t
{
	void         (*receiver)(cmdres_t*);
	void         (*send2puf)(command_t*);
	void         (**send2req)(request_t*);
}_cmp_cmdresevalor_t;

//----------------------------------------------------------------------------------------------------
//---------------------------- Subcomponents declarations --------------------------------------------
//----------------------------------------------------------------------------------------------------

#define CMP_PUFFER_HSCHECKPUFFER_SIZE 16
#define CMP_NAME_CMDPUFFER "Command puffer"
CMP_DEF_SGPUFFER(
			static,					/*type of declaration*/
			command_t,				/*type of items*/
			cmd_dtor,			/*name of the destrctor process*/
			CMP_NAME_CMDPUFFER,    /*name of the component*/
			_cmp_cmdpuffer,		/*name of the variable used for referencing it*/
			CMP_PUFFER_HSCHECKPUFFER_SIZE,	/*maximal number of items in the puffer*/
			_cmp_cmdpuffer_ctor,	/*name of the process used for constructing*/
			_cmp_cmdpuffer_dtor	/*name of the process used for deconstructing*/
		);
#undef CMP_NAME_ROUTPUFFER


#define CMP_NAME_CMDRELAYER "Command relayer component"
static void _thr_cmdrelayer_main_proc(thread_t*);
CMP_THREAD(
		static,				/*type of declaration*/
	   _cmp_cmdrelayer_t,	/*type of the threaded component*/
	   cmdrelayer,			/*unique name used for thread and component*/
	   CMP_NAME_CMDRELAYER,	/*name of the component*/
	   __CMP_NO_INIT_FUNC_,		/*name of the process initialize the component*/
	   __CMP_NO_DEINIT_FUNC_,	/*name of the process deinitialize the component*/
	   _cmdrelayer_start,	/*name of the process activate the thread*/
	   _cmdrelayer_stop,		/*name of the process deactivate the thread*/
	   _thr_cmdrelayer_main_proc	/*name of the process called in the thread*/
	   );

static void _cmp_cmdnext_receiver(command_t*);
static command_t* _cmp_cmdnext_supplier();
#define CMP_NAME_CMDNEXT "Next command container component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_cmdnext_t,          /*type of component*/
		CMP_NAME_CMDNEXT,   /*name of the component*/
		 _cmp_cmdnext,           /*variable name of the component*/
		 _cmp_cmdnext_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_cmdnext_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_cmdexe_process(command_t*);
#define CMP_NAME_CMDEXE "Command execution component"
CMP_DEF(static, 			    /*type of definitions*/
		_cmp_cmdexe_t,          /*type of component*/
		CMP_NAME_CMDEXE,   /*name of the component*/
		 _cmp_cmdexe,           /*variable name of the component*/
		 _cmp_cmdexe_ctor,      /*name of the constructor function implemented automatically*/
		 _cmp_cmdexe_dtor,      /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,       /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,       /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_      /*name of the external function called before destruction*/
		);

static void _cmp_cmdresevalor_process(cmdres_t*);
#define CMP_NAME_CMDRESEVALOR "Command result evaluator component"
CMP_DEF(static, 			      /*type of definitions*/
		_cmp_cmdresevalor_t,      /*type of component*/
		CMP_NAME_CMDRESEVALOR,         /*name of the component*/
		 _cmp_cmdresevalor,       /*variable name of the component*/
		 _cmp_cmdresevalor_ctor,  /*name of the constructor function implemented automatically*/
		 _cmp_cmdresevalor_dtor,  /*name of the destructor function implemented automatically*/
		 __CMP_NO_INIT_FUNC_,         /*name of the external function called after construction*/
		 __NO_TEST_FUNC_,         /*name of the external function called after initialization*/
		 __CMP_NO_DEINIT_FUNC_        /*name of the external function called before destruction*/
		);


//----------------------------------------------------------------------------------------------------
//------------------------- Initializations  ---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmp_cmdexetor_init()
{

	_cmp_cmdpuffer_ctor();
	_cmp_cmdrelayer_ctor();
	_cmp_cmdexe_ctor();
	_cmp_cmdnext_ctor();
	_cmp_cmdresevalor_ctor();

	//Bind
	CMP_BIND(_cmp_cmdexetor->start,   _cmp_cmdexetor_start);
	CMP_BIND(_cmp_cmdexetor->stop,    _cmp_cmdexetor_stop);

	CMP_BIND(_cmp_cmdexe->receiver,   _cmp_cmdexe_process);
	CMP_BIND(_cmp_cmdnext->receiver,   _cmp_cmdnext_receiver);
	CMP_BIND(_cmp_cmdnext->supplier,   _cmp_cmdnext_supplier);
	CMP_BIND(_cmp_cmdresevalor->receiver, _cmp_cmdresevalor_process);

	//Connect
	CMP_CONNECT(_cmp_cmdnext->demand,       _cmp_cmdpuffer->supplier);
	CMP_CONNECT(_cmp_cmdrelayer->demand,    _cmp_cmdnext->supplier);
	CMP_CONNECT(_cmp_cmdrelayer->send2exe,  _cmp_cmdexe->receiver);

	CMP_CONNECT(_cmp_cmdexe->send2nxt,      _cmp_cmdnext->receiver);
	CMP_CONNECT(_cmp_cmdexe->send2res,      _cmp_cmdresevalor->receiver);
	CMP_CONNECT(_cmp_cmdexe->send2req,      &_cmp_cmdexetor->send);
	CMP_CONNECT(_cmp_cmdresevalor->send2puf, _cmp_cmdpuffer->receiver);
	CMP_CONNECT(_cmp_cmdresevalor->send2req, &_cmp_cmdexetor->send);

	CMP_CONNECT(_cmp_cmdexetor->receiver,  _cmp_cmdpuffer->receiver);
}


void _cmp_cmdexetor_deinit()
{
	_cmp_cmdresevalor_dtor();
	_cmp_cmdpuffer_dtor();
	_cmp_cmdnext_dtor();
	_cmp_cmdrelayer_dtor();
	_cmp_cmdexe_dtor();
}

//----------------------------------------------------------------------------------------------------
//------------------------- Activator  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void* _cmp_cmdexetor_start()
{
	_cmdrelayer_start();
	return NULL;
}

void* _cmp_cmdexetor_stop()
{
	_cmdrelayer_stop();
	return NULL;
}

//----------------------------------------------------------------------------------------------------
//------------------------- Processes  ---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _thr_cmdrelayer_main_proc(thread_t *thread)
{
	CMP_DEF_THIS(_cmp_cmdrelayer_t, (_cmp_cmdrelayer_t*) thread->arg);
	command_t   *command;
	time_t       now;
	double       seconds;

	do
	{
		//CMP_DEMAND(CMP_NAME_CMDRELAYER, command, this->demand);
		command = this->demand();
		time(&now);
		seconds = difftime(now,command->created);
		if(seconds > NTRT_COMMAND_TIMEOUT_IN_SECOND){
			ERRORPRINT("Command %s execution failed. Reason: Timeout\n", command->name);
			_cmd_dtor(command);
			return;
		}

		command->state = COMMAND_STATE_EXECUTEABLE;
		//CMP_SEND(CMP_NAME_CMDRELAYER, this->send2exe, command);
		this->send2exe(command);
	}while(thread->state == THREAD_STATE_RUN);
}

void _cmp_cmdnext_receiver(command_t *command)
{
	CMP_DEF_THIS(_cmp_cmdnext_t, _cmp_cmdnext);
	//CMP_RECEIVE(CMP_NAME_CMDNEXT, command);
	this->command = command;
}

command_t* _cmp_cmdnext_supplier()
{
	CMP_DEF_THIS(_cmp_cmdnext_t, _cmp_cmdnext);
	command_t* result;
	if(this->command == NULL){
		return this->demand();
	}
	result = this->command;
	this->command = NULL;
	//CMP_SUPPLY(CMP_NAME_CMDNEXT, result);
	return result;
}

void _cmp_cmdexe_process(command_t *command)
{
	CMP_DEF_THIS(_cmp_cmdexe_t, _cmp_cmdexe);

	command_t   *next_command = NULL;
	cmdres_t    *cmdres = NULL;
	request_t   *request = NULL;
	void (*send2req)(request_t*);
	send2req = *(_cmp_cmdresevalor->send2req);

	INFOPRINT("Executing command: %s", command->name);
	if(command->action != NULL){
		cmdres = (cmdres_t*) execute_rcback(command->action);
	}

	command->state = COMMAND_STATE_EXECUTED;
	if(command->callback != NULL){
		execute_cback(command->callback);
	}

	//next_command = command->next_command;
	//command->next_command = NULL;
	//request = command->request;
	//command->request = NULL;
	ptrmov(&next_command, &command->next_command);
	ptrmov(&request, &command->request);
	_cmd_dtor(command);

	if(next_command != NULL){
		//CMP_SEND(CMP_NAME_CMDEXE, this->send2nxt, next_command);
		this->send2nxt(next_command);
	}

	if(cmdres != NULL){
		//CMP_SEND(CMP_NAME_CMDEXE, this->send2res, cmdres);
		this->send2res(cmdres);
	}

	if(request != NULL){
		send2req(request);
	}
}

void _cmp_cmdresevalor_process(cmdres_t *cmdres)
{
	CMP_DEF_THIS(_cmp_cmdresevalor_t, _cmp_cmdresevalor);
	void (*send2req)(request_t*);
	send2req = *(this->send2req);

	if(cmdres == NULL){
		return;
	}
	//CMP_RECEIVE(CMP_NAME_CMDRESEVALOR, cmdres);
	if(cmdres->command != NULL){
		//CMP_SEND(CMP_NAME_CMDRESEVALOR, this->send2puf, cmdres->command);
		this->send2puf(cmdres->command);
		cmdres->command = NULL;
	}

	if(cmdres->request != NULL){
		send2req(cmdres->request);
		cmdres->request = NULL;
	}
	cmdres_dtor(cmdres);
}

//----------------------------------------------------------------------------------------------------
//------------------------- Private tools  -----------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void _cmd_dtor(command_t *command)
{
	if(command == NULL){
		return;
	}

	if(command->action != NULL && command->action->disposable == BOOL_FALSE){
		command->action = NULL;
	}

	if(command->signal == NULL){
		cmd_dtor(command);
		return;
	}
	signal_lock(command->signal);
	signal_set(command->signal);
	signal_unlock(command->signal);
	command->signal = NULL;
	cmd_dtor(command);
}
