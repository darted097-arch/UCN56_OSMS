//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseXRBPMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the XRBP messages.  This includes
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
//
// Local Definitions
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject  IGUI_MsgQue; // CommLink to InterfaceToGUI message queue

// Prototype Definitions:
   
//
// **************************************************************************************
//
void parseXRBPMessages(messageObject & msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   XRBP_INPUT_TEMPLATE	*xrbpalldata_ptr;	
   WSconfirmMessageData * confirm_data_ptr;
   WSrequestMessageData *grq_data_ptr;
   int j;	
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG
   union {
      WSmessageTemplate		message;
      XRBP_INPUT_DATA		XRBP_inputs;
   } Local;
   int    i;
#endif
      
    //int i;   

   switch(msg.getOpcode())
   {
      
      case XRBP_INIT:
#ifdef DEBUG
	   cout <<"request XRBP_INIT: the Opcode=" << msg.getOpcode() << endl;
#endif	   
	   // copy data from shared memory to message
	      msg.setData(&(Interface->GenericRequestMessage.data),
 	                   sizeof(Interface->GenericRequestMessage.data));

           //printf("W/S send XRBP_INIT (0x1841) to CCUR\n"); 	                   

	   // Then Global buffer is ready to send to Host.

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
	   cout << "XRBP INIT prepared: theOpcode is" << msg.getOpcode() 
		<< endl;
#endif	   
	   break;

      case XRBP_REQUEST:
           // Copy the XRBP input Parameters to the message
	      msg.setData(&(Interface->XRBP.data),sizeof(Interface->XRBP.data));

           //   printf("W/S sent the following Data to CCUR (XRBP_REQUEST, 0x1840)\n");
           //   for(i=0;i<21;i++)
           //   {
           //        printf("input_val[i] = %f\n",i,Interface->XRBP.data.input_val[i]); 
           //   }
           //     printf("last_case = %d\n",Interface->XRBP.data.last_case);

#ifdef DEBUG
	   msg.getMessage(&Local.message, sizeof(Local.message));
	   cout << "parseXRBP: Request prepared: theOpcode = " << msg.getOpcode() 
		<< endl;
           cout << "parseXRBP: cs_option = " <<dec<< Local.XRBP_inputs.data.cs_option << endl;
	   for (i=0;i<23;i++){
               cout << "parseXRBP: input_val["<<dec<<i<<"] = "<<dec
		    << Local.XRBP_inputs.data.input_val[i]
		    <<endl;
           }
	   cout << "parseXRBP: last_case = " << Local.XRBP_inputs.data.last_case << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   xrbpalldata_ptr = (XRBP_INPUT_TEMPLATE *)msg.getDataPtr();
	   for (j = 0; j < 21; j++) swap32((char *)&(xrbpalldata_ptr->input_val[j]));
	   xrbpalldata_ptr->last_case = htonl(xrbpalldata_ptr->last_case);
	   xrbpalldata_ptr->error_code = htonl(xrbpalldata_ptr->error_code);
	   for (j = 0; j < 13; j++) swap32((char *)&(xrbpalldata_ptr->output_val[j]));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

           break;

      case XRBP_READY:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   xrbpalldata_ptr = (XRBP_INPUT_TEMPLATE *)msg.getDataPtr();
	   for (j = 0; j < 21; j++) swap32((char *)&(xrbpalldata_ptr->input_val[j]));
	   xrbpalldata_ptr->last_case = ntohl(xrbpalldata_ptr->last_case);
	   xrbpalldata_ptr->error_code = ntohl(xrbpalldata_ptr->error_code);
	   for (j = 0; j < 13; j++) swap32((char *)&(xrbpalldata_ptr->output_val[j]));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   // copy the message data to the interface structure -> GUI
	      msg.getData(&(Interface->XRBP.data), sizeof(Interface->XRBP.data));

            //  printf("W/S received the following Data from CCUR (XRBP_READY, 0x0841)\n");
            //  for(i=0;i<21;i++){
            //       printf("input_val[%d] = %f\n",i,Interface->XRBP.data.input_val[i]); }
            //    printf("last_case = %d\n",Interface->XRBP.data.last_case);
      
           //
           // Now Host has been initialized and we got the input data for GUI initialization.
           // The structures used here makes confusion because we use same structures 
	   // for data request to host but it is because GUI requires input data from host
	   // for screen initialization first time.
	   //
#ifdef DEBUG
           cout << "parseXRBP: received XRBP indata from Host" << endl;
	   cout << "parseXRBP: the Opcode =" <<dec << Interface->XRBP.header.opcode<<endl;
           cout << "parseXRBP: cs_option = " <<dec<< Interface->XRBP.inputs.cs_option << endl;
	   for (i=0;i<23;i++){
               cout << "parseXRBP: input_val["<<dec<<i<<"] = "<<dec
		    << Interface->XRBP.inputs.input_val[i]
		    <<endl;
           }
	   cout << "parseXRBP: last_case = " << dec 
	        << Interface->XRBP.inputs.last_case << endl;
#endif
           // Send the message with the new opcode to InterfaceToGUI
           // InterfaceToGUI will process that new message through parsePictureMessages.C
              IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	   break;

      case XRBP_MSG:
           {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            confirm_data_ptr = (WSconfirmMessageData *)msg.getDataPtr();
            confirm_data_ptr->status = ntohl(confirm_data_ptr->status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            msg.getData(&(Interface->GenericConfirmMessage.data), sizeof(Interface->GenericConfirmMessage.data));
            //IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
            
	    // terminates messages with NULL character
         
	    Interface->GenericConfirmMessage.data.message1[39] = '\0';
	    Interface->GenericConfirmMessage.data.message2[39] = '\0';
	    Interface->GenericConfirmMessage.data.message3[39] = '\0';

            // for Test   
            //printf("W/S received XRBP Messges from CCUR\n");
            //printf("Mes 1 : %s\n",Interface->GenericConfirmMessage.data.message1);
            //printf("Mes 2 : %s\n",Interface->GenericConfirmMessage.data.message2);
            //printf("Mes 3 : %s\n",Interface->GenericConfirmMessage.data.message3);

            IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	    
            break;
	    } 

      case XRBP_DATA:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   xrbpalldata_ptr = (XRBP_INPUT_TEMPLATE *)msg.getDataPtr();
	   for (j = 0; j < 21; j++) swap32((char *)&(xrbpalldata_ptr->input_val[j]));
	   xrbpalldata_ptr->last_case = ntohl(xrbpalldata_ptr->last_case);
	   xrbpalldata_ptr->error_code = ntohl(xrbpalldata_ptr->error_code);
	   for (j = 0; j < 13; j++) swap32((char *)&(xrbpalldata_ptr->output_val[j]));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   // copy the message to the interface structure -> GUI
	      msg.getData(&(Interface->XRBP.data), sizeof(Interface->XRBP.data));
	      
           // printf("W/S received the following Data from CCUR (XRBP_DATA, 0x0840)\n");
           //   for(i=0;i<21;i++){
           //       printf("input_val[%d] = %f\n",i,Interface->XRBP.data.input_val[i]); }
           //     printf("last_case = %d\n",Interface->XRBP.data.last_case);
                    
           //   printf("error_code = %d\n",Interface->XRBP.data.error_code);
           //  for(i=0;i<13;i++){
           //      printf("output_val[%d] = %f\n",i,Interface->XRBP.data.output_val[i]); }

           // Now host data has been arrived at workstation
#ifdef DEBUG
	   cout << "parseXRBP: received XRBP Output Data from Host: theOpcode = " 
		<< Interface->XRBP.header.opcode 
		<< endl;
           cout << "parseXRBP: error_code = " <<dec<< Interface->XRBP.outputs.error_code
		<< endl;
	   for (i=0;i<13;i++){
               cout << "parseXRBP: output_val["<<dec<<i<<"] = "<<dec
		    << Interface->XRBP.outputs.output_val[i]
		    <<endl;
           }
#endif
           // Send the message with the new opcode to InterfaceToGUI
           // InterfaceToGUI will process that new message through parsePictureMessages.C
              IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	   break;
   }
}
