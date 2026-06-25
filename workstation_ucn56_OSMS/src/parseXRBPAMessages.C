//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseXRBPAMessages.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ******************************************************************************
//	Purpose:	This module processes the XRBPAUTO messages.  This includes
//			both messages from the host to the workstation and messages
//			from the workstation to the host.
// *******************************************************************************
//
// *******************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.11
#include <netinet/in.h> // CORENET 2012.07.11
#endif

// Local Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"         //
#include "messageObject.h"

// Local Definitions
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject  IGUI_MsgQue; // CommLink to InterfaceToGUI message que

// Prototype Definitions:
   
//
// *******************************************************************************
//
void parseXRBPAMessages(messageObject & msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   XRBP_AUTO_TEMPLATE	*xrbpa_data_ptr;
   WSrequestMessageData *grq_data_ptr;
   int j;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
   int    i;
#endif
      
   switch(msg.getOpcode())
   {
      
      case XRBPA_UP_START:
#ifdef DEBUG
	   cout <<"request XRBP_UP_START: theOpcode=" << msg.getOpcode() << endl;
#endif
	   // copy data area from shared memory region into local buffer
	   msg.setData(&(Interface->GenericRequestMessage.data),
		       sizeof(Interface->GenericRequestMessage.data));
	   
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grq_data_ptr = (WSrequestMessageData *)msg.getDataPtr();
         grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
	   cout << "parseXRBPA:XRBPA Start ready to send: " << msg.getOpcode() << endl;
#endif
	   break;

      case XRBPA_UP_STOP:
           // Copy the XRBP input Parameters to the local buffer region
              msg.setData(&(Interface->GenericRequestMessage.data),
			  sizeof(Interface->GenericRequestMessage.data));
           // Now it is ready to be transmitted to the host

//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grq_data_ptr = (WSrequestMessageData *)msg.getDataPtr();
         grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
	   cout << "parseXRBPA:XRBPA Stop ready to send: " << msg.getOpcode() << endl;
#endif
           break;

      case XRBPA_DISPLAY:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   xrbpa_data_ptr = (XRBP_AUTO_TEMPLATE *)msg.getDataPtr();
	   xrbpa_data_ptr->error_code = ntohl(xrbpa_data_ptr->error_code);
	   for (j = 0; j < 13; j++)
		swap32((char *)&(xrbpa_data_ptr->output_val[j]));
	   for (j = 0; j < 6; j++)
	   	xrbpa_data_ptr->daytime[j] = ntohl(xrbpa_data_ptr->daytime[j]);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   // copy the processed message buffer to the interface structure -> GUI
	      msg.getData(&(Interface->XRBPAUTO.outputs), 
			  sizeof(Interface->XRBPAUTO.outputs));
           
           // Now host data has been arrived at workstation
#ifdef DEBUG
	   cout << "parseXRBPA: received XRBP Output Data from Host: theOpcode = " 
		<< Interface->XRBPAUTO.header.opcode 
		<< endl;
           cout << "parseXRBPA: error_code = " <<dec
		<< Interface->XRBPAUTO.outputs.error_code
		<< endl;
	   for (i=0;i<13;i++){
               cout << "parseXRBPA: output_val["<<dec<<i<<"] = "<<dec
		    << Interface->XRBPAUTO.outputs.output_val[i]
		    <<endl;
           }
	   for (i=0;i<6;i++){
	       cout << "parseXRBPA: daytime["<<dec<<i<<"] = "<<dec
		    << Interface->XRBPAUTO.outputs.daytime[i]
		    << endl;
           }
#endif
           // Send the message with the new opcode to InterfaceToGUI
           // InterfaceToGUI will process that new message through parsePictureMessages.C
              IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	   break;
   }

}
