//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PicPidListObject.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module is the implementation of the Shopping List Object
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "PicPidListObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//
// Function Prototypes:
//
//
// ***************************************************************************
//
PicPidListObject::PicPidListObject()
{
#ifdef DEBUG
   cout << " PicPidListObject constructor\n";
#endif
   initList("PicPidList");
}

PicPidListObject::PicPidListObject(const char * n)
{
#ifdef DEBUG
   cout << " PicPidListObject constructor for name = " << n << endl;
#endif
   initList(n);
}

PicPidListObject::~PicPidListObject()
{
#ifdef DEBUG
   cout << " PicPidListObject destructor\n";
#endif
   freeAll();
}

void PicPidListObject::initList(const char * n)
{
   strncpy(name, n, sizeof(name));
   name[sizeof(name)-1] = '\0';	// ensure NULL terminated
   pHead = NULL;
   pCurPicture = NULL;	// set the current picture focus
   numberOfPics = 0;
   listIsBuilt = FALSE;
}

void PicPidListObject::addPicToList(const char * pic)
{
   PictureListItemObject * pPictureListItem;
#ifdef DEBUG
   cout << " PicPidListObject::addPicToList(" << pic << ")" << endl;
   cout << "   pHead = " << hex << (void *)pHead << endl;
#endif

   pPictureListItem = new PictureListItemObject(pic);
   if (NULL == pPictureListItem)
   {
      cout << " ***** ERROR: Can not get memory for " << pic << " structure"
	   << endl << " ***** EXITING *****" << endl;
      exit(-1);
   }
   pPictureListItem->setNext(pHead);	// set next pointer to start of list
   pHead = pPictureListItem;	// point start of list to this item
   pCurPicture = pPictureListItem;	// set current picture focus
   numberOfPics++;		// inc picture counter

#ifdef DEBUG
   cout << "   After NEW: pPicListItem, pPicListItem.Next, #Pics = "
	<< hex << (void *)pPictureListItem << ", " 
	<< (void *)pPictureListItem->getNext() << ", "
	<< dec << numberOfPics << endl;
   flush(cout);
#endif
}

void PicPidListObject::addPidToPicture(const char * Pid, const int Ipn,
				       const int id)
{
   PidListItemObject * pPidListItem;

#ifdef DEBUG
   cout << " PicPidListObject::addPidToPicture(" << Pid << ", " 
	<< dec << Ipn << ", " << id << ") for picture = " 
	<< pCurPicture->getPicName() << " " << hex << (void *)pCurPicture 
	<< endl;
#endif

   // create a new Pid List Item
   pPidListItem = new PidListItemObject(Pid, Ipn, id);
   if (NULL == pPidListItem)
   {
      cout << " ***** ERROR: Can not get memory for " << Pid << " structure"
	   << endl << " ***** EXITING *****" << endl;
      exit(-1);
   }
   // set its next pointer to start of Pid list
   pPidListItem->setNext(pCurPicture->getpPidList()); 
   // set the Pid List pointer of the current picture to the new Pid item
   pCurPicture->setpPidList(pPidListItem);
   // increment the number of Pids counter
   pCurPicture->setnumberOfPids((pCurPicture->getnumberOfPids())+1);

#ifdef DEBUG
   cout << "   After NEW: pCurPicture, pPidListItem, pPidListItem.Next, "
	<< "#Pids = "
	<< hex << (void *)pCurPicture << ", " << pPidListItem << ", "
	<< (void *)pPidListItem->getNext() << ", "
	<< dec << pCurPicture->getnumberOfPids() << endl;
   flush(cout);
#endif
}

void PicPidListObject::printPicList()
{
   PictureListItemObject * p;

   p = pHead;
   if ((PictureListItemObject *)NULL != p)
   {
      cout << "\n***** PicPidListObject::printPicList: *** "
	   << "Printing Picture List ***\n";
      while((PictureListItemObject *)NULL != p)
      {
	 cout << "   Picture Name " << hex << (void *)p << " = " 
	      << p->getPicName() << endl;
	 p = p->getNext();
      }
   }
   else
   {
      cout << "   No Pictures in List" << endl;
   }
   cout << endl;
   flush(cout);
}

void PicPidListObject::freeAll()
{
   PictureListItemObject * pPic;
   PictureListItemObject * pNextPic;
   PidListItemObject * pPid;
   PidListItemObject * pNextPid;

#ifdef DEBUG
   cout << "\n*** PicPidListObject::freeAll()\n";
#endif

   pPic = pHead;

#ifdef DEBUG
   cout << "   pHead = " << hex << (void *)pHead << endl;
#endif

   while ( (PictureListItemObject *)NULL != pPic)
   {

#ifdef DEBUG
      cout << "   Processing Picture " << hex << (void *)pPic 
	   << " " << pPic->getPicName() << endl;
#endif

      // free up all the objects in the Pid list
      pPid = pPic->getpPidList();

#ifdef DEBUG
      cout << "   Pointer to Pid List = " << hex << (void *)pPid << endl;
#endif

      while( (PidListItemObject *)NULL != pPid)
      {
#ifdef DEBUG
	 cout << "  Processing Pid " << hex << (void *)pPid 
	      << " " << pPid->getPidName() << endl;
#endif

	 pNextPid = pPid->getNext();

#ifdef DEBUG
      cout << "   About to delete Pid " << hex << (void *)pPid 
	   << " " << pPid->getPidName() << " Next Pid = " << (void *)pNextPid 
	   << endl;
#endif

	 delete pPid;
	 pPid = pNextPid;
      }

      // free up the Picture List Item
      pNextPic = pPic->getNext();

#ifdef DEBUG
      cout << "   About to delete Picture" << hex << (void *)pPic 
	   << " " << pPic->getPicName() << " Next Pic = " << (void *)pNextPic 
	   << endl;
#endif

      delete pPic;
      pPic = pNextPic;
   }
#ifdef DEBUG
   cout << " *** Finished PicPidListObject::freeAll() ***\n";
#endif
   pHead = NULL;
   pCurPicture = NULL;
}

int PicPidListObject::setPictureFocus(char * picName)
{
   int 	retval;
   char	picStr[45];
   
#ifdef DEBUG
   cout << " PicPidListObject::setPictureFocus(" << picName << ")" << endl;
#endif

   strcpy(picStr, picName);
   strcat(picStr, ".Tdoc");

   pCurPicture = pHead;
   while(NULL != pCurPicture)
   {
//      cout << " Picture in list is " << pCurPicture->getPicName() << endl;
      if(0 == strcmp(pCurPicture->getPicName(),picStr)) break;
      pCurPicture = pCurPicture->getNext(); // 
   }
   if(NULL != pCurPicture)
   {
#ifdef DEBUG
      cout << "   Picture " << picStr << " found at " << hex 
	   << (void *)pCurPicture << endl;
#endif
      retval = TRUE;
   }
   else
   {
#ifdef DEBUG
      cout << "   Picture " << picStr << "not found\n";
#endif
      retval = FALSE;
   }
   flush(cout);
   return retval;
}
