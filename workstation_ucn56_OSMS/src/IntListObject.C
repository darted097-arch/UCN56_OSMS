//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// IntListObject.C      
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
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
#include "IntListObject.h"
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
IntListObject::IntListObject(size_t maxItems)
{
#ifdef DEBUG
   cout << " IntListObject constructor\n";
#endif
   initList("IntList", maxItems);
}

IntListObject::IntListObject(const char * n, size_t maxItems)
{
#ifdef DEBUG
   cout << " IntListObject constructor for name = " << n << endl;
#endif
   initList(n, maxItems);
}

IntListObject::~IntListObject()
{
#ifdef DEBUG
   cout << " IntListObject destructor\n";
#endif
   free(pArrayOfItems);
   pArrayOfItems = NULL;
}

int * IntListObject::createList(size_t maxItems)
{
   maxNumberOfItems = (int)maxItems;
   pArrayOfItems = (int *)calloc(maxItems, (size_t)sizeof(int));
   return(pArrayOfItems);
}

void IntListObject::initList(const char * n, size_t maxItems)
{
   strncpy(name, n, sizeof(name));
   if (NULL == (createList(maxItems)))
   {
      cout << " IntListObject: Object " << name << " could not allocate "
	   << "memory.  EXITING...\n";
      exit(1);
   }
#ifdef DEBUG
   else
      cout << " IntListObject: Object " << name << " allocated memory at "
	   << hex << (void *)pArrayOfItems << endl;
#endif
   isBuilt = FALSE;
}
