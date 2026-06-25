#ifndef _SHARED_MEMORY_OBJECT
#define _SHARED_MEMORY_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sharedMemoryObject.h 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the shared memory object that handles shared memory 
//  operations.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
//
// User Defined Include Files:
//
#include "Typedefs.h"
#include "database.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
//
#define DATABASE_KEY            (key_t) 0xDADABACE
#define INTERFACE_KEY		(key_t)	0xEDAAFACE
#define INTERPROC_KEY		(key_t)	0xEDAB0C00
#define PRIVATE_SPACE_SIZE	1000

class sharedMemoryObject
{
  public:
     sharedMemoryObject(key_t key, int size, int mode);
    ~sharedMemoryObject();
     void * getAddr() {return pMem;};
     int    getSize() {return memSize;};
     int    isCreatedByMe() {return createdByMe;};
     void * DeclarePrivateSpace(int size);
  private:
     void * 	AttachSegment(key_t key, int size, int mode);
     void	RemoveSegment(void * address);
     void	setAddr(void *a) {pMem = a;};
     void	setSize(int s) {memSize = s;};
     void	setCreatedByMe(int b) {createdByMe = b;};
     void *	pMem;		// pointer to shared memory
     key_t	key;		// user defined, passed in
     int	shmid;		// shared memory Id
     int	createdByMe;	// indicator that this object created
				// shared mem
     int	memSize;	// size of public portion of shared memory
                                // actual size is memSize + PRIVATE_SPACE_SIZE
};
#endif

