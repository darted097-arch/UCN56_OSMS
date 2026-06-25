//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// connectToGUI.C       
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module performs the initialization sequence whenever the	
//  InterfaceToGUI task connects to the Picasso GUI	
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
// User Defined Include Files: NONE
//
#include "interface.h"
#include "PermShoppingListObject.h"
#include "messageQueueObject.h"
#include "messageObject.h"
#include "messageParameters.h"
#include "PicPidListObject.h"
//
// External Variables: NONE
//
extern INTERFACE		* Interface;		// shared memory Interface structure
extern PermShoppingListObject	PermShoppingList;
extern messageQueueObject	IGUI_MsgQue;		// InterfaceToGUI message queue
extern PicPidListObject	PicPidList;	// Picture list w/PIDs
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes:
//
void defineDatabase();		// creates db structure and fills the vars
void defineMessages();		// creates generic messages
void defineInterface();		// creates interface structure
void defineFunctions();		// registers functions used in the GUI
//
// ***************************************************************************
//
void connectToGUI (int status, const char *msg)
{
   // create a message object for messages to be received by parsePictureMessages
   messageObject	qmsg(FROM_HOST_MSG_ID,"connectToGUImsg");
   
   // check the status coming in		
   if (status & PfCrtmStart)
      cerr << "InterfaceToGUI Connection Established, status = 0x" << hex << status << endl;
   if (status & PfCrtmResume)
      cerr << "InterfaceToGUI Connection Re-Established, status = 0x" << hex << status << endl
           << "Message: " << msg << endl;
	
#ifdef DEBUG   
   cout << " connectToGUI: status from rtm = " << dec << status << endl;
   cout << " connectToGUI: About to define db, messages, etc.\n";
#endif
   
   defineDatabase();		// define the db structure and fill it
   defineMessages();		// define the generic messages
   defineInterface();		// define interface variable structures	
   defineFunctions();		// register all GUI functions
   
   // flush the buffer... (New in Version 2.0)
   PfFlushCreateVar();

   // create the permanent shopping list
#ifdef DEBUG   
   cout << " connectToGUI: About to create the Permanent Shopping List\n";
#endif
   
//   PermShoppingList.createShoppingList();
   PicPidList.createPicturePidLists();
   
   // Flush, i.e., remove all messages from the message queue.
   //There should not be any messages on the message queue, but 
   //experience has shown that some errant programs on the host 
   //send messages to the workstation even before the WS has 
   //finished its intialization.
   IGUI_MsgQue.flush();

   // set the message opcode and put the message on the message queue
   qmsg.setOpcode(DISPLAY_SECURITY_PAGE);
   status = IGUI_MsgQue.sendMessage(qmsg.pMsg, qmsg.getMsgSize(),
                                    qmsg.getType());

   if (status == 0)
      cout << " connectToGUI: sent message to display security page" << endl;
   else
      cout << " ***** connectToGUI: Error sending message to display security page" << endl;

   cout << "\n***** connectToGUI: rtm initialization routine complete *****\n";
}// end connectToGUI
