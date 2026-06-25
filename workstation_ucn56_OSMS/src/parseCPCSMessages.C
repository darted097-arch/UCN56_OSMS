//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseCPCSMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the IOCPCS messages.  This includes
//  messages from the host to the workstation.
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
void parseCPCSMessages(messageObject & msg)
{
#ifdef DEBUG
         cout <<" C A L Display sent from Host!"<<endl;
#endif
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
     // to ccur
    case CONFIRM_CECOR_REQUEST:
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(CONFIRM_CECOR_REQUEST);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CPCEAC_REQUEST:
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(CONFIRM_CPCEAC_REQUEST);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CPCS_REQUEST:
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(CONFIRM_CPCS_REQUEST);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEAC_REQUEST:
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(CONFIRM_CEAC_REQUEST);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
	  break;
    case REJECT_CPCS_REQUEST:
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(REJECT_CPCS_REQUEST);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
	  break;
   }
}
