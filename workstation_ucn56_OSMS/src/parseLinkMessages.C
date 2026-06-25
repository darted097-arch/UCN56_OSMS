//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseLinkMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Data Link Status messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "messageObject.h"
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//      
//
// ***************************************************************************
//
struct LINK_DATA1{

   	StatusLine      SLine[7];
};

void parseLinkMessages(messageObject & msg)
{
   LINK_DATA1 *pLinkData;
   int line;
   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
     // to ccur
    case REQUEST_LINK_STATUS:
    case LINK_PAGE_OFF:
#ifdef DEBUG
         cout << " request Link Status  Display" << endl;
         cout << "opcode= " << hex << msg.getOpcode() << endl;         
#endif
         break;

     // from ccur	
    case SEND_LINK_STATUS:
#ifdef DEBUG
         cout <<" Link Status Display sent from Host!"<<endl;
#endif
	 pLinkData = (LINK_DATA1 *)&(Interface->Link_Data);
         // get the data from the message into the shared memory area
	  msg.getData(&(Interface->Link_Data), sizeof(Interface->Link_Data));
	  for (line=1; line<=7; line++)
	  {
	     pLinkData->SLine[line-1].status1[7] = '\0';
	     pLinkData->SLine[line-1].status2[7] = '\0';
	     pLinkData->SLine[line-1].status3[7] = '\0';
	     pLinkData->SLine[line-1].status4[7] = '\0';
	     pLinkData->SLine[line-1].status5[7] = '\0';
	     pLinkData->SLine[line-1].status6[7] = '\0';
	     pLinkData->SLine[line-1].status7[7] = '\0';

	  }//endfor
#ifdef DEBUG
	     cout << "Status = " << pLinkData->SLine[line-1].status1 << endl;	
#endif
   
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(SEND_LINK_STATUS);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
   }
}
