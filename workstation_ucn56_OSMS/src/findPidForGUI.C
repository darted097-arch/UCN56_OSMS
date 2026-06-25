//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// findPidForGUI.C      
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module searches shared memory for the inputted Point Id.
//			
//   Note, it would be easy to use the Picasso statement:
//   ipn = findPidForGUI(PointId) and use the result ipn.  This works
//   for normal use.  In Picasso, use the execute statement:
//   execute ("{findPidForGUI(\"%s\");}",PointId);
//   This allows the code to compile even when the routine has not
//   been registered with the rtm.  You cannot use the statement:
//   execute ("{ipn=findPidForGUI(\"%s\");}",PointId);
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// databaseParameters	
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes:
//
int find_pid (char *pid);		// find point id
//
// ***************************************************************************
//
int32 findPidForGUI(int32 numArgs, void *data)
{

   int32 theResult;		// find_pid result
   int32 theSize;		// the size of the input
   int32 theType;		// the input variable type
   void *theData[4];		// parsed version of the data   
   char *thePointId;		// the point to search for
   char *scopeString;           // execution scope of good and bad functions
   char *goodFunc;              // function to execute if point id is good
   char *badFunc;               // function to execute if point id is good
   int argCounter;              // index of current argument being checked
   int badArg;                  // flag set TRUE if error found in args
   
   cout << "findPointIdForGUI = " << hex << numArgs << endl;
   
   // Update System/Interface variables
   //PfUseField(Interface->System.PicassoId);
   //PfFlushUseFields();
   //PfSend(Interface->System.PicassoId);
   //PfFlush();
   
   badArg = FALSE;
   if ( numArgs != 4 )
   {
      cout << "Bad Number of Arguments = " << dec << numArgs << endl;
      badArg = TRUE;
   }
   else
   {
      for (argCounter=0; argCounter<numArgs; argCounter++)
      {
         theData[argCounter] = PfGetFuncArg(&data, &theType, &theSize);
         if ( (theType != PfCChar) && (theType != PfCUnsignedChar) )
         {
            cout << "Bad Input to findPidForGUI" << endl;
            badArg = TRUE;
         }
#ifdef DEBUG
         else
         {
	    cout << "findPidForGUI(): argument " << dec << argCounter 
	    << " = " << (char *)theData[argCounter] << endl;

         }
#endif
      } //endfor
   }
   
   if (!badArg)
   {
      thePointId = (char *)theData[0];
      scopeString = (char *)theData[1];
      goodFunc = (char *)theData[2];
      badFunc = (char *)theData[3];
   
      cout << "findPidForGUI: thePointId = " << thePointId << endl;
   
      theResult = find_pid(thePointId);
   
      cout << "theResult = " << dec << theResult << endl;
   
      // set up the pTALK callback function
      PfExecute(NULL,"{FindPidResult = %d; }", theResult);
      if (theResult >=0)
         PfExecute(scopeString,"{%s; }",goodFunc);
      else
         PfExecute(scopeString,"{%s; }",badFunc);
   }

   return (OK);
}
