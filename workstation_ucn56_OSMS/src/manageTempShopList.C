//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// manageTempShopList.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module is a callback function that creates the Point Id temporary 
//  shopping list from Picasso pictures.		
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
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
//#include "messageObject.h"
#include "sharedMemoryObject.h"
#include "TempShoppingListObject.h"
//
// External Variables:
//

extern	TempShoppingListObject	TempShoppingList;
extern	messageQueueObject	WS_MsgQue;	// GUI to CommLink message queue object
//
// Local Constant Definitions:
//
#ifndef DEBUG
   #define DEBUG
#endif
//
// Function Prototypes:
//
#include "prototypes.h"			// prototypes
//
// ***************************************************************************
//
int32 manageTempShopList(int32 numArgs, void *data)
{
   int32 theSize;			// the size of the input
   int32 theType;			// the input variable type
   void *theData;			// parsed version of the data   
   int32 theOption;			// the submitted Option

  
   struct tm   *current;		// current date and time structure
   time_t	seconds;		// seconds since 1/1/70

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//   char 	dateAndTime[21];	// mm/dd/yy, hh:mm:ss - 
   char 	dateAndTime[32];	// mm/dd/yy, hh:mm:ss - 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Start of executable code.

   // get the current time for error reporting and also for the message header
   time(&seconds);
   current = localtime(&seconds);

   // Format the date and time for error messages
   sprintf(dateAndTime,"%2.2d/%2.2d/%2.2d, %2.2d:%2.2d:%2.2d - ",
	   current->tm_mon+1, current->tm_mday,current->tm_year, 
	   current->tm_hour, current->tm_min, current->tm_sec); 
   
   // Extract the data, type and size   
   theData = PfGetFuncArg(&data, &theType, &theSize);
   
   // if it's not an int, log it but continue
   if ( numArgs != 2)
   {

      cerr      << dateAndTime << " Bad Number of Arguments"
	        << "numArgs = " << dec << numArgs << endl;
   }
   else
   {
      if ( theType != PfCInt || theSize != 1)
      {

         cerr      << dateAndTime << " Bad Input to manageTempShopList"
	           << "theType = " << theType << " theSize = " << theSize << endl;
      }
   }

   // Extract the Option   
   theOption = *(int32*)theData;

#ifdef DEBUG   
   cout      << dateAndTime << "manageTempShopList: Processing " 
             << hex << theOption << endl;
#endif

   // >>>>>>>>>>>>>>>> Process Message According to Option  <<<<<<<<<<<<<<<<<<<<
  
   switch (theOption)
   {
      case INIT_TEMP_SHOP_LIST:
	 // initializes temporary shopping list on W/S and CCUR
	 TempShoppingList.init();
	 
    	break;
		
      case WSINIT_TEMP_SHOP_LIST:
	 // Zero out the number of temporary shopping list items
	  TempShoppingList.wsinit();
	  break;
	 	 
      case TEMP_SHOP_LIST_ADD:

	 // adds one point-id to temporary shopping list message on W/S
	 
	 theData = PfGetFuncArg(&data, &theType, &theSize);
   	 if ( ( (theType != PfCChar) && (theType != PfCUnsignedChar) ) || (strlen((char*)theData) > 8))
      		cout << "Bad Input to manageTempShopList" << endl;
   
	 // add the point to the temporary shopping list
	 TempShoppingList.add((char *)theData);	
 	 break;
	 
      case TEMP_SHOP_LIST_SEND:
	 // send the temporary shopping list to the host
	 TempShoppingList.send();
         break; 

      case TEMP_SHOP_INIT_ADD:
	 // initialize the shoppint
	 TempShoppingList.init();
	 
	 // get point-id
	 theData = PfGetFuncArg(&data, &theType, &theSize);
   	 if ( ( (theType != PfCChar) && (theType != PfCUnsignedChar) ) || (strlen((char*)theData) > 8))
      	    cout << "Bad Input to manageTempShopList" << endl;
	 
	 // add the point to the temporary shopping list
	 TempShoppingList.add((char *)theData);
	 
	 // send the shopping list to the host
	 TempShoppingList.send();
	 break;
	
      default:
	 cout << "manageTempShopList Received Unknown Option: " << hex << theOption << endl;
	 return (!OK);
	 
      } // end of Option switch statement
      
   return (OK);

}//end manageTempShopList



