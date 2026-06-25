//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseRPCCEAMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the RPCCEA messages.  This includes
//			both messages from the host to the workstation and messages from
//			the workstation to the host.
// **************************************************************************************
//													
// **************************************************************************************
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

//													
// **************************************************************************************
//													
// Prototype Definitions:
   
void parseRPCCEAMessages(messageObject & Msg)
{
   union {
      WSmessageTemplate		message;
      WSconfirmMessage		Generic_Confirm;
} Local;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSrequestMessageData *grq_data_ptr;
   WSconfirmMessageData *cfm_data_ptr;
   RPCCEAModeData	*rpcceadata_ptr;	
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // structures for processing of RPCCEA confirm message from the host
   struct WSconfirmMessageData *pLocalConfirm     = &Local.Generic_Confirm.data;
   struct WSconfirmMessageData *pInterfaceConfirm = &Interface->GenericConfirmMessage.data; 
   
    // copy the global data buffer into the local version
   Msg.getMessage(&Local.message,sizeof(Local.message));

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   switch (Msg.getOpcode())
   {
      
//*************************************************************************************************************
//   CEA Option-7: RPCCEA Selection
//*************************************************************************************************************
      // >>>>>>>>>>>>>>>>>>>> RPCCEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request
        case RPCCEA_MODE_REQUEST: 
	{
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
#ifdef DEBUG
    	  cout << "send RPCCEA_MODE_REQUEST " << endl;
#endif
	
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
         grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
      	}
      // >>>>>>>>>>>>>>>>>>>> RPCCEA Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<
        case RPCCEA_CURRENT_MODE:
	 {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   rpcceadata_ptr = (RPCCEAModeData *)Msg.getDataPtr();
	   rpcceadata_ptr->mode = ntohl(rpcceadata_ptr->mode);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    Msg.getData(&(Interface->RPCCEACurrentModeMessage.data),sizeof(Interface->RPCCEACurrentModeMessage.data));
#ifdef DEBUG
    	    cout << "Received RPCCEA_CURRENT_MODE " << endl;
#endif
            break;
	 }
      // >>>>>>>>>>>>>>>>>>>> RPCCEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
	 // the selected option is to be transmitted
	case RPCCEA_PROCESS_REQUEST:
         {
	    // get the host name to select the proper printer name
	    gethostname (Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
  	    Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    break;
         }
      // >>>>>>>>>>>>>>>>>>>> RPCCEA Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<
        case RPCCEA_PROCESS_CONFIRM:
	 {
	    // terminates messages with NULL character
         
	    Local.Generic_Confirm.data.message1[39] = '\0';
	    Local.Generic_Confirm.data.message2[39] = '\0';
	    Local.Generic_Confirm.data.message3[39] = '\0';
            Interface->RPCCEACurrentModeMessage.data.mode =  
	       Local.Generic_Confirm.data.status; //to assign current running mode
	    *pInterfaceConfirm = *pLocalConfirm;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    Interface->RPCCEACurrentModeMessage.data.mode =
	    	ntohl(Interface->RPCCEACurrentModeMessage.data.mode);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    	    cout << "Received RPCCEA_PROCESS_CONFIRM opcode" << endl;
	    Msg.setOpcode(GENERIC_PROCESSING_CONFIRMED);
	    // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C
	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	    break;
	 }

//*************************************************************************************************************
//   CEA Option-1: CEA Alarm and Limits Display
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking CEA Alarm and Limit Display Data
        case CEA_ALM_LIMIT_REQUEST: 
	{
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	  cout << "send CEA_ALM_LIMIT_REQUEST " << endl;
#endif
	 break;
	 }


        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent to the GUI from the Host <<<<<<<<<<<<<<<<<<<<
        case CEA_ALM_LIMIT_PROCESS:
	 {
	    Msg.getData(&(Interface->CEAAlarmLimitData.data),sizeof(Interface->CEAAlarmLimitData.data));
#ifdef DEBUG
    	    cout << "Received CEA Alarm and Limits Data grom the Host " << endl;
#endif
            break;
	 }

//*************************************************************************************************************
//   CEA Option-2: One CEA Position Reports
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking One CEA Position Reports
        case ONE_CEA_POSITION_REPT: 
	{
	 // get the host name to select the proper printer name
	 gethostname  (Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	 cout << "send ONE_CEA_POSITION_REPT request " << endl;
#endif
	 break;
	 }


//*************************************************************************************************************
//   CEA Option-3: One CEA Group Position Reports
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking One CEA Position Reports
        case ONE_CEA_GROUP_POSITION_REPT: 
	{
	 // get the host name to select the proper printer name
	 gethostname  (Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	 cout << "send ONE_CEA_GROUP_POSITION_REPT request " << endl;
#endif
	 break;
	 }


//*************************************************************************************************************
//   CEA Option-4: All CEA Group Position Reports
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking One CEA Position Reports
        case ALL_CEAGROUP_POSITION_REPT:
	{
	 // get the host name to select the proper printer name
	 gethostname  (Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	 cout << "send ALL_CEAGROUP_POSITION_REPT request " << endl;
#endif
	 break;
	 }


//*************************************************************************************************************
//   CEA Option-5: CEA Drop Testing 
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking One CEA Position Reports
        case CEA_DROPNO_REQUEST: 
	{
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	 cout << "send CEA_DROPNO_REQUEST CEA selection number " << endl;
#endif
	 break;
	 }


//*************************************************************************************************************
//   CEA Option-6: CEDMCS Data Transfer 
//*************************************************************************************************************
        // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
        //  No data is needed, just need the opcode to send request asking One CEA Position Reports
        case CEDMCS_DATAXFER_MODE: 
	{
	 Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grq_data_ptr = (WSrequestMessageData *)Msg.getDataPtr();
            grq_data_ptr->requestOption = htonl(grq_data_ptr->requestOption);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
    	 cout << "send CEDMCS_DATAXFER_MODE selection " << endl;
#endif
	 break;
	 }

      // >>>>>>>>>>>>>>>>>>>> CEA Messages sent from the HOST to the GUI <<<<<<<<<<<<<<<<<<<<
      // >>>>>>> This "case" block will be used for confirmation for other option also.<<<<<<<<<
        case CEA_GENERIC_PROCESS_CONFIRMED:
	 {
	    // terminates messages with NULL character
       
	    Local.Generic_Confirm.data.message1[39] = '\0';
	    Local.Generic_Confirm.data.message2[39] = '\0';
	    Local.Generic_Confirm.data.message3[39] = '\0';

	 //printf("cea confirm data opcode = %d  \n",Local.Generic_Confirm.header.opcode);
	 //printf("cea confirm data numberOfSomething = %d  \n",Local.Generic_Confirm.header.numberOfSomething);
	 //printf("cea confirm data dateAndTime = %s  \n",Local.Generic_Confirm.header.dateAndTime);
	 //printf("cea confirm data spareHeader = %s  \n",Local.Generic_Confirm.header.spareHeader);
	 //printf("cea confirm data wscommName = %s  \n",Local.Generic_Confirm.header.wscommName);

	 //printf("cea confirm data status = %d  \n",Local.Generic_Confirm.data.status);
 	 //printf("cea confirm message1= %s  \n",Local.Generic_Confirm.data.message1);
	 //printf("cea confirm message2= %s  \n",Local.Generic_Confirm.data.message2);
	 //printf("cea confirm message3= %s  \n",Local.Generic_Confirm.data.message3);

            Interface->RPCCEACurrentModeMessage.data.mode =  
	       Local.Generic_Confirm.data.status; //to assign current running mode
	    *pInterfaceConfirm = *pLocalConfirm;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    Interface->RPCCEACurrentModeMessage.data.mode =
	    	ntohl(Interface->RPCCEACurrentModeMessage.data.mode);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    	    cout << "Received CEA_GENERIC_PROCESS_CONFIRMED opcode" << endl;
	    Msg.setOpcode(GENERIC_PROCESSING_CONFIRMED);

	    // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C

	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	    break;
         }

        case CEA_MESSAGE_IN_BODY:
	 {
	    // terminates messages with NULL character
       
	    Local.Generic_Confirm.data.message1[39] = '\0';
	    Local.Generic_Confirm.data.message2[39] = '\0';
	    Local.Generic_Confirm.data.message3[39] = '\0';
	    Local.Generic_Confirm.data.message4[39] = '\0';
	    Local.Generic_Confirm.data.message5[39] = '\0';

	 //printf("cea confirm data opcode = %d  \n",Local.Generic_Confirm.header.opcode);
	 //printf("cea confirm data numberOfSomething = %d  \n",Local.Generic_Confirm.header.numberOfSomething);
	 //printf("cea confirm data dateAndTime = %s  \n",Local.Generic_Confirm.header.dateAndTime);
	 //printf("cea confirm data spareHeader = %s  \n",Local.Generic_Confirm.header.spareHeader);
	 //printf("cea confirm data wscommName = %s  \n",Local.Generic_Confirm.header.wscommName);

	 printf("cea confirm data status = %d  \n",Local.Generic_Confirm.data.status);
 	 printf("cea confirm message1= %s  \n",Local.Generic_Confirm.data.message1);
	 printf("cea confirm message2= %s  \n",Local.Generic_Confirm.data.message2);
	 printf("cea confirm message3= %s  \n",Local.Generic_Confirm.data.message3);

            Interface->RPCCEACurrentModeMessage.data.mode =  
	       Local.Generic_Confirm.data.status; //to assign current running mode
	    *pInterfaceConfirm = *pLocalConfirm;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    Interface->RPCCEACurrentModeMessage.data.mode =
	    	ntohl(Interface->RPCCEACurrentModeMessage.data.mode);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    	    cout << "Received CEA_MESSAGE_IN_BODY opcode" << endl;
	    Msg.setOpcode(CEA_MESSAGE_IN_BODY);

	    // Send the message with the new opcode to InterfaceToGUI
	    // InterfaceToGUI will process that new confirm message through parsePictureMessages.C

	    IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         }
	 
   } // end of opcode switch statement

}
