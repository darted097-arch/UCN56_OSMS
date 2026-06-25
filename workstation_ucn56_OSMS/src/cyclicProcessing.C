//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// cyclicProcessing.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module runs each cycle and updates the database
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>		// picasso specific
#include <iostream.h>
#include <time.h>		// For time settings
#include <stdlib.h>		// DEBUG
#include <string.h>		// for testing
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "DatabaseObject.h"
#include "PermShoppingListObject.h"
#include "TempShoppingListObject.h"
#include "PicPidListObject.h"
#include "messageObject.h"
#include "messageQueueObject.h"
#include "DataQueueObject.h"
//
// External Variables:
//
// Global Permanent Shopping List
extern	PermShoppingListObject	PermShoppingList;
extern	TempShoppingListObject	TempShoppingList;
extern	INTERFACE	*Interface;	// Structure of all shm to GUI variables
extern	PicPidListObject	PicPidList; // List of pictures with 
					    //permanent Point IDs
extern char curPicture[];		// name of currently displayed picture
extern messageQueueObject     IGUI_MsgQue;    // Messages to InterfaceToGUI
//
// Create Global shared memory objects
extern  DatabaseObject   DBO;
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
#include "prototypes.h"			// all function prototypes
static int pictureIsDifferent(const char * s1, const char * s2);
static void strip(char * s);
static int oldNumberOfUpdates;		// for debug output
//
static int displayedUnitsIpn = -1;     	// find_pid("CXYKMETD")

static char prevPicture[40] = {'\0'};

static DataQueueObject	DQ;
unsigned int msg_buf[WS_MESSAGE_LENGTH/sizeof(int)]; // a buffer for a message
//
// ***************************************************************************
//
int cyclicProcessing(int id)
{   
   time_t  bintime;
   struct  timeval first,second, lapsed;  	// for debug timing purposes
   struct  timezone tzp;			// for debug timing purposes
   
   int     numberOfUpdates;			// for debug output
   size_t  len;
   int	   isRcsMassFlow = 0;			// ujson add
   
   // >>>>>>>>>>>>>> Start Code:
//	printf("cyclicProcessing() start ---> \n");
   // Verify a valid id.  If -1, then error
   if (-1 == id)
   {
      cout << " Error in cyclicProcessing id = " << hex << id << endl;
      return FAIL;
   }
   
   if (TRUE != PicPidList.isPicPidListBuilt())
   {
   	cout << " cyclicProcessing: Skipping Update" << endl;
	oldNumberOfUpdates = -1; 		// initialize for debug output
	Interface->System.wsState &= ~(SHOP_LISTS_BUILT_WS_STATE);
   	return (OK);
   }
   Interface->System.wsState |= SHOP_LISTS_BUILT_WS_STATE;

   while ((len = IGUI_MsgQue.receiveMessage(&msg_buf, NOWAIT,
                                          FROM_HOST_MSG_ID) ) != -1)
   {
      if (sizeof(msg_buf) < len)
      {
         cout << " ***** ERROR: cyclicProcessing(): Message from message "
              << "queue is larger than buffer provided for it" << endl;
      }
      DQ.addItem(&msg_buf, len, DQO_WS_MESSAGE_TYPE);
   }

   if (-1 == displayedUnitsIpn )
   { 
      displayedUnitsIpn = find_pid("CXYKMETD");	// CORENET 2013.09
      //displayedUnitsIpn = find_pid("METDISP");	

      cout << "     IPN for METDISP is " << dec << displayedUnitsIpn << endl;
      if(-1 == displayedUnitsIpn)
      {
         cout << " ***** FATAL ERROR ***** Required IPN (METDISP) not found.  EXITING..."
	      << endl;
         flush(cout);
	 exit(-1); 
      }
      cout << "      Value of METDIS is " << dec << (int)DBO.getvalue(displayedUnitsIpn) << endl;

      //201027_jhp
      int i;
      for(i = 0; i < NUM_REC; i++)
      {
         DBO.setnewsta(i,0);
      }
   }
   if (-1 != displayedUnitsIpn)
   {
      //Interface->System.displayedUnits = (int)DBO.getvalue(displayedUnitsIpn);	// for Y34 (CORENET 2013.10.30)
      Interface->System.displayedUnits = 1;
   }
   else
   {
      Interface->System.displayedUnits = 0;	// default to english if PID not found
   }
   
   // if picture has changes, rebuild the Permanent shopping list
   if(pictureIsDifferent(curPicture, prevPicture))
   {
//#ifdef DEBUG
      cout << " *** cyclicProcessing detected picture change\n";
      cout << "   Current Picture, Previous Picture = " 
	   << curPicture << ", " << prevPicture  << endl;
      flush(cout);
//#endif
      strcpy(prevPicture, curPicture);

      strip(curPicture);	// strip out all but picture name

      isRcsMassFlow = strcmp(curPicture, "PicUCN_RCS_MASS_FLOW"); // ujson add

      cout << "isRcsMassFlow = " << dec << isRcsMassFlow << "  " << curPicture << endl;

      if (isRcsMassFlow != 0)
      	PermShoppingList.buildPermanentShoppingList(curPicture);
   } // endif picture changed
   flush(cout);


   // For debug timing purposes
   gettimeofday(&first,&tzp);
   
   if(TRUE == Interface->System.connected)
   {
      time(&bintime);   
      strftime(Interface->System.timeString,12,"%X",localtime(&bintime));
      // ujson 2016.11.15
      //strftime(Interface->System.dateString,12,"%x",localtime(&bintime));
      strftime(Interface->System.dateString,12,"%m/%d/%Y",localtime(&bintime));
   }

   // Update System/Interface variables
   PfUpdateValueId(Interface->System.PicassoId);

   // Update the array of temporary update variables
   if(TempShoppingList.getNumberOfItems() > 0)
   {
      PfUpdateValuesId(TempShoppingList.getListAddr(),
		       TempShoppingList.getNumberOfItems());
   }


   // Update the array of permanent update variables
   PfUpdateValuesId(PermShoppingList.getListAddr(), PermShoppingList.getNumberOfItems());
      
   PfFlush();

   PfExecute(NULL,"{ClearDisplayBlock();}");

   // Process any messages for the pictures
   parsePictureMessages(DQ);
	
   // Debug time calls
   gettimeofday(&second, &tzp);
 
   if (first.tv_usec > second.tv_usec)
   {
      second.tv_usec += 1000000;
      second.tv_sec--;
   }

   lapsed.tv_usec = second.tv_usec - first.tv_usec;
   lapsed.tv_sec = second.tv_sec - first.tv_sec;
   
   numberOfUpdates = PermShoppingList.getNumberOfItems() +
      		     TempShoppingList.getNumberOfItems();
        
   // output timing information if lapsed time exceeds LAPSED_TIME_INTERVAL or
   // if number of points updated changed
      
   if(lapsed.tv_sec > LAPSED_TIME_INTERVAL  || 
      numberOfUpdates != oldNumberOfUpdates)
   {   
      cout << Interface->System.dateString << ", " 
	   << Interface->System.timeString
           << " Lapsed time for " << dec << numberOfUpdates
           << " records is " 
           << dec << lapsed.tv_sec << " seconds and " 
	   << dec << lapsed.tv_usec << " microseconds " << endl;
#ifdef DEBUG
      cout << " number of temp updates is " << dec 
	   << TempShoppingList.getNumberOfItems() << endl;
      cout << " number of permanant updates is " << dec 
           << PermShoppingList.getNumberOfItems() << endl;
#endif
   }
   oldNumberOfUpdates = numberOfUpdates;
   return OK;
}


static int pictureIsDifferent(const char * pic1, const char * pic2)
{
   char		p1[40];
   char		p2[40];

   // get the input into local arrays
   if (40 < strlen(pic1))
   {
      cout << " ***** ERROR in cyclicProcessing: Picture name too long: "
	   << pic1 << endl;
      exit(1);
   }
   if (40 < strlen(pic2))
   {
      cout << " ***** ERROR in cyclicProcessing: Picture name too long: "
	   << pic2 << endl;
      exit(1);
   }
   strcpy(p1, pic1);
   strcpy(p2, pic2);
   strip(p1);
   strip(p2);
   return(strcmp(p1, p2));
}

static void strip(char * s)
{
   char * 	pStr;
   // get past path info
   while(pStr = strstr(s, "/"))	// while we keep finding '/' characters
   {
      pStr++;	// past the '/' character
      strcpy(s, pStr);	// copy the shortened string back into the original
   }
   pStr = strstr(s, ".");	// look for the '.' character
   if(pStr) *pStr = '\0';	// end the string after the file name
}

