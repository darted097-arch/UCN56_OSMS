//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseAckResetMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Alarm Acknowledge-Reset messages. These messages
//  are for Alarm Acknowledge Request and Confirmation, and for Alarm Reset
//  Request and Confirmation.  This includes messages from the host to the 
//  workstation, and messages from the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
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

//
// Local Constant Definitions:
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void parseAckResetMessages(messageObject & Msg)
{
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {
      // to ccur from Workstation, i.e. InterfaceToGUI processing.
      
      case REQUEST_ALARM_ACK:
      case REQUEST_ALARM_RESET:
	 
	 // Function Keys or pushbuttons on footer requests 
	 // alarm acknowledge or reset to be performed on CCUR.

#ifdef DEBUG
	 cout << "parseAckResetMessages: processing REQUEST_ALARM_xxx" << endl;	 
#endif
	 // no Interface->AckResetData data now, just Opcode
	 //
	 // get the Interface data into the message
	 //	 Msg.setData(&Interface->AckResetData,
	 //				sizeof(Interface->AckResetData));
	 
	 break;
	 	 
	 //  from ccur, i.e. CommLink processing
      case CONFIRM_ACK_PROCESSING:	// CCUR sends confirmation status 
      case CONFIRM_RESET_PROCESSING:
	 	
	 // get data from the message to shared memory 
	 //	 Msg.getData(&Interface->AckResetData, sizeof(AckReset_Data));
	 	 
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display	 	
	 // the confirmation status of the alarm acknowledge or reset.
	 switch (Msg.getOpcode())
	 {
	    case CONFIRM_ACK_PROCESSING:
#ifdef DEBUG
	       cout << "parseAckResetMessages: processing CONFIRM_ACK_PROCESSING" << endl;
#endif
	       Msg.setOpcode(ACK_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_RESET_PROCESSING:
#ifdef DEBUG
	       cout << "parseAckResetMessages: processing CONFIRM_RESET_PROCESSING" << endl;
#endif
	       Msg.setOpcode(RESET_PROCESSING_CONFIRMED);
	       break;

	    default:	// can't happen
	       break;
	 } // endswitch

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	 break;

      default:
	   cout  << "parseAckResetMessages: Received Unknown Message, opcode: " 
	         << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
