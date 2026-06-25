#ifndef _TEMP_SHOPPING_LIST_OBJECT
#define _TEMP_SHOPPING_LIST_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// TempShoppingListObject.h
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the Temporary Shopping List object that handles Temporary 
//  Shopping List operations.
// ***************************************************************************
//
// ***************************************************************************
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "IntListObject.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
//

class TempShoppingListObject : public IntListObject
{
  public:
     		TempShoppingListObject(size_t maxItems);
     		TempShoppingListObject(const char * objectName, size_t maxItems);
    	       ~TempShoppingListObject();

     
     void 	init();			// init list to empty
     void	add(const char * Pid);	// add Pid to list
     void	send();			// send message to CommLink
     void	create();		// creates list from user requests
     void       wsinit();               // Zeros out number of items on temp shopping list

  private:
     // Ipns of temp update vars (dimension - NUM_TEMP)
     int	arrayOfTempIpns[NUM_TEMP];
};
#endif

