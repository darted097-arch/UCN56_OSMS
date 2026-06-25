//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// mainInterfaceToGUI.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module to links the shared memory region of the database
//  to the Picasso GUI
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "database.h"			// database variables
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "sharedMemoryObject.h"
#include "DatabaseObject.h"
#include "PermShoppingListObject.h"
#include "TempShoppingListObject.h"
#include "PicPidListObject.h"
//
// Global Variables:
//
//CYCLIC		Cyclic;		// Cyclic/control variables local to this task
// Global Permanent Shopping List
PermShoppingListObject	PermShoppingList("PSLO", NUM_REC);
TempShoppingListObject	TempShoppingList("TSLO", NUM_TEMP);
PicPidListObject	PicPidList;

INTERFACE	*Interface;	// shm/CCUR to GUI interface variables

messageQueueObject IGUI_MsgQue(COMM2GUI_KEY, FALSE);	// CommLink to GUI message queue
messageQueueObject WS_MsgQue(WS_MESSAGE_KEY, FALSE);	// GUI to CommLink message queue

// Create Global shared memory objects
DatabaseObject 	             DBO(DATABASE_KEY,(NUM_REC+1)*sizeof(RECORD),0);
sharedMemoryObject InterfaceSMO(INTERFACE_KEY,sizeof(INTERFACE),0);

char curPicture[40] = {'\0'};	// name of currently displayed picture

// Local Constant Definitions: NONE
//
//
// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//          
int main(int argc, char *argv[])
{
   char	systemName[LEN_SYSTEMNAME];	// identifies the hostname

   cout << "mainInterfaceGUI START !!!!!!! " <<  endl;

   // Assign global shared memory pointer
   Interface = (INTERFACE *) InterfaceSMO.getAddr();

   // see if the user provided a system name
   if (1 < argc) 
   {
      strcpy (systemName,argv[1]);
      #ifdef DEBUG
         cout << "Command Line SystemName: " << systemName << endl;
      #endif
   }
   else
   {
      gethostname (systemName,LEN_SYSTEMNAME);
      #ifdef DEBUG
         cout << "Default SystemName: " << systemName << endl;
      #endif
   }

   // various initializations
   initInterfaceToGUI();
   
   // Connect the application to the rtm
   PfInitialize (APP_NAME, "InterfaceToGUI", RTM_NAME, systemName, CACHE_SIZE, TRUE,
		  connectToGUI, disconnectFromGUI);
   if (apiError != OK)
   {
      cout << "Could not initialize API..." << endl;
      PfPrintSystemError(apiError);
   }
   else
      cout << "Initialized API with the rtm" << endl;

   // Initialize the cyclic process handler
   PfSetProcessHandler(cyclicProcessing, UPDATE_RATE);

   // This loop queues the cyclic process handler
   if (PfMainLoop() != OK)
   {
      cout << "Could not start Pf loop..." << endl;
      PfPrintSystemError(apiError);
   }

   if (PfClose() != OK)
   {
      cout << "Could not close Pf..." << endl;
      PfPrintSystemError(apiError);
   }
   cout << "Exit PicassoGlue" << cout;

}

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
void swap32(char * pval)
{
char tmp;
        tmp = *(pval+3);
        *(pval+3) = *(pval);
        *(pval) = tmp;
        tmp = *(pval+2);
        *(pval+2) = *(pval+1);
        *(pval+1) = tmp;
}
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



