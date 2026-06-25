//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
//
// parsePidForTestMessages.C
// ver 1.0 -- 2017.02.21.
// **********************************************
//
// ***************************************************************************
//  This module processes the Print Pid For Test messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************

// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

// Local Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"
#include "messageObject.h"

// External Variables:

extern	INTERFACE		*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject	IGUI_MsgQue;	// Commlink to InterfaceToGUI message queue

// Prototype Definitions:
   
//
// ********************************************************************************************	
//

void parsePidForTestMessages(messageObject & Msg)
{

   switch (Msg.getOpcode())
   {
      case REQUEST_PIDTEST:
         cout << " REQUEST_PIDTEST" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->wsPidTestData), sizeof(Interface->wsPidTestData));

	 break;

      case CONFIRM_PIDTEST:
         cout << " CONFIRM_PIDTEST" << endl;
	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->hostPidTestMsg), sizeof(Interface->hostPidTestMsg));
	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 break;

      default:
         cout << "parsePidForTestMessages: Received Unknown Message, opcode: " << hex << Msg.getOpcode() << endl;
	 break;
   }

}
