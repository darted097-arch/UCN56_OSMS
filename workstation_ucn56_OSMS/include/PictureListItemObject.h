#ifndef _PIC_LIST_ITEM_OBJECT
#define _PIC_LIST_ITEM_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PictureListItemObject.h
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object contains defines the Picture List Item 
// ***************************************************************************
//
// ***************************************************************************
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "PidListItemObject.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
//
// Local Structure Definitions

class PictureListItemObject
{
    public:
	PictureListItemObject(const char * picName);
	~PictureListItemObject();

	void	resetPidFocus(){pPidFocus = NULL;};

	void	setpPidList(PidListItemObject * p){pPidList = p;};
	void	setNext(PictureListItemObject * p){Next = p;};
	void	setnumberOfPids(const int n){numberOfPids = n;};
	int	setNextPidFocus()
	{
	   int retval;
	   
	   // if not PIDs in list, return FAIL
	   if(NULL == pPidList) retval = FAIL;
           else
	   {
	      // if starting at NULL, set to start of Pid List
	      if(NULL == pPidFocus)
	      {
	         pPidFocus = pPidList;
	         retval = SUCCESS;
	      }
	      else
	      // if there is a next Pid, return SUCCESS, otherwise return FAIL
	      {
	         if(NULL != (pPidFocus = pPidFocus->getNext())) 
		 {
		    retval = SUCCESS;
	         }
		 else retval = FAIL;
	      } // endelse there is a next PID on the list
	   } //endelse there are PIDs in the list
	   return retval;
	};

	char * 			getPicName(){return PicName;};
	PidListItemObject * 	getpPidList(){return pPidList;};
	PictureListItemObject *	getNext(){return Next;};
	int			getnumberOfPids(){return numberOfPids;};

	int			getCurIpn()
	{
	   if(NULL != pPidFocus) return pPidFocus->getIpn();
	   else 		 return FAIL;
	};

	int			getCurPicassoId()
	{
	   if(NULL != pPidFocus) return pPidFocus->getPicassoId();
	   else 		 return FAIL;
	};

	char *			getCurPointName()
	{
	   if(NULL != pPidFocus) return pPidFocus->getPidName();
	   else			 return FAIL;
	}

    private:
	void 	setPicName(const char * n)
	{
	    strncpy(PicName, n, sizeof(PicName)-1);
	};

    	char		PicName[100];	
    	PidListItemObject *	pPidList;	// pointer to Pid list
	PidListItemObject *	pPidFocus; // pointer to a Pid item in list
    	PictureListItemObject *	Next;	// pointer to next PicListItem
	int numberOfPids;
};
#endif

