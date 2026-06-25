#ifndef _INT_LIST_OBJECT
#define _INT_LIST_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// IntListObject.h      
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the Gemeric Shopping List object.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
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
//

class IntListObject
{
  public:
     		IntListObject(size_t maxItems);
     		IntListObject(const char * objectName, size_t maxItems);
    	       ~IntListObject();

     void	incNumberOfItems(){numberOfItems++;};
     void	setNumberOfItems(int n){numberOfItems = n;};
     void	setItem(int indx, int val){pArrayOfItems[indx] = val;};
     void	setListIsBuilt(){isBuilt = TRUE;};
     
     int	getNumberOfItems(){return(numberOfItems);};
     int	getItem(int indx){return(pArrayOfItems[indx]);};
     char *	getName(){return name;};	// return name of list
     int	isListBuilt(){return(isBuilt);};
     int *	getListAddr(){return(pArrayOfItems);};

  private:
     int	numberOfItems;		// number of points in list
     int *	pArrayOfItems;		// pointer to array of Picasso Ids

     int	isBuilt;		// TRUE if filling of list is complete
     int	maxNumberOfItems;	// Max number of items in list
     char	name[32];		// user given name of object

     int * 	createList(size_t maxItems);	// allocate memory, etc.
     void	initList(const char * n, size_t maxItems);

};
#endif

