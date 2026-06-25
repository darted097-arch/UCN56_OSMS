//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// InterProcSMObject.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This file implements the InterProcSMObject, which contains all 
// shared memory that does not interfact with Picasso.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <stdlib.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "InterProcSMObject.h"
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

interProc * InterProcSMObject::pSM;

InterProcSMObject::InterProcSMObject(key_t key, int size, int mode) 
   : sharedMemoryObject(key, size, mode)
{
#ifdef DEBUG
   cout << " InterProcSMObject constructor\n";
   cout << " Address of InterProcSMObject in shared memory " << hex 
	<< (void *)getAddr() << endl;
#endif
   pSM = (interProc *)getAddr();
}

InterProcSMObject::~InterProcSMObject()
{
#ifdef DEBUG
   cout << " InterProcSMObject destructor\n";
#endif
}


void InterProcSMObject::setMODEMOption(const char * s)
{
   cout << "InterProcObject::setMODEMOption: setting MODEM option to " << s
        << endl;
   if (MODEM_OPTION_LEN >= strlen(s))
   {
      strcpy(pSM->MODEM, s);
   }
   else
   {
      cout << "InterProcObject::setMODEMOption: ERROR Length of modem string "
           << s << " is " << dec << strlen(s)
           << ", which exceeds maximum length of " << dec << MODEM_OPTION_LEN
           << endl;
      cout << "EXITING..." << endl;
      exit(127);
   }
}

