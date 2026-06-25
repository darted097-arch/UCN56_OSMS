//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseDTrendMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the Digital Trend messages.  This includes
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
#include <netdb.h>
#include <netinet/in.h>
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
//#ifndef DEBUG
 //  #define DEBUG
//#endif
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//      
//
// ********************************************************************************************	
//
void parseDTrendMessages(messageObject & msg)
{
#ifdef DEBUG
   union {
      WSmessageTemplate		message;
      WSDigitalTrendMessage     DTrendMsg;
   } Local;
#endif
   int    NoPrint;
   
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSrequestMessageData *grq_data_ptr; // 
   DTREND_DATA *dtrend_data_ptr; // 
   WSresponseMessageData *grm_data_ptr; // 
   WSResetMessageData *reset_data_ptr; // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
      // to ccur
      case REQUEST_DTREND_DATA:	// Picture requested PIDs, etc from CCUR

#ifdef DEBUG
	 printf("requestOption= %d\n",Interface->GenericRequestMessage.data.requestOption);
	 cout << " Send REQUEST_DTREND_DATA message to CCUR " << endl;
#endif
	 // get the host name
         gethostname(Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);

	 // copy the DTrend input Parameters from shared memory into the message
	 msg.setData(&(Interface->GenericRequestMessage.data), 
		      sizeof(Interface->GenericRequestMessage.data));

         // Now it is ready to be transmitted to the host

#ifdef DEBUG
	 cout << "DTrend -msg : Request theOpcode = " << hex << msg.getOpcode() << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grq_data_ptr = (WSrequestMessageData *)msg.getDataPtr(); // CORENET
         grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption); // CORENET
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;

      // from ccur
      case SEND_DTREND_DATA:	// CCUR responded to REQUEST_DTREND_DATA by sending PIDs, etc.

#ifdef DEBUG
	 cout << " Received SEND_DTREND_DATA message from CCUR" << endl;
#endif

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         dtrend_data_ptr = (DTREND_DATA *)msg.getDataPtr(); 
	 dtrend_data_ptr->displayRate = ntohl(dtrend_data_ptr->displayRate);
	 dtrend_data_ptr->nosToPrint = ntohl(dtrend_data_ptr->nosToPrint);
	 dtrend_data_ptr->prtQueue = ntohl(dtrend_data_ptr->prtQueue);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 msg.setOpcode(DISPLAY_DTREND_DATA);

	 // copy data from message to shared memory 
	 msg.getData(&(Interface->DigitalTrendData), 
			sizeof(Interface->DigitalTrendData));
#ifdef DEBUG	 
         // Now host data has been arrived at workstation
         cout << "Received from CCUR: Set1Point1 = " << Interface->DigitalTrendData.Set1Point1 << endl;
         cout << "Received from CCUR: Set2Point1 = " << Interface->DigitalTrendData.Set2Point1 << endl;
	 cout << "Received from CCUR: displayRate = " << dec << Interface->DigitalTrendData.displayRate<< endl;
	 cout << "Received from CCUR: nosToPrint = " << dec << Interface->DigitalTrendData.nosToPrint<< endl;
	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
#endif
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 break;

	 //  to ccur
      case PROCESS_DTREND_DATA:	// Pictures sends new PIDs, etc. to CCUR

#ifdef DEBUG	 
	 cout << " Send PROCESS_DTREND_DATA message to CCUR" << endl;
	 printf("Interface.PointID1= %s\n",Interface->DigitalTrendData.Set1Point1);	 
	 printf("Interface.PointID1= %s\n",Interface->DigitalTrendData.Set1Point1);	 
#endif	 
	 // get the host name to select the proper printer name
         gethostname(Interface->DigitalTrendData.theWorkstation,LEN_SYSTEMNAME);

         // put the Interface Shared Memory data into the message
	 msg.setData(&(Interface->DigitalTrendData), sizeof(Interface->DigitalTrendData));
         
         // Now it is ready to be transmitted to the host
#ifdef DEBUG
	 msg.getMessage(&Local.DTrendMsg, sizeof(Local.DTrendMsg));
	 cout << "Send to CCUR: theOpcode = " << hex <<   Local.message.header.opcode << endl;
         cout << "Send to CCUR: Set1Point1= " <<          Local.DTrendMsg.data.Set1Point1<<endl;
         cout << "Send to CCUR: Set2Point1= " <<          Local.DTrendMsg.data.Set2Point1<<endl;
	 cout << "Send to CCUR: DisplayRate = " << dec << Local.DTrendMsg.data.displayRate<< endl;
	 cout << "Send to CCUR: Interface.nosToPrint = " << dec << Interface->DigitalTrendData.nosToPrint<< endl;
	 cout << "Send to CCUR: nosToPrint = " << dec <<  Local.DTrendMsg.data.nosToPrint<< endl;
         printf ("Send to CCUR: theWorkstation = %s\n",   Local.DTrendMsg.data.theWorkstation);
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         dtrend_data_ptr = (DTREND_DATA *)msg.getDataPtr(); 
	 dtrend_data_ptr->displayRate = htonl(dtrend_data_ptr->displayRate);
	 dtrend_data_ptr->nosToPrint = htonl(dtrend_data_ptr->nosToPrint);
	 dtrend_data_ptr->prtQueue = htonl(dtrend_data_ptr->prtQueue);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 	 
      //  from ccur
      case CONFIRM_DTREND_PROCESSING:	// CCUR confirms receipt of new PIDs, etc. from Picture

#ifdef DEBUG
	 cout << " Received CONFIRM_DTREND_PROCESSING message from CCUR" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grm_data_ptr = (WSresponseMessageData *)msg.getDataPtr(); // 
         grm_data_ptr->returnStatus = ntohl(grm_data_ptr->returnStatus); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 msg.getData(&(Interface->GenericResponseMessage.data.returnStatus), 
	          sizeof(Interface->GenericResponseMessage.data.returnStatus));

#ifdef DEBUG
	 cout << "Received from CCUR: Interface.returnStatus = " << dec 
	      << Interface->GenericResponseMessage.data.returnStatus<< endl;
#endif
	 // Interface->GenericResponseMessage.data.returnStatus = 0;
	 
	 NoPrint = Interface->DigitalTrendData.nosToPrint;

	 if (NoPrint == 0)
	 {
         // Change the opcode to tell InterfaceToGUI to tell the picture
         //that the processing was confirmed.
	    msg.setOpcode(DTREND_PROCESSING_CONFIRMED);
	 }
	 else
	 {
           msg.setOpcode(DTREND_PRT_PROCESS_CONFIRMED);
	 }
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 break;

      // to ccur
      case RESET_DTREND_REQUEST: // Picture resets flag on DTrend Module of CCUR

#ifdef DEBUG
	 cout << " Send RESET_DTREND_REQUEST message to CCUR " << endl;
#endif
	 // get the host name
         gethostname(Interface->DigitalTrendReset.data.theWorkstation,LEN_SYSTEMNAME);

	 // copy the DTrend input Parameters from shared memory into the message
	 msg.setData(&(Interface->DigitalTrendReset.data), 
		      sizeof(Interface->DigitalTrendReset.data));


//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         reset_data_ptr = (WSResetMessageData *)msg.getDataPtr(); // 
	 reset_data_ptr->requestOption = htonl(reset_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
         // Now it is ready to be transmitted to the host

#ifdef DEBUG
	 cout << "DTrend -msg : Request theOpcode = " << hex << msg.getOpcode() << endl;
#endif
	 break;

      //  from ccur
      case INUSE_DTREND_RESPONSE:	// CCUR confirms receipt of new PIDs, etc. from Picture

#ifdef DEBUG
	 cout << " Received INUSE_DTREND_RESPONSE message from CCUR" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         dtrend_data_ptr = (DTREND_DATA *)msg.getDataPtr(); 
	 dtrend_data_ptr->displayRate = ntohl(dtrend_data_ptr->displayRate);
	 dtrend_data_ptr->nosToPrint = ntohl(dtrend_data_ptr->nosToPrint);
	 dtrend_data_ptr->prtQueue = ntohl(dtrend_data_ptr->prtQueue);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

         // Change the opcode to tell InterfaceToGUI to tell the picture
         // that the processing was confirmed.
	 msg.setOpcode(INUSE_DTRD_RESPONSE_CONFIRM);
	 
	 // copy the data from the message into the shared memory area
	 msg.getData(&(Interface->DigitalTrendData), 
			sizeof(Interface->DigitalTrendData));
#ifdef DEBUG	 
         // Now host data has been arrived at workstation
         cout << "Received from CCUR: Set1Point1 = " << Interface->DigitalTrendData.Set1Point1 << endl;
         cout << "Received from CCUR: Set2Point1 = " << Interface->DigitalTrendData.Set2Point1 << endl;
	 cout << "Received from CCUR: displayRate = " << dec << Interface->DigitalTrendData.displayRate<< endl;
	 cout << "Received from CCUR: nosToPrint = " << dec << Interface->DigitalTrendData.nosToPrint<< endl;
#endif
	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());

#ifdef DEBUG
         cout << "Received from CCUR: Workstation = " << Interface->DigitalTrendData.theWorkstation << endl;
#endif
	 break;

      //  from ccur
      case COMPLETED_DTREND_PRINT:	// CCUR confirms receipt of new PIDs, etc. from Picture

#ifdef DEBUG
	 cout << " Received COMPLETED_DTREND_PRINT message from CCUR" << endl;
#endif
// There is no data to get! Only the opcode in the header
//	 msg.getData(&(Interface->GenericResponseMessage.data), 
//		      sizeof(Interface->GenericResponseMessage.data));

         // Change the opcode to tell InterfaceToGUI to tell the picture
         // that the processing was confirmed.
	    msg.setOpcode(COMPLETED_DTRD_PRT_CONFIRM);

	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 break;

      //  from ccur
      case INTERRUPTED_DTREND_PRINT:	// CCUR confirms receipt of new PIDs, etc. from Picture

#ifdef DEBUG
	 cout << " Received INTERRUPTED_DTREND_PRINT message from CCUR" << endl;
#endif
// There is no data to get! Only the opcode in the header
//	 msg.getData(&(Interface->GenericResponseMessage.data), 
//		      sizeof(Interface->GenericResponseMessage.data));

         // Change the opcode to tell InterfaceToGUI to tell the picture
         // that the processing was confirmed.
	    msg.setOpcode(INTERRUPT_DTRD_PRT_CONFIRM);

	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 break;

      default:
	   
	 cout << "parseDTrendMessages: Received Unknown Message, opcode: " 
	      << hex << msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
