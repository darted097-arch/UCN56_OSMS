#ifndef _DATA_QUEUE_OBJECT_H
#define _DATA_QUEUE_OBJECT_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// DataQueueObject.h    
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is a generic data queue on which data queue items
//  can be placed.
// ***************************************************************************
//
// ***************************************************************************
// System Include Files
//
#include <stdlib.h>
#include <sys/types.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
const int DQO_NON_SPECIFIC_TYPE	= 0;
const int MAX_DATA_QUEUE_ITEM_SIZE = 0x1000;	// 16K bytes
//
// miscellaneous constants
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
class DataQueueItemObject
{
   public:

      DataQueueItemObject(const void * data, size_t dSize);
      DataQueueItemObject(const void * data, size_t dSize, int type);
     ~DataQueueItemObject(){freeData();};

      DataQueueItemObject * 	getNext(){return pNext;};
      size_t	getdataSize(){return dataSize;};
      int	getdataType(){return dataType;};

      size_t	putData(void * buf, size_t bufSize);

      void	setNext(DataQueueItemObject * p){pNext = p;};

   private:
      void	freeData(){free(pData);};

      int			dataType;	// type of data
      size_t			dataSize;
      DataQueueItemObject * 	pNext;
      void * 			pData;
};



class DataQueueObject
{
  public:
     DataQueueObject();
    ~DataQueueObject();

     // Returns TRUE if queue is empty, FALSE otherwise
     int isEmpty(){if(NULL == pFront) return TRUE; else return FALSE;};
     
     // Removes the item at the front of the queue.  Returns the size of the 
     //item.  Caller should ensure that the buffer is of the correct type and 
     //size.  Only the passed in size will be copied into the passed buffer.
     //If the returned size is larger that the passed size, than not all 
     //the data on the queue item was copied into the buffer.
     size_t removeItem(void * buffer, size_t size);

     // get the next item in the queue of type "type".  Removes the item 
     //from the queue.  Returns size of the item.  Caller must ensure that
     //buffer pointed to is large enough to hold the item being returned.  
     //Only the passed size of bytes will be copied into the passed buffer
     size_t removeItem(int type, void * buffer, size_t size);

     // If there is something on the queue, return the size of the item at 
     //the front of the queue,or 0 if the queue is empty.
     size_t getItemSize();

     // If there is an item on the queue of the passed type, return the size 
     //of the first item on the queue of the passed type, or 0 if there 
     //are no items on the queue of the passed type.
     size_t getItemSize(int type);

     // return the type of the next item in the queue or -1 if the queue
     //is empty.
     int getdataType();

     // add an item to the back of the queue. Return 0 on fail, 
     //size of buffer otherwise.
     int addItem(const void * buffer, size_t bufSize);
     int addItem(const void * buffer, size_t bufSize, int type);

     // remove all items from the queue.  Throw them away.
     void removeAllItems();
     void removeAllItems(int type);

  private:
     DataQueueItemObject *	pFront;	// pointer to front of queue
     DataQueueItemObject *	pBack;	// pointer to back of queue
};



#endif

