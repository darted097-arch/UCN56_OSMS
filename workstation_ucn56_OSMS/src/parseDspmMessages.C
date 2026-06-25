//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseDspmMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  This module processes the DSPM messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// **************************************************************************************
//
// **************************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
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

// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//      
//
// **************************************************************************************
//
void parseDspmMessages(messageObject & Msg)
{   
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
        WSconfirmMessageData * confirm_data_ptr;
   	WSrequestMessageData *grm_data_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {
     // to ccur
     case REQUEST_DSPM_REPORT:
          //get the host name to select the proper printer
          gethostname(Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
#ifdef DEBUG
          printf("hostname=%s\n",Interface->GenericRequestMessage.data.theWorkstation);
          printf("DSPM option=%d\n",Interface->GenericRequestMessage.data.requestOption);
          cout <<" request DSPM reports " <<endl;
#endif
          // set the data portion of the message from the shared memory
	  //Interface structure
	  Msg.setData(&(Interface->GenericRequestMessage.data),
		     sizeof(Interface->GenericRequestMessage.data));

#ifdef DEBUG
          cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
          grm_data_ptr = (WSrequestMessageData *)Msg.getDataPtr(); 
          grm_data_ptr->requestOption = htonl(grm_data_ptr->requestOption); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  break;
	
      // from ccur
    case ERROR_QUEUE_TASK:
         cout << " task queue has failed" << endl;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)Msg.getDataPtr();
         confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // get the message data into the Interface structure
	 Msg.getData(&(Interface->GenericConfirmMessage.data),
		     sizeof(Interface->GenericConfirmMessage.data));

         // send the message to InterfaceToGUI
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 printf("DSPM ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
	 break;
	
    case CONFIRM_CPC_REPORT:
#ifdef DEBUG
         cout <<"CPC REPORT HAS SUCCESSFULLY PRINTED!"<<endl;
#endif
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1.

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
         
    case CONFIRM_CEAC_REPORT:
#ifdef DEBUG
         cout <<"CEAC REPORT HAS SUCCESSFULLY PRINTED!"<<endl;
#endif

// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
             
    case CONFIRM_PPS_REPORT:
#ifdef DEBUG
         cout <<"PPS REPORT HAS SUCCESSFULLY PRINTED!"<<endl;
#endif
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
             
    case CONFIRM_CPC_TRP_RPT:
#ifdef DEBUG
         cout <<"CPC TRIP REPORT HAS SUCCESSFULLY PRINTED!"<<endl;
#endif
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
             
    case CONFIRM_CEAC_TRP_RPT:
#ifdef DEBUG
         cout <<"CPC TRP REPORT HAS SUCCESSFULLY PRINTED!"<<endl;
#endif
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
         
    case ERROR_OPEN_PRINTER:
#ifdef DEBUG
         cout <<"opening printer has failed"<<endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)Msg.getDataPtr();
         confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // get the data from the message into the Interface structure
	 Msg.getData(&(Interface->GenericConfirmMessage.data), 
		     sizeof(Interface->GenericConfirmMessage.data));

         // put the message on the InterfaceToGUI message queue
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

#ifdef DEBUG
         printf("DSPM ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
    
    case CONFIRM_QUEUE_DSPMRPT:
#ifdef DEBUG
         cout <<"DSPMRPT task has successfully queued by selection 3!"<<endl;
#endif
// NOTE: The opcode should be changed to an InterfaceToGUI opcode before puttine
//      the message on the queue.  This may work for now, but messages from the 
//      host and messages to the host are identified by the high order nibble, 0 or 1. LEO

         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
         break;
             
    default:
	   
         cout << "parseDspmMessages: Received Unknown Message, opcode: " 
              << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
