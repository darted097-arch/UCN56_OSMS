//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PermShoppingListObject.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module is the implementation of the Permanent Shopping List Object
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
#include "IntListObject.h"
#include "PermShoppingListObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//#ifndef DEBUG
//   #define DEBUG
//#endif
//
// Function Prototypes:
//
//
// ***************************************************************************
//
PermShoppingListObject::PermShoppingListObject(size_t maxItems)
   : IntListObject(maxItems)
{
#ifdef DEBUG
   cout << " PermShoppingListObject constructor\n";
#endif
}

PermShoppingListObject::PermShoppingListObject(const char * n, size_t maxItems)
   : IntListObject(n, maxItems)
{
#ifdef DEBUG
   cout << " PermShoppingListObject constructor for name = " << n << endl;
#endif
}

PermShoppingListObject::~PermShoppingListObject()
{
#ifdef DEBUG
   cout << " PermShoppingListObject destructor\n";
#endif
}
