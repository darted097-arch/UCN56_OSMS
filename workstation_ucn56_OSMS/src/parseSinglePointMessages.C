//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseSinglePointMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes messages the Single Point Display messages
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.11
#include <netinet/in.h> // CORENET 2012.07.11
#endif

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
#ifndef DEBUG
   #define DEBUG
#endif
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue
//
// Local Constant Definitions: NONE
//
// Function Prototypes:
//
#include "prototypes.h"
//   
// ***************************************************************************
//
void parseSinglePointMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   SINGLE_POINT * sp_data_ptr;	
   WSconfirmMessageData * gcm_data_ptr;	
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   union {
      WSmessageTemplate		message;
      WSsinglePointMessage	DisplaySinglePoint;
      WSsinglePointMessage	PrintSinglePoint;
      WSresponseMessage		ConfirmPrint;
   } Local;
   
   
   // structures for the Single Point Display
   struct SINGLE_POINT  *pInterfaceData = &Interface->SinglePointDisplay;
   struct SINGLE_POINT  *pLocalData     = &Local.DisplaySinglePoint.data;

   // structures for the Single Point Print confirmation
   struct WSresponseMessage *pInterfaceConfirm = &Interface->GenericResponseMessage;
   struct WSresponseMessage *pLocalConfirm     = &Local.ConfirmPrint;

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
   cout << "parseSinglePointMessages():\n";
#endif

   // copy to local union so that the message can be accessed as various 
   //sturctures
   Msg.getMessage(&(Local.message), sizeof(Local.message));

   switch (Msg.getOpcode())
   {
	// Workstation -> CCUR
        // Send the same info to the CCUR for both messages; it won't hurt
	case DISPLAY_SINGLE_POINT:
	case PRINT_SINGLE_POINT:
#ifdef DEBUG
	   cout << "Display Single Point Message" << endl;
#endif
	   // Put data in the Interface structure in case you want to use it in the display(s)
	   // get the hostname to know which host printer to use
	   gethostname (Interface->SinglePointDisplay.theWorkstation,LEN_SYSTEMNAME);

	   // get the IPN for validation at the host
	   Interface->SinglePointDisplay.theIPN = find_pid(Interface->SinglePointDisplay.thePointId);

	   // Copy the Interface structure to the message 
	   Msg.setData(&Interface->SinglePointDisplay,
		       sizeof(Interface->SinglePointDisplay));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   sp_data_ptr = (SINGLE_POINT *)Msg.getDataPtr();
	   sp_data_ptr->theIPN = htons(sp_data_ptr->theIPN);
	   sp_data_ptr->theAlignment = htons(sp_data_ptr->theAlignment);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   break;

	// CCUR -> Workstation
      case CONFIRM_PRINT_SINGLE_POINT:	// CCUR confirms receipt of new PID from Picture
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   gcm_data_ptr = (WSconfirmMessageData *)Msg.getDataPtr();
	   gcm_data_ptr->status = ntohl(gcm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
	 cout << " Received CONFIRM_PRINT_SINGLE_POINT message from CCUR" << endl;
#endif
	 // copy the data from the message into the shared memory area
	 Msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));

         // Change the opcode to tell InterfaceToGUI to tell the picture
         // that the processing was confirmed.
	    Msg.setOpcode(PRINT_SINGLE_POINT_CONFIRMED);

	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());

#ifdef DEBUG
         printf("CONFIRM_PRINT_SINGLE_POINT IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;

	default:
	   
	 cout << "parseSinglePointMessages: Received Unknown Message, opcode: " 
	      << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
   
}
