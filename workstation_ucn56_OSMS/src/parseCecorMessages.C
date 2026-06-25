//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseCecorMessages.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  This module processes the cecor messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
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
// ***************************************************************************
//
void parseCecorMessages(messageObject & msg)
{
#ifdef DEBUG
   union {
      WSmessageTemplate		message;
      WSCecorMessage            CecorMsg;
   } Local;
#endif   
   
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	CECOR_DATA * cecor_data_ptr; 
	WSconfirmMessageData * confirm_data_ptr;	
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   switch (msg.getOpcode())
   {
      // to ccur
      case REQUEST_CECOR_SNAPSHOT:
         //get the host name to select the proper printer
//          gethostname(Interface->GenericConfirmMessage.data.theWorkstation,LEN_SYSTEMNAME);
//          printf("hostname=%s\n",Interface->GenericConfirmMessage.data.theWorkstation);
#ifdef DEBUG
         printf("CECOR option=%d\n",Interface->CecorData.cecorOption);
         printf("CECOR filename=%s\n",Interface->CecorData.cecorFilename);
         printf("CECOR comments=%s\n",Interface->CecorData.cecorComments);
         cout <<" sending cecor option and filename and comments " <<endl;
#endif
	 // copy the shared memory Interface data into the message
	 msg.setData(&(Interface->CecorData), sizeof(Interface->CecorData));

#ifdef DEBUG
         msg.getMessage(&Local.message, sizeof(Local.message));
         cout << "opcode= " << hex << msg.getOpcode() << endl;
         printf("CECOR option in message is=%d\n",Local.CecorMsg.data.cecorOption);
         printf("CECOR filename in message is=%s\n",Local.CecorMsg.data.cecorFilename);
         printf("CECOR comments=%s\n",Local.CecorMsg.data.cecorComments);          
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         cecor_data_ptr = (CECOR_DATA *)msg.getDataPtr(); // CORENET
         cecor_data_ptr->cecorOption = htonl(cecor_data_ptr->cecorOption); // CORENET
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  break;
	
    // from ccur
    case CONFIRM_CECOR_DISKFILE:
#ifdef DEBUG
         cout << "CECOR snapshot disk file is completed" << endl;
#endif
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
	 break;
	  
    // from ccur
    case CONFIRM_CECOR_TAPE:
#ifdef DEBUG
         cout << "CECOR snapshot tape is completed" << endl;
#endif
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
         break;  

    // from ccur
    case ERROR_QUEUE_IOCPCS:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
         confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout << " queue IOCPCS task failed" << endl;
#endif
	 // copy the data from the message into the shared memory area
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));

	 // put the message onto the InterfaceToGUI message queue
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());

#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
	
    case ERROR_CECOR_FILEOPEN:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
	 confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout <<"cecor file open error"<<endl;
#endif
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
    
    case ERROR_CECOR_MAG1OPEN:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
	 confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout <<"mag tape open error"<<endl;
#endif
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
     
    case ERROR_CECOR_DISKREAD:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
	 confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout << " disk read error" << endl;
#endif
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
	  
    case ERROR_TAPE_NOT_MOUNT:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
	 confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout << " tape not mounted" << endl;
#endif
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
    
    case ERROR_CECOR_MAG1WRITE:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr(); 
	 confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 
#ifdef DEBUG
         cout <<"writing to mag tape failed"<<endl;
#endif
	 msg.getData(&(Interface->GenericConfirmMessage.data), 
		      sizeof(Interface->GenericConfirmMessage.data));
         IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(),msg.getType());
#ifdef DEBUG
         printf("CECOR ERROR MESSAGE IS %s\n",Interface->GenericConfirmMessage.data.message1);
#endif
	 break;
    

    default:
	   
         cout << "parseCecorMessages: Received Unknown Message, opcode: " 
                   << hex << msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
