#ifndef _PIC_PID_LIST_OBJECT
#define _PIC_PID_LIST_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PicPidListObject.h   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the object that contains a list of pictures and Point Ids
// contained in each picture.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
//
// User Defined Include Files:
//
#include "PictureListItemObject.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
//
// Local Structure Definitions

class PicPidListObject
{
    public:
        PicPidListObject();
        PicPidListObject(const char * objectName);
        ~PicPidListObject();

	void	createPicturePidLists();
	void	buildPicPidList(const char * picFile);
	void	initList(const char * nameOfList);
	void	printPicList();
	void	resetPidFocus(){pCurPicture->resetPidFocus();};

      	// adds an item to the list and returns to pointer to the head of the 
      	//list
      	void	addPicToList(const char * Pic);
	void	addPidToPicture(const char * Pid, const int Ipn,
				const int id);
      	void	freeAll();	// frees all objects in the list

	void	setPicPidListIsBuilt(){listIsBuilt = TRUE;};

        // sets pCurPicture to point to the PictureListItem for the input
	//picture or NULL if the picture is not in the list.  Returns 
	//TRUE if picture is in the list, FALSE otherwise.
	int	setPictureFocus(char * picName); // sets pCurPicture

	// sets focus to next Pid in list. returns SUCCESS if there is 
	//a next Pid to focus on, FAIL otherwise
	int	setNextPidFocus(){return pCurPicture->setNextPidFocus();};

	int	isPicPidListBuilt(){return listIsBuilt;};

	// returns number of Pictures in list
      	int	getNumberOfPics(){return numberOfPics;};
      	
	char *	getName(){return name;};// return name of object
	
	// return Picasso Id of the current focused Pid of the current 
	//focused picture.
	int	getCurIpn(){return pCurPicture->getCurIpn();};	
	int	getCurPicassoId(){return pCurPicture->getCurPicassoId();};	
	char *	getCurPointName(){return pCurPicture->getCurPointName();};

  private:
	int	listIsBuilt;		// TRUE if Picture PID list is built
	PictureListItemObject *	pHead;	// pointer to head of list of Pictures
	PictureListItemObject * pCurPicture; // pointer to current picture item
     	int	numberOfPics;		// number of pictures in this list
     	char	name[32];		// name of the list
};
#endif

