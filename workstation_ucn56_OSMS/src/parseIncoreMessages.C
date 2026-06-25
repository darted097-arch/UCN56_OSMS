//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseIncoreMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the Incore messages.  This includes
//			both messages from the host to the workstation and messages from
//			the workstation to the host.
// **************************************************************************************
//													
// **************************************************************************************
//													
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <unistd.h>

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

#define DEBUG

// Local Definitions
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

// Prototype Definitions:
   
//													
// **************************************************************************************
//													
void parseIncoreMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   IncoreDataTemplate * incoredata_ptr; // CORENET
#endif
// >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
    cout << "parseIncoreMessage()...opcode = " << hex << Msg.getOpcode() << endl;
#endif

   switch (Msg.getOpcode())
   {
      
      // >>>>>>>>>>>>>>>>>>>> Incore Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<

      case INCORE_PROCESS_REQUEST:
      case INCORE_CHARGE_REQUEST:
      case INCORE_CHARGE_UPDATE: 
	{
	 // get the host name to select the proper printer name
	 //gethostname (Interface->IncoreData.theWorkstation, LEN_SYSTEMNAME);
	 gethostname (Interface->IncoreData.theWorkstation, 10);
         Msg.setData(&(Interface->IncoreData),sizeof(Interface->IncoreData));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 incoredata_ptr = (IncoreDataTemplate *)Msg.getDataPtr();
	 incoredata_ptr->option = htonl(incoredata_ptr->option);
	 incoredata_ptr->detectorNumber = htonl(incoredata_ptr->detectorNumber);
	 incoredata_ptr->detectorLevel = htonl(incoredata_ptr->detectorLevel);
	 incoredata_ptr->maxTime = htonl(incoredata_ptr->maxTime);
	 swap32((char *)&(incoredata_ptr->oldCharge));
	 swap32((char *)&(incoredata_ptr->newCharge));
	 incoredata_ptr->yesno = htonl(incoredata_ptr->yesno);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
    	    cout << "SND DATA...opcode = " << hex << Msg.getOpcode() << endl;
#endif

	 break;
      	}

     // >>>>>>>>>>>>>>>>>>>> Incore Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<

      case INCORE_PROCESS_CONFIRM:
	 {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////	   
	 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>. CORENET
#ifdef LINUX_APP
	 incoredata_ptr = (IncoreDataTemplate *)Msg.getDataPtr();
	 incoredata_ptr->option = ntohl(incoredata_ptr->option);
	 incoredata_ptr->detectorNumber = ntohl(incoredata_ptr->detectorNumber);
	 incoredata_ptr->detectorLevel = ntohl(incoredata_ptr->detectorLevel);
	 incoredata_ptr->maxTime = ntohl(incoredata_ptr->maxTime);
	 swap32((char *)&(incoredata_ptr->oldCharge));
	 swap32((char *)&(incoredata_ptr->newCharge));
	 incoredata_ptr->yesno = ntohl(incoredata_ptr->yesno);
#endif
	 //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    // terminates messages with NULL character
         
            Msg.getData(&(Interface->GenericConfirmMessage.data),sizeof(Interface->GenericConfirmMessage.data));
	    Interface->GenericConfirmMessage.data.message1[39] = '\0';
	    Interface->GenericConfirmMessage.data.message2[39] = '\0';
	    Interface->GenericConfirmMessage.data.message3[39] = '\0';
	    Msg.setOpcode(GENERIC_PROCESSING_CONFIRMED);

	    //CORENET 0706
	    cout << "INCORE message1 = " << Interface->GenericConfirmMessage.data.message1 << endl;
	    cout << "INCORE message2 = " << Interface->GenericConfirmMessage.data.message2 << endl;
	    cout << "INCORE message3 = " << Interface->GenericConfirmMessage.data.message3 << endl;


	    // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C
	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
#ifdef DEBUG
    	    cout << "Received INCORE_PROCESS_CONFIRM opcode" << endl;
#endif
	    break;
	 }

     // >>>>>>>>>>>>>>>>>>>> Incore Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<

      case INCORE_CHARGE_RESPONSE:
	 {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////	   
	 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>. CORENET
#ifdef LINUX_APP
	 incoredata_ptr = (IncoreDataTemplate *)Msg.getDataPtr();
	 incoredata_ptr->option = ntohl(incoredata_ptr->option);
	 incoredata_ptr->detectorNumber = ntohl(incoredata_ptr->detectorNumber);
	 incoredata_ptr->detectorLevel = ntohl(incoredata_ptr->detectorLevel);
	 incoredata_ptr->maxTime = ntohl(incoredata_ptr->maxTime);
	 swap32((char *)&(incoredata_ptr->oldCharge));
	 swap32((char *)&(incoredata_ptr->newCharge));
	 incoredata_ptr->yesno = ntohl(incoredata_ptr->yesno);
#endif
	 //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    // terminates messages with NULL character
         
            Msg.getData(&(Interface->IncoreData),sizeof(Interface->IncoreData));
	    Interface->IncoreData.pid[8]       = '\0';
	    Interface->IncoreData.message1[39] = '\0';
	    Interface->IncoreData.message2[39] = '\0';
	    Interface->IncoreData.message3[39] = '\0';
	    // put confirm message data into generic confirm message
	    strcpy(Interface->GenericConfirmMessage.data.message1,
		   Interface->IncoreData.message1);
	    strcpy(Interface->GenericConfirmMessage.data.message2,
		   Interface->IncoreData.message2);
	    strcpy(Interface->GenericConfirmMessage.data.message3,
		   Interface->IncoreData.message3);
	    Msg.setOpcode(GENERIC_PROCESSING_CONFIRMED);
	    // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C
	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
#ifdef DEBUG
   	    cout << "Received INCORE_CHARGE_RESPONSE opcode" << endl;
#endif
	    break;
	 }
 
   } // end of opcode switch statement

}
