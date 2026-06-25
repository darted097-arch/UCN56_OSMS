//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseVTRENDMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Video Trend messages.  This includes
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
// ***************************************************************************
//
void parseVTRENDMessages(messageObject & msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   VIDEO_DATA *v_data_ptr; 
   VideoTrendMember *vt_mem_ptr;
   VTflagMessageData *vtf_data_ptr; 
   WSresponseMessageData *grm_data_ptr; 
   VIDEO_SETUP_DATA *vsu_data_ptr; 
   int i = 0;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
      // to ccur
      case REQUEST_VTREND_DATA:	// Request message to CCUR
#ifdef DEBUG
//	 printf("option= %d\n",Interface->VTFlagMessage.data.requestOption);	 
	 cout << " REQUEST_VTREND_DATA" << endl;
#endif
	 msg.setData(&(Interface->VTFlagMessage.data),
		     sizeof(Interface->VTFlagMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 vtf_data_ptr = (VTflagMessageData *)msg.getDataPtr(); 
	 vtf_data_ptr->requestOption = htonl(vtf_data_ptr->requestOption); 
	 vtf_data_ptr->flipflop1 = htonl(vtf_data_ptr->flipflop1); 
	 vtf_data_ptr->flipflop2 = htonl(vtf_data_ptr->flipflop2); 
	 vtf_data_ptr->flipflop3 = htonl(vtf_data_ptr->flipflop3); 
	 vtf_data_ptr->flipflop4 = htonl(vtf_data_ptr->flipflop4); 
	 vtf_data_ptr->flipflop5 = htonl(vtf_data_ptr->flipflop5); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
	 cout << "opcode= " << hex << msg.getOpcode() << endl;
#endif
	 break;

      case VTREND_RESET_REQUEST: // to ccur set/reset trend no.
      
         msg.setData(&(Interface->VTFlagMessage.data),
                     sizeof( Interface->VTFlagMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         vtf_data_ptr = (VTflagMessageData *)msg.getDataPtr();
         vtf_data_ptr->requestOption = htonl(vtf_data_ptr->requestOption);
         vtf_data_ptr->flipflop1 = htonl(vtf_data_ptr->flipflop1);
         vtf_data_ptr->flipflop2 = htonl(vtf_data_ptr->flipflop2);
         vtf_data_ptr->flipflop3 = htonl(vtf_data_ptr->flipflop3);
         vtf_data_ptr->flipflop4 = htonl(vtf_data_ptr->flipflop4);
         vtf_data_ptr->flipflop5 = htonl(vtf_data_ptr->flipflop5);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         break;            

	 //  from ccur
      case RECIEVE_VTREND_DATA:	// CCUR responded to REQUEST_VTREND_DATA by sending PIDs, etc.
#ifdef DEBUG
	 cout << " RECIEVE_VTREND_DATA" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 v_data_ptr = (VIDEO_DATA *)msg.getDataPtr(); // CORENET
	 for (i = 0; i < 20; i++) {
		if (i == 0) vt_mem_ptr = &(v_data_ptr->Set1Point1);
		else if (i == 1) vt_mem_ptr = &(v_data_ptr->Set1Point2);
		else if (i == 2) vt_mem_ptr = &(v_data_ptr->Set1Point3);
		else if (i == 3) vt_mem_ptr = &(v_data_ptr->Set1Point4);
		else if (i == 4) vt_mem_ptr = &(v_data_ptr->Set2Point1);
		else if (i == 5) vt_mem_ptr = &(v_data_ptr->Set2Point2);
		else if (i == 6) vt_mem_ptr = &(v_data_ptr->Set2Point3);
		else if (i == 7) vt_mem_ptr = &(v_data_ptr->Set2Point4);
		else if (i == 8) vt_mem_ptr = &(v_data_ptr->Set3Point1);
		else if (i == 9) vt_mem_ptr = &(v_data_ptr->Set3Point2);
		else if (i == 10) vt_mem_ptr = &(v_data_ptr->Set3Point3);
		else if (i == 11) vt_mem_ptr = &(v_data_ptr->Set3Point4);
		else if (i == 12) vt_mem_ptr = &(v_data_ptr->Set4Point1);
		else if (i == 13) vt_mem_ptr = &(v_data_ptr->Set4Point2);
		else if (i == 14) vt_mem_ptr = &(v_data_ptr->Set4Point3);
		else if (i == 15) vt_mem_ptr = &(v_data_ptr->Set4Point4);
		else if (i == 16) vt_mem_ptr = &(v_data_ptr->Set5Point1);
		else if (i == 17) vt_mem_ptr = &(v_data_ptr->Set5Point2);
		else if (i == 18) vt_mem_ptr = &(v_data_ptr->Set5Point3);
		else if (i == 19) vt_mem_ptr = &(v_data_ptr->Set5Point4);
		else break;
		swap32((char *)&(vt_mem_ptr->Minimum));
		swap32((char *)&(vt_mem_ptr->Baseline));
		swap32((char *)&(vt_mem_ptr->Maximum));
	 }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(DISPLAY_VTREND_DATA);

	 // copy data from message to shared memory 
	 msg.getData(&(Interface->VideoTrendData), sizeof(Interface->VideoTrendData));
	 
	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 Interface->VTFlagMessage.data.flipflop2 = 0;
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 //Interface->VTFlagMessage.data.flipflop2 = 0;
	 break;

//  to ccur
      case PROCESS_VTREND_DATA:	// Pictures sends new PIDs, etc. to CCUR
#ifdef DEBUG
	 cout << " PROCESS_VIDEO_DATA" << endl;
	 printf("Interface.Pid1= %s\n",Interface->VideoTrendSetupData.Point1.VideoTrendId);	 
#endif
	 msg.setData(&(Interface->VideoTrendSetupData), sizeof(Interface->VideoTrendSetupData));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 vsu_data_ptr = (VIDEO_SETUP_DATA *)msg.getDataPtr(); 
	 vsu_data_ptr->Setupno = htonl(vsu_data_ptr->Setupno); 
	 for (i = 0; i < 4; i++) {
		if (i == 0) vt_mem_ptr = &(vsu_data_ptr->Point1);
		else if (i == 1) vt_mem_ptr = &(vsu_data_ptr->Point2);
		else if (i == 2) vt_mem_ptr = &(vsu_data_ptr->Point3);
		else if (i == 3) vt_mem_ptr = &(vsu_data_ptr->Point4);
		else break;
		swap32((char *)&(vt_mem_ptr->Minimum));
		swap32((char *)&(vt_mem_ptr->Baseline));
		swap32((char *)&(vt_mem_ptr->Maximum));
	 }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 	 
	 //  from ccur
      case CONFIRM_VTREND_PROCESS:	// CCUR confirms receipt of new PIDs, etc. from Picture
#ifdef DEBUG
	 cout << " CONFIRM_VIDEO_PROCESS" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 grm_data_ptr = (WSresponseMessageData *)msg.getDataPtr(); 
	 grm_data_ptr->returnStatus = ntohl(grm_data_ptr->returnStatus); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 msg.getData(&(Interface->GenericResponseMessage.data), 
		     sizeof(Interface->GenericResponseMessage.data));

	 // Change the opcode to tell InterfaceToGUI to tell the picture 
	 //that the processing was confirmed.
	 msg.setOpcode(VTREND_PROCESS_CONFIRMED);

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 break;

      case CONFIRM_VTREND_FLAG:       // ccur confirms receipt data ..
         msg.setOpcode(DISPLAY_VTREND_RESET);
         
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         v_data_ptr = (VIDEO_DATA *)msg.getDataPtr(); // CORENET
         for (i = 0; i < 20; i++) {
                if (i == 0) vt_mem_ptr = &(v_data_ptr->Set1Point1);
                else if (i == 1) vt_mem_ptr = &(v_data_ptr->Set1Point2);
                else if (i == 2) vt_mem_ptr = &(v_data_ptr->Set1Point3);
                else if (i == 3) vt_mem_ptr = &(v_data_ptr->Set1Point4);
                else if (i == 4) vt_mem_ptr = &(v_data_ptr->Set2Point1);
                else if (i == 5) vt_mem_ptr = &(v_data_ptr->Set2Point2);
                else if (i == 6) vt_mem_ptr = &(v_data_ptr->Set2Point3);
                else if (i == 7) vt_mem_ptr = &(v_data_ptr->Set2Point4);
                else if (i == 8) vt_mem_ptr = &(v_data_ptr->Set3Point1);
                else if (i == 9) vt_mem_ptr = &(v_data_ptr->Set3Point2);
                else if (i == 10) vt_mem_ptr = &(v_data_ptr->Set3Point3);
                else if (i == 11) vt_mem_ptr = &(v_data_ptr->Set3Point4);
                else if (i == 12) vt_mem_ptr = &(v_data_ptr->Set4Point1);
                else if (i == 13) vt_mem_ptr = &(v_data_ptr->Set4Point2);
                else if (i == 14) vt_mem_ptr = &(v_data_ptr->Set4Point3);
                else if (i == 15) vt_mem_ptr = &(v_data_ptr->Set4Point4);
                else if (i == 16) vt_mem_ptr = &(v_data_ptr->Set5Point1);
                else if (i == 17) vt_mem_ptr = &(v_data_ptr->Set5Point2);
                else if (i == 18) vt_mem_ptr = &(v_data_ptr->Set5Point3);
                else if (i == 19) vt_mem_ptr = &(v_data_ptr->Set5Point4);
                else break;
                swap32((char *)&(vt_mem_ptr->Minimum));
                swap32((char *)&(vt_mem_ptr->Baseline));
                swap32((char *)&(vt_mem_ptr->Maximum));
         }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // copy data from message to shared memory  // added by yrlee 19990915
	 msg.getData(&(Interface->VideoTrendData), sizeof(Interface->VideoTrendData));

         Interface->VTFlagMessage.data.flipflop2 = 1;
         Interface->System.receivedData = TRUE;
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
         //Interface->System.receivedData = TRUE;
         break;
         
      default:
	   
	 cout << "parseVTRENDMessages: Received Unknown Message, opcode: " 
	      << hex << msg.getOpcode() << endl;

   } // end of opcode switch statement
}
