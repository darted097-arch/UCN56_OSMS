//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// TempShoppingListObject.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module is the implementation of the Temporary Shopping List Object
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
//#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "database.h"			// database structure
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		// message queue
#include "messageObject.h"
#include "sharedMemoryObject.h"
#include "TempShoppingListObject.h"
#include "DatabaseObject.h"
#include "IntListObject.h"
//
// External Variables:
//
extern	messageQueueObject	WS_MsgQue;	// GUI to CommLink message queue object
//
// Local Constant Definitions:
//
//#ifndef DEBUG
//   #define DEBUG
//#endif
//
// Function Prototypes:
//
#include "prototypes.h"			// prototypes
//
// ***************************************************************************
//
TempShoppingListObject::TempShoppingListObject(size_t maxItems)
   : IntListObject("NoName", maxItems)
{
#ifdef DEBUG
   cout << " TempShoppingListObject constructor\n";
#endif
}

TempShoppingListObject::TempShoppingListObject(const char * n, size_t maxItems)
   : IntListObject(n, maxItems)
{
#ifdef DEBUG
   cout << " TempShoppingListObject constructor for name = " << n << endl;
#endif
}

TempShoppingListObject::~TempShoppingListObject()
{
#ifdef DEBUG
   cout << " TempShoppingListObject destructor\n";
#endif
}

void TempShoppingListObject::init()
{
   extern messageQueueObject	WS_MsgQue;

   int		 status;
   messageObject ShoppingListMsg(TO_HOST_MSG_ID, "initTempShoppingListMsg");
   
   // initialize the number of Picasso points to update
   setNumberOfItems(0);

   // set the time, date, opcode, and numberOfSomething in the message header
   ShoppingListMsg.setHeader(INIT_TEMP_SHOP_LIST, 0);
	
   // send message to CCUR to init the temp shopping list
   status = WS_MsgQue.sendMessage(ShoppingListMsg.pMsg,
				  ShoppingListMsg.getMsgSize(),
				  ShoppingListMsg.getType());

   if (0 != status)
   {
      cout << " TempShoppingListObject::init(): "
	   << "ERROR putting message on message queue\n";

   }
   else
   {
#ifdef DEBUG
      cout << " TempShoppingListObject::init(): successfully put a "
           << dec << ShoppingListMsg.getMsgSize() << " byte "
	   << "INIT_TEMP_SHOP_LIST  message on the WS_MsgQue. Opcode = " 
	   << hex << ShoppingListMsg.getOpcode() << endl;
#endif
   }

}//end init

void TempShoppingListObject::wsinit()
{
   extern messageQueueObject	WS_MsgQue;


   // initialize the number of Picasso points to update
   setNumberOfItems(0);
}// end wsinit

void TempShoppingListObject::add(const char * Pid)
{
   extern DatabaseObject	DBO;		// shared memory database
   int				ipn;		// find_pid ipn
   char				thePointId[12];	// the point to search for

   // copy the PID into an array
   strcpy(thePointId,Pid);
   
#ifdef DEBUG   
   cout << " manageTempShopList: thePointId = " << thePointId << endl;
#endif
   cout << " manageTempShopList: thePointId = " << thePointId << endl;
   
   if ( (ipn = find_pid(thePointId)) > 0 )
      {
	 if (getNumberOfItems() >= NUM_TEMP)
	 {
	    cout     << " Temp Shopping List Exceeds " << dec << NUM_TEMP 
	             << "entries" << endl;	      
	 }
	 else
	 {    
#ifdef DEBUG
	    cout << DBO.getpid(ipn) << " (Ipn: " << dec << ipn  
	         << ") Inserted into Temp Update List as Picasso Id: " << dec  
	         << DBO.getiPicassoId(ipn) << endl;
#endif
	    // Put point into the Picasso Update array and Ipn array
	    setItem(getNumberOfItems(), DBO.getiPicassoId(ipn));
	    arrayOfTempIpns[getNumberOfItems()] = ipn;
	    incNumberOfItems();

	    if(0 != DBO.getlIPN(ipn))
	    {
	       setItem(getNumberOfItems(), 
		       DBO.getiPicassoId(DBO.getlIPN(ipn)));
	       arrayOfTempIpns[getNumberOfItems()] = DBO.getlIPN(ipn);
	       incNumberOfItems();
	    }

	    if(0 != DBO.gethIPN(ipn))
	    {
	       setItem(getNumberOfItems(), 
		       DBO.getiPicassoId(DBO.gethIPN(ipn)));
	       arrayOfTempIpns[getNumberOfItems()] = DBO.gethIPN(ipn);
	       incNumberOfItems();
	    }

	 }//endelse
      }//endif
      else
      {
         // if point id is not found, then TROUBLE; 
	 // output to error device, log file, and error file
	 cout      << " TempShoppingListObject::add() ERROR: " 
	           << thePointId << " Not Found in database" << endl;

      }//endelse

}// end add



void TempShoppingListObject::send()
{
   extern messageQueueObject	WS_MsgQue;
   
   int		 i;
   int		 status;
   shoppingData	 tempShoppingList;
   messageObject ShoppingListMsg(TO_HOST_MSG_ID, "sendTempShoppingListMsg");

   cout << "+++++++++++++++ TempShoppingListObject::send()" << endl;
   
   // get the global shopping list of IPNs to the local buffer
   for (i = 0; i < getNumberOfItems(); i++)
   {
      tempShoppingList.theIPN[i] = (short)arrayOfTempIpns[i];
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
      tempShoppingList.theIPN[i] = ntohs(tempShoppingList.theIPN[i]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   }
	 
   // set the time, date, opcode, and numberOfSomething in the message header
   ShoppingListMsg.setHeader(TEMP_SHOP_LIST_SEND, getNumberOfItems());
   ShoppingListMsg.setData(&(tempShoppingList.theIPN[0]), 
		   sizeof(tempShoppingList.theIPN[0]) * getNumberOfItems());
	
   // send message to CCUR
   status = WS_MsgQue.sendMessage(ShoppingListMsg.pMsg,
			          ShoppingListMsg.getMsgSize(),
				  ShoppingListMsg.getType());
   if (0 != status)
   {
      cout      << " TempShoppingListObject::send(): "
                << " ERROR putting message on message queue\n";

   }
   else
   {
      cout << " TempShoppingListObject::send(): successfully put a "
           << dec << ShoppingListMsg.getMsgSize() << " byte "
	   << "TEMP_SHOP_LIST_SEND  message on the WS_MsgQue. Opcode = " 
	   << hex << ShoppingListMsg.getOpcode() << endl;
   }
 	
}//end send


void TempShoppingListObject::create()
{
   int		status;
   
#ifdef DEBUG   
   struct tm   *current;		// current date and time structure
   time_t	seconds;		// seconds since 1/1/70
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//   char       dateAndTime[21];        // mm/dd/yy, hh:mm:ss - 
   char         dateAndTime[32];        // mm/dd/yy, hh:mm:ss - 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#endif
   
   messageObject	Msg(TO_HOST_MSG_ID, "createTempShopListMsg");
   

// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Start of executable code.
#ifdef DEBUG
   // get the current time for error reporting and also for the message header
   time(&seconds);
   current = localtime(&seconds);

   // Format the date and time for error messages
   sprintf(dateAndTime,"%2.2d/%2.2d/%2.2d, %2.2d:%2.2d:%2.2d - ",
           current->tm_mon+1, current->tm_mday,current->tm_year, 
	   current->tm_hour, current->tm_min, current->tm_sec); 
      
   cout     << dateAndTime << " TempShoppingListObject::create() Processing " 
            << endl;
#endif
 
   if (getNumberOfItems() > 0)
   {   
      // set message to tell host to init list
      Msg.setHeader(INIT_TEMP_SHOP_LIST, 0);
      
      // send message to CCUR 
      status = WS_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(),
				     Msg.getType());
      
      if (0 != status)
      {
         cout << " TempShoppingListObject::create(): "
	      << "ERROR putting message on message queue\n";

      }
      else
      {
         cout << " TempShoppingListObject::create(): successfully put a "
              << dec << Msg.getMsgSize() << " byte " 
	      << "INIT_TEMP_SHOP_LIST message on the WS_MsgQue. Opcode = " 
	      << hex << Msg.getOpcode() << endl;
      }

      // tell CommLink to send the shopping list to the host
      send();
      
   } //endif
   else
   {
      cout << " TempShoppingListObject::create() called but ";
      cout << "numberOfItems <= 0. No points on list\n";
   }
   
}//end create()
