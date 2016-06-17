/**
 * @file lib_puffers.h
 * @brief puffers used in the program
 * @author Bal�zs, Kreith; Debrecen, Hungary
 * @copyright Project maintained by Almasi, Bela; Debrecen, Hungary
 * @date 2014.02.11
*/
#ifndef INCGUARD_NTRT_LIBRARY_PUFFER_H_
#define INCGUARD_NTRT_LIBRARY_PUFFER_H_
#include "../inc/inc_predefs.h"
#include "lib_defs.h"
#include "lib_descs.h"
#include "lib_threading.h"



/** \typedef datapuffer_t
      \brief Describe a puffer used for stores unspecified data
  */
typedef struct datapuffer_struct_t
{
	void                    **items;		///< A pointer array of data the puffer will uses for storing
	int32_t                   length;		///< The maximal amount of data the puffer can store
	int32_t                   start;	///< index for read operations. It points to the next element going to be read
	int32_t                   end;	    ///< index for write operations. It points to the last element, which was written by the puffer
	int32_t                   count;
	void                     *read;
} datapuffer_t;

typedef struct mdatapuffer_struct_t
{
	void     **items;
	int32_t    length;
	int32_t    end;
	int32_t    start;
	int32_t    read_index;
	//int32_t    cat;
	int32_t    count;
	int32_t    _plus;
	void      *_read;
}mdatapuffer_t;

mdatapuffer_t* mdatapuffer_ctor(int32_t items_num);
void mdatapuffer_dtor(mdatapuffer_t *mdatapuffer);
void* mdatapuffer_read(mdatapuffer_t *mdatapuffer);
void mdatapuffer_write(mdatapuffer_t *mdatapuffer, void *item);
int32_t mdatapuffer_readcapacity(mdatapuffer_t *mdatapuffer);
int32_t mdatapuffer_writecapacity(mdatapuffer_t *mdatapuffer);
bool_t mdatapuffer_writeable(mdatapuffer_t *mdatapuffer);
bool_t mdatapuffer_readable(mdatapuffer_t *mdatapuffer);
void mdatapuffer_setreadindex(mdatapuffer_t *mdatapuffer, int32_t plus);
void mdatapuffer_clear(mdatapuffer_t *mdatapuffer, void (*dtor)(void*));
void mdatapuffer_reset(mdatapuffer_t *mdatapuffer);

datapuffer_t* datapuffer_ctor(int32_t items_num);
void datapuffer_dtor(datapuffer_t *datapuffer);
void* datapuffer_read(datapuffer_t *datapuffer);
void* datapuffer_peek(datapuffer_t* puffer);
void datapuffer_write(datapuffer_t *datapuffer, void *item);
int32_t datapuffer_capacity(datapuffer_t *datapuffer);
int32_t datapuffer_readcapacity(datapuffer_t *datapuffer);
int32_t datapuffer_writecapacity(datapuffer_t *datapuffer);
bool_t datapuffer_isfull(datapuffer_t *datapuffer);
bool_t datapuffer_isempty(datapuffer_t *datapuffer);
void datapuffer_clear(datapuffer_t *datapuffer, void (*dtor)(void*));

/** \typedef datapuffer_t
      \brief Describe a puffer used for stores unspecified data
  */
/*
typedef struct datapuffer_struct_t
{
	void                    **items;		///< A pointer array of data the puffer will uses for storing
	int32_t                   length;		///< The maximal amount of data the puffer can store
	int32_t                   read_index;	///< index for read operations. It points to the next element going to be read
	int32_t                   write_index;	///< index for write operations. It points to the last element, which was written by the puffer
	volatile bool_t           is_empty;		///< Indicate weather the puffer is empty or not.
	volatile bool_t	          is_full;		///< Indicate weather the puffer is full or not
	//spin_t	                 *spin;		    ///< point to a mutex used for read and write operations.
} datapuffer_t;
*/
/** \fn callback_t* cback_ctor()
      \brief Initializes a new instance of the datapuffer_t with a specified length
	  \param length the size of the puffer
	  \return Returns with an initialized datapuffer
  */
//datapuffer_t* datapuffer_ctor(int32_t size);

/** \fn void puffer_dtor(void*)
      \brief Destroy an instance of a datapuffer_t
  */
//void datapuffer_dtor(datapuffer_t *puffer);

/** \fn void datapuffer_write(datapuffer_t *puffer, void *data)
      \brief Add an item to a puffer
	  \param puffer The used puffer for adding an item
	  \param data The data will be added to the puffer
  */
//void datapuffer_write(datapuffer_t *puffer, void *data);

/** \fn void* datapuffer_read(datapuffer_t *puffer)
      \brief Remove an element from the puffer and returns with it
	  \param puffer The used puffer for removing an item
	  \return Returns with the removed item
  */
//void* datapuffer_read(datapuffer_t *puffer);


#define GEN_PUFF_RECV_PROC_ROMA(PUFFER_PTR, DATA_PTR, SLEEP_IN_CASE_OF_FULL)  \
	/*if(PUFFER_PTR->is_full == BOOL_TRUE){*/								  \
	if(datapuffer_isfull(PUFFER_PTR) == BOOL_TRUE)					          \
		WARNINGPRINT("Puffer is full.\n");							 		  \
		do{ thread_sleep(SLEEP_IN_CASE_OF_FULL); }							  \
		while(datapuffer_isfull(PUFFER_PTR) == BOOL_TRUE);	  				  \
	}																		  \
	DEBUGPRINT("Writing to the puffer");									  \
	datapuffer_write(PUFFER_PTR, (void*) DATA_PTR);							  \
	/*Romantic policy implementation end*/

#define GEN_PUFF_RECV_PROC_WINE(PUFFER_PTR, DATA_PTR, DATA_DTOR)	 		 \
	/*Wine policy: the older the better, the fresher will wither*/			 \
	/*if(PUFFER_PTR->is_full)											   */\
	if(datapuffer_isfull(PUFFER_PTR) == BOOL_TRUE)					         \
	{																		 \
		DATA_DTOR(DATA_PTR);												 \
		WARNINGPRINT("Packet puffer is full, new packet is dropped.\n");	 \
		return;																 \
	}																		 \
	//Wine policy implementation end*/

#define GEN_PUFF_RECV_PROC_MILK(PUFFER_PTR, DATA_TYPE, DATA_PTR, DATA_DTOR)  			\
	/*Milk policy: the fresher the better, the older will wither*/						\
	if(datapuffer_isfull(PUFFER_PTR) == BOOL_TRUE)										\
	{																					\
		DATA_DTOR((DATA_TYPE*) datapuffer_read(PUFFER_PTR));							\
		WARNINGPRINT("Packet puffer is full, the oldest packet is dropped\n");			\
	}																					\
	datapuffer_write(PUFFER_PTR, (void*) DATA_PTR);							  			\
	//Milk policy implementation end*/


#define GEN_PUFF_SUPL_PROC(PUFFER_PTR, DATA_TYPE, DATA_PTR, ENTRTY_RESULT)				\
	/*Data supply process*/																\
	if(datapuffer_isempty(PUFFER_PTR) == BOOL_TRUE)										\
	{																					\
		return ENTRTY_RESULT;															\
	}																					\
	DEBUGPRINT("Reading from the puffer");												\
	DATA_PTR = (DATA_TYPE*) datapuffer_read(PUFFER_PTR);
	/*Data supply process end*/

#define GEN_PUFF_CLEAR_PROC(PUFFER_PTR, ITEM_TYPE, ITEM_DTOR)							\
		ITEM_TYPE* item;																\
		while(datapuffer_isempty(PUFFER_PTR) == BOOL_FALSE){							\
			item = (ITEM_TYPE*) datapuffer_read(PUFFER_PTR);							\
			ITEM_DTOR(item);															\
		}																				\

#endif //INCGUARD_NTRT_LIBRARY_PUFFER_H_
