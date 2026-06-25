#ifndef _PERM_SHOPPING_LIST_OBJECT
#define _PERM_SHOPPING_LIST_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PermShoppingListObject.h
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
#include "IntListObject.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
//

class PermShoppingListObject : public IntListObject
{
  public:
     		PermShoppingListObject(size_t maxItems);
     		PermShoppingListObject(const char *objectName, size_t maxItems);
    	       ~PermShoppingListObject();
	       
	void	buildPermanentShoppingList(const char * picName);
     
  private:
	void	addPid(char * Pid);

	int	PidCtr;
};
#endif

