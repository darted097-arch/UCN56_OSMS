//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sharedMemoryObject.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This file implements the shared Memory object.
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "sharedMemoryObject.h"
//
// External Variables:
//
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
#include "prototypes.h"
//
// ***************************************************************************
//
sharedMemoryObject::sharedMemoryObject(key_t k, int size, int mode)
{
  shmid = -1;
  key = k;
  setCreatedByMe(FALSE);
  setSize(0);
  setAddr(AttachSegment(key, size, mode));
  if (NULL != getAddr())
  {
    cout << " Successfully attached to shared memory for key = " << hex << key << endl;
    setSize(size);
#ifdef DEBUG
    cout << " Shared memory Address = " << hex << getAddr() << " shmid = " << hex << shmid << endl;
#endif
  }//endif
  else
  {
    cout << " FATAL ERROR: Could not attach to shared memory for key = " << hex << key << endl;
    exit(1); // exit the task!!!
  }
}

sharedMemoryObject::~sharedMemoryObject()
{
   struct shmid_ds buf;
   
   cout << " Shared Memory destructor called for id = " << hex << shmid << endl;
   shmctl(shmid, IPC_STAT, &buf);
   if (buf.shm_nattch > 1)
   {
      cout << " Shared Memory for id = " << hex << shmid << " attached ";
      cout << "to " << buf.shm_nattch-1 << " other task(s)." << endl; 
      cout << " Shared Memory for id = " << hex << shmid << " not removed\n";
   }
   else RemoveSegment(getAddr());
}

void * sharedMemoryObject::AttachSegment(key_t key, int size, int mode)
{
  void * address;
  struct shmid_ds buf;

  /*  Assume the shared memory segment already exists - get its ID */
  shmid = shmget(key, 0, 0);

  if (-1 == shmid)
  {
#ifdef DEBUG
    perror( "Cannot get shared segment. Need to create it");
    cout << " Cannot get shared segment. Need to create it" << endl;
#endif
    
    if (0 == size) return NULL;

    /*  The shared memory segment doesn't exist, so create it  */
    shmid = shmget(key, size+PRIVATE_SPACE_SIZE, IPC_CREAT | 0666 );
    if( shmid == -1 )
    {
      perror( "cannot create shared segment" );
      printf( "key = %X.H   size = %d   flg = %#o\n",
        key, size, IPC_CREAT | 0666 );
      printf( "key = %X.H   size = %d   flg = %#o\n",
        key, size, IPC_CREAT | 0666 );
      return NULL;
    }
    else
    {
       setCreatedByMe(TRUE);
    }//endif
  }//endif (shmid == -1)

  if (-1 == shmctl(shmid, IPC_STAT, &buf))
  {
    perror( "shmctl failed" );
    return NULL;
  }

  /*  Attach segment to address space  */
  address = shmat(shmid, NULL, mode);
  if (address == (void *)-1)
  {
    perror( "attaching shared segment failed" );
    return NULL;
  }
  return address;
}

void sharedMemoryObject::RemoveSegment(void * address )
{
  shmdt(address);	/* detach shared segment */

  /*  Remove the shared segment  */
  if (-1 == shmctl(shmid, IPC_RMID, NULL))
    perror( "shmctl failed to remove segment" );
}


void * sharedMemoryObject::DeclarePrivateSpace(int size)
{
   static char * topPrivateSpace = ((char *)getAddr()) + getSize();
   char * nextFree = topPrivateSpace;
   void * retAddr;
   
   if ( size <= (PRIVATE_SPACE_SIZE - (nextFree - topPrivateSpace)))
   {
      retAddr = (void *)nextFree;
      nextFree += size;
      cout << "sharedMemoryObject:: Bytes used so far: " 
	   << dec << (nextFree - topPrivateSpace) << endl;
      flush(cout);
   }
   else
   {
      retAddr = 0;
      cout << "sharedMemoryObject:: Not enough space in shared memory for "
	   << "variable" << endl;
      cout << " Need " << dec << size << " bytes but only have "
           <<  dec << PRIVATE_SPACE_SIZE - (nextFree - topPrivateSpace)
           << " bytes." << endl;
   }
   return retAddr;
}
