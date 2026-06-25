//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parsePrreptMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the PRINT REPORTS messages.  This includes
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
#include "messageObject.h"		//
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue
//
// Local Constant Definitions: NONE
//
// Function Prototypes: NONE
//      
//
// ***************************************************************************
//
void parsePrreptMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 WSrequestMessageData  *grqm_data_ptr; // 
   	 WSresponseMessageData *grsm_data_ptr; // 
	 TRIP_RECORD	*trip_record_ptr;
	 SOE_RECORD	*soe_record_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {

     case REQUEST_PRREPT:
          //get the host name to select the proper printer
          gethostname(Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
#ifdef DEBUG
          printf("hostname=%s\n",Interface->GenericRequestMessage.data.theWorkstation);
          printf("option=%d\n",Interface->GenericRequestMessage.data.requestOption);
#endif
          cout <<" request Print reports " <<endl;
	  Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
#ifdef DEBUG
          cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grqm_data_ptr = (WSrequestMessageData *)Msg.getDataPtr(); // 
         grqm_data_ptr->requestOption = htonl(grqm_data_ptr->requestOption); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  break;
	
     case REQUEST_POSTRIP_PRINT:
          //get the host name to select the proper printer
          gethostname(Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
#ifdef DEBUG
          printf("hostname=%s\n",Interface->GenericRequestMessage.data.theWorkstation);
          printf("option=%d\n",Interface->GenericRequestMessage.data.requestOption);
#endif
          cout <<" request selected postrip report " <<endl;
	  Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
#ifdef DEBUG
          cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grqm_data_ptr = (WSrequestMessageData *)Msg.getDataPtr(); // 
         grqm_data_ptr->requestOption = htonl(grqm_data_ptr->requestOption); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  break;
	
     case REQUEST_SOE_PRINT:
          //get the host name to select the proper printer
          gethostname(Interface->GenericRequestMessage.data.theWorkstation,LEN_SYSTEMNAME);
#ifdef DEBUG
          printf("hostname=%s\n",Interface->GenericRequestMessage.data.theWorkstation);
          printf("option=%d\n",Interface->GenericRequestMessage.data.requestOption);
#endif
          cout <<" request selected soe report " <<endl;
	  Msg.setData(&(Interface->GenericRequestMessage.data),sizeof(Interface->GenericRequestMessage.data));
#ifdef DEBUG
          cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grqm_data_ptr = (WSrequestMessageData *)Msg.getDataPtr(); // 
         grqm_data_ptr->requestOption = htonl(grqm_data_ptr->requestOption); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  break;
	  
    case DEMANDLOG_PRINT_CONFIRMED:
         cout << " confirm DEMAND LOG print" << endl;
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break;

    case SEND_POSTRIP_DATA:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         trip_record_ptr = (TRIP_RECORD *)Msg.getDataPtr(); // 
         trip_record_ptr->TripNo = htonl(trip_record_ptr->TripNo); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         cout <<"send postrip data"<<endl;
	 Msg.setOpcode(DISPLAY_POSTRIP_DATA);
	 Msg.getData(&(Interface->TripRecord),sizeof(Interface->TripRecord));         
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
#ifdef DEBUG
         printf("Trip Record1 is: %s\n",Interface->TripRecord.TripRecord1);
         printf("Trip Record2 is: %s\n",Interface->TripRecord.TripRecord2);
         printf("Trip Record3 is: %s\n",Interface->TripRecord.TripRecord3);
         printf("Trip Record4 is: %s\n",Interface->TripRecord.TripRecord4);
         printf("Trip Record5 is: %s\n",Interface->TripRecord.TripRecord5);
         printf("Total trip no is: %d\n",Interface->TripRecord.TripNo);
#endif
         break;
    
    case POSTRIP_PRINT_CONFIRMED:
         cout << " confirm selected postrip print" << endl;
         Interface->PrreptDynMsg.PrreptDynMsg1[0]='\0';
         Interface->PrreptDynMsg.PrreptDynMsg2[0]='\0';         
	 Msg.getData(&(Interface->PrreptDynMsg),sizeof(Interface->PrreptDynMsg)); 
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break; 
	  
    case SEND_SOE_DATA:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         soe_record_ptr = (SOE_RECORD *)Msg.getDataPtr(); // 
         soe_record_ptr->SoeNo = htonl(soe_record_ptr->SoeNo); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         cout <<"send soe data"<<endl;
	 Msg.setOpcode(DISPLAY_SOE_DATA);
	 Msg.getData(&(Interface->SoeRecord),sizeof(Interface->SoeRecord));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
#ifdef DEBUG
         printf("Soe Record0 is: %s\n",Interface->SoeRecord.SoeRecord0);
         printf("Soe Record1 is: %s\n",Interface->SoeRecord.SoeRecord1);
         printf("Soe Record2 is: %s\n",Interface->SoeRecord.SoeRecord2);
         printf("Soe Record3 is: %s\n",Interface->SoeRecord.SoeRecord3);         
         printf("Soe Record4 is: %s\n",Interface->SoeRecord.SoeRecord4);
         printf("Soe Record5 is: %s\n",Interface->SoeRecord.SoeRecord5);
         printf("Soe Record6 is: %s\n",Interface->SoeRecord.SoeRecord6);
         printf("Soe Record7 is: %s\n",Interface->SoeRecord.SoeRecord7);            
         printf("Soe Record8 is: %s\n",Interface->SoeRecord.SoeRecord8);
         printf("Soe Record9 is: %s\n",Interface->SoeRecord.SoeRecord9); 
         printf("Total soe no is: %d\n",Interface->SoeRecord.SoeNo);
#endif         
         break;
   
    case SOE_PRINT_CONFIRMED:
         cout << " confirm selected soe print" << endl;
         Interface->PrreptDynMsg.PrreptDynMsg1[0]='\0';
         Interface->PrreptDynMsg.PrreptDynMsg2[0]='\0';           
	 Msg.getData(&(Interface->PrreptDynMsg),sizeof(Interface->PrreptDynMsg)); 
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break; 
	 
    case LASTPOSTRIP_PRINT_CONFIRMED:
  	 cout << " confirm last postrip print" << endl;
         Interface->PrreptDynMsg.PrreptDynMsg1[0]='\0';
         Interface->PrreptDynMsg.PrreptDynMsg2[0]='\0';        	 
	 Msg.getData(&(Interface->PrreptDynMsg),sizeof(Interface->PrreptDynMsg));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break; 
	 
    case LASTSOE_PRINT_CONFIRMED:  
         cout << " confirm last soe print" << endl;
         Interface->PrreptDynMsg.PrreptDynMsg1[0]='\0';
         Interface->PrreptDynMsg.PrreptDynMsg2[0]='\0';                 
	 Msg.getData(&(Interface->PrreptDynMsg),sizeof(Interface->PrreptDynMsg));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break; 
	 
    default:
	   
         cout << "parsePrreptMessages: Received Unknown Message, opcode: " 
              << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
