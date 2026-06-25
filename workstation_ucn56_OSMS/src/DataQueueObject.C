//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// DataQueueObect.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This file implements the DataQueue object.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <stdlib.h>
#include <values.h>
//
// User Defined Include Files:
//
#include "DataQueueObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
DataQueueObject::DataQueueObject()
{
   cout << " DataQueueObject constructor" << endl;
   pFront = NULL;
   pBack = NULL;
}

DataQueueObject::~DataQueueObject()
{
   cout << " DataQueueObject destructor" << endl;
   removeAllItems();
}


int DataQueueObject::addItem(const void * buffer, size_t bufSize)
{
   return(addItem(buffer, bufSize, DQO_NON_SPECIFIC_TYPE));
}


int DataQueueObject::addItem(const void * buffer, size_t bufSize, int type)
{
   DataQueueItemObject *	pDQIO;

#ifdef DEBUG
   cout << " DataQueueObject::addItem(" << dec << type
	<< ") attempting to add and item" << endl;
   cout << "   *buffer = " << hex << (void*)buffer 
	<< " size = " << dec << bufSize << endl;
#endif
   // limit size of buffer
   if (MAX_DATA_QUEUE_ITEM_SIZE < bufSize  ||  0 >= bufSize)
   {
      cerr << "***** ERROR DataQueueObject() Size of requested item = "
	   << dec << bufSize << " is bigger than max = " 
	   << MAX_DATA_QUEUE_ITEM_SIZE << " or equal to 0 or less" << endl;
      return(0);
   }

   // allocate a new DataQueueItemObject and add it to the back of the queue
   // Save the current pointer to the back of the queue
   pDQIO = pBack;

   // create a new item for the back of the queue and point pBack to it
   pBack = new DataQueueItemObject(buffer, bufSize, type);

   // If the queue is empty, set the front of the queue to the new item.
   // Otherwise set the next pointer of what was the last item on the queue to 
   //point to the new last item on the queue.
   if(isEmpty()) pFront = pBack;
   else	pDQIO->setNext(pBack);

   return(bufSize);	// success
}


size_t DataQueueObject::removeItem(void * buffer, size_t size)
{
   return(removeItem(DQO_NON_SPECIFIC_TYPE, buffer, size));
}


size_t DataQueueObject::removeItem (int type, void * buffer, size_t size)
{
//   Copies the data in the data queue entry into the passed buffer
// and returns the size of the data in the data queue entry or 0
// on error.  The caller should ensure the passed buffer is large enough 
// to hold the data in the data queue entry.

   int	retlen;         // the return code, size of data in item on the queue
   DataQueueItemObject *	pDQIO;
   DataQueueItemObject *	pPrevDQIO;
#ifdef DEBUG
   cout << "  DataQueueObject::removeItem(" << dec << type << ", " 
        << hex << (void *)buffer << ", "
	<< dec << size << ")" << endl;
#endif
   if(isEmpty())
   {
#ifdef DEBUG
      cout << " DataQueueObject::putData(): Can not return an item. "
	   << "Queue is empty!" << endl;
#endif
      return(0);
   }

   pDQIO = pFront;
   pPrevDQIO = pFront;

   // if we are not looking for a specific type of item
   if (DQO_NON_SPECIFIC_TYPE != type)
   {
      // look for the specific type of item on the queue
      while(type != pDQIO->getdataType())
      {
         pPrevDQIO = pDQIO;
         pDQIO = pDQIO->getNext();
         if (NULL == pDQIO)
         {
	    cerr << "DataQueueObject::putData(): No item of type = " 
	    << dec << type << " on the queue" << endl;
	    return(0);
         }
      } // endwhile
   } // endif

   // put the data from the object into the buffer referenced by the passed 
   //buffer pointer
   retlen = pDQIO->putData(buffer, size);

   // if we skiped any queue entries because they were not the requested 
   //type, adjust the Next pointer over the object we are going to remove
   //Note: pDQIO points to the item we are going to remove
   if(pPrevDQIO != pDQIO) pPrevDQIO->setNext(pDQIO->getNext());

   // if we are removing the first item, set the front pointer to the next 
   //item in the queue. This is the normal case.
   if(pFront == pDQIO) pFront = pDQIO->getNext();
   
   // delete the data queue item
   delete pDQIO;

   // reset back pointer for empty queue
   if(NULL == pFront) pBack = NULL;
   
   // return the length of the data
   return(retlen);
}


int DataQueueObject::getdataType()
{
   if(isEmpty()) return(-1);
   else return(pFront->getdataType());
}


size_t DataQueueObject::getItemSize()
{
   if(isEmpty()) return(0);
   else return(pFront->getdataSize());
}


size_t DataQueueObject::getItemSize(int type)
{
   DataQueueItemObject *	pDQIO;

#ifdef DEBUG
      cout << " DataQueueObject::getItemSize(" << dec << type << ")" << endl;
#endif

   if(isEmpty())
   {
#ifdef DEBUG
      cout << " DataQueueObject::getItemSize(" << dec << type
	   << "): No items on queue!  Returning 0" << endl;
#endif
      return(0);
   }

   // if type is for a non specific type of item, invoke that method and return
   if (DQO_NON_SPECIFIC_TYPE == type) return(getItemSize());

   // find the first item of the requested type
   pDQIO = pFront;
   while(type != pDQIO->getdataType())
   {
      pDQIO = pDQIO->getNext();
      if (NULL == pDQIO)
      {
	 cerr << "DataQueueObject::getItemSize(): No item of type = " 
	 << dec << type << " on the queue" << endl;
	 return(0);
      }
   }
#ifdef DEBUG
      cout << " DataQueueObject::getItemSize(" << dec << type
	   << "): found an item of input type. Size = " << pDQIO->getdataSize() 
	   << " Type = " << pDQIO->getdataType() << endl;
#endif
   return(pDQIO->getdataSize());
}


void DataQueueObject::removeAllItems()
{
   DataQueueItemObject * pTemp;

   if (isEmpty())
   {
#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems(): Nothing to remove."
	   << " Queue is empty!" << endl;
#endif
      return;
   }

   while (!(isEmpty()))
   {
#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems() removing an item of " 
           << "size = " << dec << pFront->getdataSize() << endl;
#endif
      pTemp = pFront->getNext();// save pointer to next item
      delete pFront;		// delete the Data Queue Item
      pFront = pTemp;		// point to the next Data Queue Item
   }

   // Be sure the pointer to the back of the queue points nowhere too
   pBack = (DataQueueItemObject *)NULL;
}


void DataQueueObject::removeAllItems(int type)
{
   size_t	size, removedItemSize;
   void *	pBuf;

#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems(" << dec << type << ")" << endl;
#endif

   if (isEmpty())
   {
#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems(" << dec << type
	   << "): Nothing to remove. Queue is empty!" << endl;
#endif
      return;
   }

   // while we can find an item of the input type, remove it from the queue
   while(0 != (size = getItemSize(type)))
   {
#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems(" << dec << type
	   << ") removing an item of size = " 
	   << size << endl;
#endif
      pBuf = malloc(size);
      removedItemSize = removeItem(type, pBuf, size);
      if (removedItemSize > size)
      {
         cout << " ** WARNING ** DataQueueObject::removeAllItems(" << dec 
	      << type << ") removed an item of " << "size = " 
	      << removedItemSize << " from the queue but only copyied " 
	      << size << " bytes, as requested, into the passed buffer" 
	      << endl;
      }
#ifdef DEBUG
      cout << " DataQueueObject::removeAllItems(" << dec << type
	   << ") removed an item of " << "size = " 
	   << size << endl;
#endif
      free(pBuf);
   } //endwhile
} // end removeAllItems()




//********************************************************************
DataQueueItemObject::DataQueueItemObject(const void * buf, size_t bSize)
{
   pNext = (DataQueueItemObject *)NULL;
   dataSize = bSize;
   pData = malloc(bSize);
   memcpy(pData, buf, bSize);
   dataType = DQO_NON_SPECIFIC_TYPE;
};


DataQueueItemObject::DataQueueItemObject(const void * buf, size_t bSize, 
                                         int type)
{
   pNext = (DataQueueItemObject *)NULL;
   dataSize = bSize;
   pData = malloc(bSize);
   memcpy(pData, buf, bSize);
   dataType = type;
};



size_t DataQueueItemObject::putData(void * buf, size_t bufSize)
{
   int size;
   
   if(bufSize < dataSize)
   {
      size = bufSize;
      cout << " WARNING: DataQueueItemObject::putData(" << hex << (void *)buf
           << ", " << dec << bufSize << "): passed buffer size is smaller "
           << "than the size of the item requested = " << dataSize << endl;
      cout << "   Only " << bufSize << " bytes will be moved to the passed "
           << "buffer. " << endl;
   }
   else
   {
#ifdef DEBUG
      if(bufSize > dataSize)
      {
         cout << " DataQueueItemObject::putData(" << hex << (void *)buf
              << ", " << dec << bufSize << "): passed buffer size is larger "
              << "than the size of the item requested = " << dataSize << endl;
         cout << "   only " << dataSize << " bytes will be copied to input "
	      << "buffer" << endl;
      }
#endif
      size = dataSize;
   }
   memcpy(buf, pData, size);
   return(dataSize);
}


