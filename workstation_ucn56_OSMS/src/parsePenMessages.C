//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parsePenMessages.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Pen Recorder messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
//ok#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"
#include "messageObject.h"
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

//
// Local Constant Definitions:
//
#ifndef DEBUG
   #define DEBUG
#endif
//
// Function Prototypes: NONE
//      
//
// ***************************************************************************
//
void parsePenMessages(messageObject & Msg)
{

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CORENET
#ifdef LINUX_APP
	PEN_DATA * pendata_ptr; // CORENET
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
   cout << "parsePenMessages:\n";
#endif
   
   switch(Msg.getOpcode())
   {
      // to ccur
      case REQUEST_PEN_DATA:	// Picture requested PIDs, etc from CCUR
#ifdef DEBUG
	 cout << " REQUEST_PEN_DATA" << endl;
#endif
	 // Nothing to do in this case
	 break;

	 //  from ccur
      case SEND_PEN_DATA:	// CCUR responded to REQUEST_PEN_DATA by sending PIDs, etc.
#ifdef DEBUG
	 cout << " SEND_PEN_DATA" << endl;
#endif
	 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CORENET
#ifdef LINUX_APP
	 pendata_ptr = (PEN_DATA *)Msg.getDataPtr();

	 swap32((char *) &(pendata_ptr->trend1Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend1Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend1Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend1Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend2Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend2Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend2Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend2Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend3Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend3Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend3Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend3Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend4Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend4Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend4Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend4Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend5Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend5Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend5Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend5Pen2.Maximum));
#endif
	 //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 Msg.setOpcode(DISPLAY_PEN_DATA);

	 // get data from message to shared memory 
	 Msg.getData(&Interface->PenRecorderData, sizeof(PEN_DATA));
	 
	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());	 
	 break;

	 //  to ccur
      case PROCESS_PEN_DATA:	// Pictures sends new PIDs, etc. to CCUR
#ifdef DEBUG
	 cout << " PROCESS_PEN_DATA\n";
#endif
	 Msg.setData(&Interface->PenRecorderData,
		     sizeof(Interface->PenRecorderData));

	 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CORENET
#ifdef LINUX_APP
	 pendata_ptr = (PEN_DATA *)Msg.getDataPtr();

	 swap32((char *) &(pendata_ptr->trend1Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend1Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend1Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend1Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend2Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend2Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend2Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend2Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend3Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend3Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend3Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend3Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend4Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend4Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend4Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend4Pen2.Maximum));

	 swap32((char *) &(pendata_ptr->trend5Pen1.Minimum));
	 swap32((char *) &(pendata_ptr->trend5Pen1.Maximum));
	 swap32((char *) &(pendata_ptr->trend5Pen2.Minimum));
	 swap32((char *) &(pendata_ptr->trend5Pen2.Maximum));
#endif
	 //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 	 
	 //  from ccur
      case CONFIRM_PEN_PROCESSING:	// CCUR confirms receipt of new PIDs, etc. from Picture
#ifdef DEBUG
	 cout << " CONFIRM_PEN_PROCESSING" << endl;
#endif
	 
	 // Change the opcode to tell InterfaceToGUI to tell the picture 
	 //that the processing was confirmed.
	 Msg.setOpcode(PEN_PROCESSING_CONFIRMED);

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	 break;

      default:
	 cout << "parsePenMessages: Received Unknown Message, opcode: " 
	      << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
