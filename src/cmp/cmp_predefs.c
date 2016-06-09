#include "cmp_predefs.h"
#include "inc_predefs.h"
#include "inc_texts.h"
#include "lib_tors.h"

/*
#define CMP_NAME_RECYCLE "Handshake constructor/destructor"
#define CMP_PUFFER_RECYCLE_SIZE 16
static void _clean_handshake(handshake_t*);
CMP_DEF_RECPUFFER(
		static,
		handshake_t,
		packet_ctor,
		packet_dtor,
		_clean_handshake,
		CMP_NAME_RECYCLE,
		_cmp_hsrecycler,
		CMP_PUFFER_RECYCLE_SIZE,
		_cmp_hsrecycler_ctor,
		_cmp_hsrecycler_dtor
		);

#undef CMP_NAME_RECYCLE
#undef CMP_PUFFER_RECYCLE_SIZE

#define CMP_NAME_RECYCLE "Packet constructor/destructor"
#define CMP_PUFFER_RECYCLE_SIZE 16
static void _clean_packet(packet_t*);
CMP_DEF_RECPUFFER(
		static,
		packet_t,
		packet_ctor,
		packet_dtor,
		_clean_packet,
		CMP_NAME_RECYCLE,
		_cmp_precycler,
		CMP_PUFFER_RECYCLE_SIZE,
		_cmp_precycler_ctor,
		_cmp_precycler_dtor
		);

#undef CMP_NAME_RECYCLE
#undef CMP_PUFFER_RECYCLE_SIZE

#define CMP_NAME_RECYCLE "Command constructor/destructor"
#define CMP_PUFFER_RECYCLE_SIZE 16
static void _clean_command(command_t*);
CMP_DEF_RECPUFFER(
		static,
		command_t,
		command_ctor,
		command_dtor,
		_clean_command,
		CMP_NAME_RECYCLE,
		_cmp_cmdrecycler,
		CMP_PUFFER_RECYCLE_SIZE,
		_cmp_precycler_ctor,
		_cmp_precycler_dtor
		);

#undef CMP_NAME_RECYCLE
#undef CMP_PUFFER_RECYCLE_SIZE


void cmp_tors_start()
{
	_cmp_precycler_ctor();
	_cmp_hsrecycler_ctor();
	_cmp_cmdrecycler_ctor();
}

void cmp_tors_stop()
{
	_cmp_precycler_dtor();
	_cmp_hsrecycler_dtor();
	_cmp_cmdrecycler_dtor();
}

packet_t* cmp_packet_ctor()
{
	return _cmp_precycler->supplier();
}

void cmp_packet_dtor(packet_t *item)
{
	_cmp_precycler->receiver(item);
}


command_t* cmp_command_ctor()
{
	return _cmp_cmdrecycler->supplier();
}

void cmp_command_dtor(command_t *item)
{
	_cmp_cmdrecycler->receiver(item);
}


handshake_t* cmp_handshake_ctor()
{
	return _cmp_hsrecycler->supplier();
}

void cmp_handshake_dtor(handshake_t *item)
{
	_cmp_hsrecycler->receiver(item);
}

void _clean_packet(packet_t *item)
{
	memset(item, 0, sizeof(packet_t));
}

void _clean_command(command_t *item)
{
	memset(item, 0, sizeof(command_t));
}

void _clean_handshake(handshake_t *item)
{
	memset(item, 0, sizeof(handshake_t));
}
*/
