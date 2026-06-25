//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// buildPermanentShoppingList.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module builds the Permanent Shopping List
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <string.h>		// 

#ifdef LINUX_APP
#include <netdb.h>	// CORENET 2012.07.09
#include <netinet/in.h>	// CORENET 2012.07.09
#endif
//
// User Defined Include Files:
//
#include "interface.h"			// interface variables
#include "DatabaseObject.h"
#include "PermShoppingListObject.h"
#include "PicPidListObject.h"
#include "messageQueueObject.h"
#include "messageObject.h"
#include "messages.h"
//
// External Variables:
//
extern	INTERFACE *	Interface; // Structure of all shm to GUI variables
extern	PicPidListObject	PicPidList; // List of pictures with 
					    //permanent Point IDs
extern  DatabaseObject   	DBO;
extern	messageQueueObject	WS_MsgQue;	// CommLink message queue

// Local Constant Definitions: NONE
//
// Function Prototypes: NONE
//
#include "prototypes.h"			// all function prototypes
//
// ***************************************************************************
//

static int	startIndex = 0;
static int	startPidCtr = 0;
static int	startNumberOfIpns = 0;
static int	numberOfIpns = 0;

static shoppingData	ShoppingListMessage;

const int NUM_PICTURES_TO_UPDATE = 4;
const int NEW_PICTURE_INDEX = NUM_PICTURES_TO_UPDATE - 1;

static char picturesToUpdate[NUM_PICTURES_TO_UPDATE][40] =
{
   "PicUCN_header",
   "PicUCN_footer",
   "Pic220_CoreHeatRemoval"
   ""
};

const int NUM_PERMANENT_PIDS = 2;
static char permanentPidList[NUM_PERMANENT_PIDS][12] =
{
   //"CXYKMETD",
   "METDISP", // for Y34 (CORENET 2013.10.30)
   "CFYK3N1"
   //"CXDBREV"
};


void PermShoppingListObject::buildPermanentShoppingList(const char * picName)
{
   int i, j;
   int ipn;
   int status;
   char * pic;
   messageObject Msg(TO_HOST_MSG_ID, "PermanentShoppingListMessage");

   PidCtr = startPidCtr;
   numberOfIpns = startNumberOfIpns;

#ifdef DEBUG
   cout << " PermShoppingListObject::buildPermanentShoppingList(" 
	<< picName << ")" << endl;
#endif

   strcpy(&(picturesToUpdate[NEW_PICTURE_INDEX][0]), picName);

#ifdef DEBUG
   cout << "  The pictures to update are:" << endl;
   for(i = startIndex; i < NUM_PICTURES_TO_UPDATE; i++)
   {
      pic = &(picturesToUpdate[i][0]);
      cout << "      " << pic<< endl;
   }
#endif
   // *** create a new Picasso update array, i.e., permanent shopping
   // ***list, for the new picture.
   for(i = startIndex; i < NUM_PICTURES_TO_UPDATE; i++)
   {
      // if we are processing the header, footer, Page220 this time,
      //and we have already processed them, set the start index to 
      //the new picture entry in the picturesToUpdate array.
      //This way, when we change pictures, we only process the points 
      //that change.
      if((0 == startIndex)  &&  (NEW_PICTURE_INDEX == i))
      {
         // Add the permanent Pids here
         for (j = 0; j < NUM_PERMANENT_PIDS; j++)
         {
            addPid(&(permanentPidList[j][0]));
         }

	 startIndex = NEW_PICTURE_INDEX;
	 startPidCtr = PidCtr;
	 startNumberOfIpns = numberOfIpns;

#ifdef DEBUG
	 cout << "   Set startIndex to " << dec << NEW_PICTURE_INDEX
	      << " and startPidCtr to " << startPidCtr << endl;
#endif
      }
      pic = &(picturesToUpdate[i][0]);
      if(PicPidList.setPictureFocus(pic))
      {
#ifdef DEBUG
         cout << "   found picutre " << pic << " in the picture list" << endl;
#endif
         PicPidList.resetPidFocus();	// Focus on nothing
         // get the PicassoId of the currently focused on Pid for this picture
         while(FAIL != PicPidList.setNextPidFocus())
         {
#ifdef DEBUG
	    cout << "   adding PicassoId " << PicPidList.getCurPicassoId() 
		 << " for PID " << PicPidList.getCurPointName() 
		 << " at index " << dec << PidCtr << endl;
#endif
	    cout << " Permanent for PID " << PicPidList.getCurPointName() << endl;

            setItem(PidCtr, PicPidList.getCurPicassoId());
            PidCtr++;

	    ipn = PicPidList.getCurIpn();
	    ShoppingListMessage.theIPN[numberOfIpns] = ipn;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    ShoppingListMessage.theIPN[numberOfIpns] = htons(ShoppingListMessage.theIPN[numberOfIpns]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    numberOfIpns++;
	    
	    if(0 != DBO.getlIPN(ipn))
	    {
	       ShoppingListMessage.theIPN[numberOfIpns++] = DBO.getlIPN(ipn);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	      ShoppingListMessage.theIPN[numberOfIpns-1] = htons(ShoppingListMessage.theIPN[numberOfIpns-1]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	       setItem(PidCtr++, DBO.getiPicassoId(DBO.getlIPN(ipn)));
	    }

	    if(0 != DBO.gethIPN(ipn))
	    {
	       ShoppingListMessage.theIPN[numberOfIpns++] = DBO.gethIPN(ipn);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
               ShoppingListMessage.theIPN[numberOfIpns-1] = htons(ShoppingListMessage.theIPN[numberOfIpns-1]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	       setItem(PidCtr++, DBO.getiPicassoId(DBO.gethIPN(ipn)));
	    }

         } // endwhile there are Pids in the list
      } // endif picture is found in the picture list

#ifdef DEBUG
      else
      {
	 cout << "   " << pic << " not found in the picture list" << endl;
      }
#endif
   } // endfor

   // Add the Video Trend points here
#ifdef DEBUG
         cout << "   Adding the Video Trend & PT-Plot Points" << endl;
#endif
   addPid(Interface->VideoTrendData.Set1Point1.VideoTrendId);
   addPid(Interface->VideoTrendData.Set1Point2.VideoTrendId);
   addPid(Interface->VideoTrendData.Set1Point3.VideoTrendId);
   addPid(Interface->VideoTrendData.Set1Point4.VideoTrendId);
   addPid(Interface->VideoTrendData.Set2Point1.VideoTrendId);
   addPid(Interface->VideoTrendData.Set2Point2.VideoTrendId);
   addPid(Interface->VideoTrendData.Set2Point3.VideoTrendId);
   addPid(Interface->VideoTrendData.Set2Point4.VideoTrendId);

   /* 
   addPid(Interface->VideoTrendData.Set3Point1.VideoTrendId);
   addPid(Interface->VideoTrendData.Set3Point2.VideoTrendId);
   addPid(Interface->VideoTrendData.Set3Point3.VideoTrendId);
   addPid(Interface->VideoTrendData.Set3Point4.VideoTrendId);
   addPid(Interface->VideoTrendData.Set4Point1.VideoTrendId);
   addPid(Interface->VideoTrendData.Set4Point2.VideoTrendId);
   addPid(Interface->VideoTrendData.Set4Point3.VideoTrendId);
   addPid(Interface->VideoTrendData.Set4Point4.VideoTrendId);
   addPid(Interface->VideoTrendData.Set5Point1.VideoTrendId);
   addPid(Interface->VideoTrendData.Set5Point2.VideoTrendId);
   addPid(Interface->VideoTrendData.Set5Point3.VideoTrendId);
   addPid(Interface->VideoTrendData.Set5Point4.VideoTrendId);
   */ 
   addPid("RCP102A") ;
   addPid("RCP102B") ;
   addPid("RCP102C") ;
   addPid("RCP102D") ;
   addPid("RCP103J") ;
   addPid("RCP104K") ;
   addPid("RCT115") ;
   addPid("RCT125") ;
   addPid("RCT112HA") ;
   addPid("RCT112HB") ;
   addPid("RCT122HA") ;
   addPid("RCT122HB") ;
   addPid("RCT112CA") ;
   addPid("RCT112CB") ;
   addPid("RCT122CA") ;
   addPid("RCT122CB") ;
   addPid("RCYXLKI") ; // RcsLeak // yrlee 20070704
   addPid("RCYXLKU") ; // RcsLeak // yrlee 20070704 


   // Does the PicassoId always need to be at the end???
#ifdef DEBUG
   cout << "   adding PicassoId " << Interface->System.PicassoId
        << " for Interface->System.PicassoId at index " 
	<< dec << PidCtr << endl;
#endif
   setItem(PidCtr, Interface->System.PicassoId);
   PidCtr++;

   setNumberOfItems(PidCtr);

   setListIsBuilt();

#ifdef DEBUG
   cout << "   End rebuildPermanentShoppingList(): " 
	<< "Number of items in Perm Shopping List = " << dec 
	<< getNumberOfItems() << " Number of IPNs: " << numberOfIpns << endl;
#endif
   flush(cout);

   // Send the shopping list to the host via CommLink
   Msg.setHeader(PERM_SHOP_LIST, numberOfIpns);
   Msg.setData(&(ShoppingListMessage.theIPN[0]), 
		 (sizeof(ShoppingListMessage.theIPN[0]) * numberOfIpns));

   status = WS_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
   if (0 != status)
   {
      cout << " buildPermanentShoppingList(): ERROR sending PERM_SHOP_LIST "
           << "message\n";
      flush(cout);
   }
// #ifdef DEBUG
   cout << "\n***** buildPermanentShoppingList() completed sending "
	<< dec << Msg.getMsgSize() << " byte Permanent "
        << "Shopping List message to CommLink *****\n\n";
// #endif
   flush(cout);
} // end buildPermanentShoppingList()


void PermShoppingListObject::addPid(char * Pid)
{
   int	ipn;
#ifdef DEBUG
   cout << " PermShoppingListObject::addPid(" << Pid << ")" << endl;
#endif
   if(0 == strcmp("", Pid)) return;
   if ( (ipn = find_pid(Pid)) > 0)
   {
#ifdef DEBUG
      cout << '\t' << DBO.getpid(ipn) << ":" << '\t'
           << "Inserted into Permanent Update List as Picasso Id: "
           << DBO.getiPicassoId(ipn) << " at index " << dec << PidCtr << endl;
#endif
      // Put ALL points into the Picasso Update array
      setItem(PidCtr, DBO.getiPicassoId(ipn));
      PidCtr++;

      // Put ALL points into the host update message
      ShoppingListMessage.theIPN[numberOfIpns] = ipn;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
      ShoppingListMessage.theIPN[numberOfIpns] = htons(ShoppingListMessage.theIPN[numberOfIpns]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      numberOfIpns++;

   } //endif valid ipn returned from find_pid
#ifdef DEBUG
   else
   {
      cout << "   input Pid " << Pid << " is not valid" << endl;
   }
   flush(cout);
#endif
}




