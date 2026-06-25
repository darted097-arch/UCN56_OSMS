//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseBarMessages.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Bar Chart messages.  This includes
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

// generic template for the local processing of workstation messages
//
// Local Constant Definitions: NONE
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
void parseBarMessages(messageObject & Msg)
{

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSrequestMessageData *grq_data_ptr; 
   WSresponseMessageData *grm_data_ptr; 
   BAR_DATA *abc_data_ptr; 
   BarChartMember *member_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {
      // to ccur
      case REQUEST_BAR_DATA:	// Picture requested PIDs, etc from CCUR
#ifdef DEBUG
	 printf("option= %d\n",Interface->GenericRequestMessage.data.requestOption);	 
	 cout << " request_bar_data" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
	 // put the Interface data into the message
	 Msg.setData(&(Interface->GenericRequestMessage.data), sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr(); 
         grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;

	 //  from ccur
      case SEND_BAR_DATA:	// CCUR responded to REQUEST_BAR_DATA by sending PIDs, etc.
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         abc_data_ptr = (BAR_DATA *)Msg.getDataPtr(); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Maximum));
#endif 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
	 cout << " send_bar_data" << endl;
#endif
	 // copy data from message to shared memory 
	 Msg.getData(&(Interface->BarChartData), sizeof(Interface->BarChartData));
	 
	 // Change the opcode in the message to tell InterfaceToGUI to tell the picture to display the data
	 Msg.setOpcode(DISPLAY_BAR_DATA);

         // Change the size of the message sent to InterfaceToGUI
	 Msg.setMsgSize(sizeof(MSG_HEADER_LEN));

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 
	 break;

	 //  to ccur
      case PROCESS_BAR_DATA:	// Pictures sends new PIDs, etc. to CCUR
#ifdef DEBUG
	 cout << " process_bar_data" << endl;
	 printf("Interface.Pid1= %s\n",Interface->BarChartData.Group1Point1.BarChartId);	 
#endif
	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->BarChartData), sizeof(Interface->BarChartData));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         abc_data_ptr = (BAR_DATA *)Msg.getDataPtr(); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point1.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point2.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point3.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group1Point4.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point1.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point2.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point3.Maximum)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Minimum)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Baseline)); 
         swap32((char *)&(abc_data_ptr->Group2Point4.Maximum)); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 	 
	 //  from ccur
      case CONFIRM_BAR_PROCESSING:	// CCUR confirms receipt of new PIDs, etc. from Picture
#ifdef DEBUG
	 cout << " confirm_bar_processing" << endl;
#endif

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grm_data_ptr = (WSresponseMessageData *)Msg.getDataPtr(); 
         grm_data_ptr->returnStatus = ntohl(grm_data_ptr->returnStatus); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // Get the data from the message into shared memory
	 Msg.getData(&Interface->GenericResponseMessage.data.returnStatus, 
	          sizeof(Interface->GenericResponseMessage.data.returnStatus));

	 // Change the opcode to tell InterfaceToGUI to tell the picture 
	 //that the processing was confirmed.
	 Msg.setOpcode(BAR_PROCESSING_CONFIRMED);

         // Change the size of the message sent to InterfaceToGUI
	 Msg.setMsgSize(sizeof(MSG_HEADER_LEN));

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break;

      default:
	   
	 cout << "parseBarMessages: Received Unknown Message, opcode: " 
	      << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
