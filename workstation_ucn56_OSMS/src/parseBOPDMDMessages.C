//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseBOPDMDMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the BOP Print messages.  This includes
//			both messages from the host to the workstation and messages from
//			the workstation to the host.
// ********************************************************************************************	
//
// ********************************************************************************************	
//
// System Include Files:
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

// Local Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "messageObject.h"		//

// Local Definitions
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

// Prototype Definitions:
   
void parseBOPDMDMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSBOPDMDRequestData * bopdmdreq_data_ptr;
   WSBOPDMDConfirmData * bopdmdcnf_data_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   switch (Msg.getOpcode())
   {
      
      // >>>>>>>>>>>>>>>>>>>> BOPDMD Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request
       case REQUEST_BOP_PRINT:
	{
	 // get the host name to select the proper printer name
	 gethostname (Interface->BOPDMD_REQUEST_MESSAGE.data.theWorkstation,LEN_SYSTEMNAME);
         Msg.setData(&(Interface->BOPDMD_REQUEST_MESSAGE.data),sizeof(Interface->BOPDMD_REQUEST_MESSAGE.data)); 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           bopdmdreq_data_ptr = (WSBOPDMDRequestData *)Msg.getDataPtr();
           bopdmdreq_data_ptr->Option = htonl(bopdmdreq_data_ptr->Option);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
         cout << "send REQUEST_BOP_PRINT " << endl;   
#endif
         break;
         }
      //
   
      // >>>>>>>>>>>>>>>>>>>> BOPDMD Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<
        case CONFIRM_BOP_PRINT:
     {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           bopdmdcnf_data_ptr = (WSBOPDMDConfirmData *)Msg.getDataPtr();
           bopdmdcnf_data_ptr->Option = ntohl(bopdmdcnf_data_ptr->Option);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    // terminates messages with NULL character
         
            Msg.getData(&(Interface->BOPDMD_CONFIRM_MESSAGE.data),sizeof(Interface->BOPDMD_CONFIRM_MESSAGE.data));
	    Interface->BOPDMD_CONFIRM_MESSAGE.data.message1[39] = '\0';
	    Interface->BOPDMD_CONFIRM_MESSAGE.data.message2[39] = '\0';
	    Interface->BOPDMD_CONFIRM_MESSAGE.data.message3[39] = '\0';
#ifdef DEBUG
    	    cout << "Received CONFIRM_BOP_PRINT opcode" << endl;
#endif
	    Msg.setOpcode(BOP_PRINT_CONFIRMED);
            // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C
	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	    break;
	 }  

      default:
	   
	     cout << "parseBOPDMDMessages: Received Unknown Message, opcode: " 
	          << hex << Msg.getOpcode() << endl;

	 
   } // end of opcode switch statement

}
