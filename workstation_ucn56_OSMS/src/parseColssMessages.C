//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseColssMessages.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  This module processes the Colss messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
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
int i;
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue
        Colss_Data	colssData;
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//      
//
// **************************************************************************************
void parseColssMessages(messageObject & msg)
{
	
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   Colss_Data * colss_data_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
	 //  to ccur
      case REQUEST_COLSS_PROCESSING:	// Pictures requests to process  to CCUR
 	  cout << " process_colss_data" << endl; 
       	  cout << " process_colss_data" << endl; 
          //get the host name to select the proper printer
          gethostname(Interface->ColssData.WsName,LEN_SYSTEMNAME);
          printf("hostname=%s\n",Interface->ColssData.WsName);
          
	 // copy the shared memory data into the message
	 msg.setData(&(Interface->ColssData), sizeof(Interface->ColssData));
	    printf("COLSS init value = %f , \n",Interface->ColssData.InitData.value_initialized);
	    printf("COLSS init sample value = %d , \n",Interface->ColssData.InitData.sample_value);

 	 // to see contents of request message
 
	    printf("COLSS message type = %d , \n",Interface->ColssData.message_type);
	    cout << "COLSS Message type = " << hex << Interface->ColssData.message_type << endl;

            if(Interface->ColssData.message_type == CONFIRM_COLSS_CURRENT_MODE) 
	       printf("COLSS requested mode = %d , \n",Interface->ColssData.ModeData.ws_to_host_mode);	 
	 		
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           colss_data_ptr = (Colss_Data *)msg.getDataPtr();
           colss_data_ptr->message_type = htonl(colss_data_ptr->message_type);
           colss_data_ptr->plot_status = htonl(colss_data_ptr->plot_status);
           colss_data_ptr->selected_page = htonl(colss_data_ptr->selected_page);
           colss_data_ptr->InitData.selected_option = htonl(colss_data_ptr->InitData.selected_option);
           colss_data_ptr->InitData.sample_value = htonl(colss_data_ptr->InitData.sample_value);
           swap32((char *)&(colss_data_ptr->InitData.value_initialized));
           colss_data_ptr->ModeData.ws_to_host_mode = htonl(colss_data_ptr->ModeData.ws_to_host_mode);
           colss_data_ptr->ModeData.colss_current_mode = htonl(colss_data_ptr->ModeData.colss_current_mode);
           colss_data_ptr->ModeData.test_case = htonl(colss_data_ptr->ModeData.test_case);
           colss_data_ptr->ModeData.use_burnup_file = htonl(colss_data_ptr->ModeData.use_burnup_file);
           colss_data_ptr->ModeData.print_option = htonl(colss_data_ptr->ModeData.print_option);
           colss_data_ptr->ModeData.security_level = htonl(colss_data_ptr->ModeData.security_level);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 	 
	 //  from ccur
      case CONFIRM_COLSS_PROCESSING:	// CCUR confirms receipt of  from Picture

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           colss_data_ptr = (Colss_Data *)msg.getDataPtr();
           colss_data_ptr->message_type = ntohl(colss_data_ptr->message_type);
           colss_data_ptr->plot_status = ntohl(colss_data_ptr->plot_status);
           colss_data_ptr->selected_page = ntohl(colss_data_ptr->selected_page);
           colss_data_ptr->InitData.selected_option = ntohl(colss_data_ptr->InitData.selected_option);
           colss_data_ptr->InitData.sample_value = ntohl(colss_data_ptr->InitData.sample_value);
           swap32((char *)&(colss_data_ptr->InitData.value_initialized));
           colss_data_ptr->ModeData.ws_to_host_mode = ntohl(colss_data_ptr->ModeData.ws_to_host_mode);
           colss_data_ptr->ModeData.colss_current_mode = ntohl(colss_data_ptr->ModeData.colss_current_mode);
           colss_data_ptr->ModeData.test_case = ntohl(colss_data_ptr->ModeData.test_case);
           colss_data_ptr->ModeData.use_burnup_file = ntohl(colss_data_ptr->ModeData.use_burnup_file);
           colss_data_ptr->ModeData.print_option = ntohl(colss_data_ptr->ModeData.print_option);
           colss_data_ptr->ModeData.security_level = ntohl(colss_data_ptr->ModeData.security_level);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

 	 cout << " confirm_colss_processing" << endl;
 	 
         msg.getData(&(colssData),   sizeof(Colss_Data));
	 if(colssData.message_type != CONFIRM_COLSS_CPUC_PRINT)
            msg.getData(&(Interface->ColssData),   sizeof(Colss_Data));
	    printf("COLSS init value from ccur = %f , \n",Interface->ColssData.InitData.value_initialized);
	    printf("COLSS init sample value from ccur= %d , \n",Interface->ColssData.InitData.sample_value);
	 printf("COLSS  current mode= %d  \n",Interface->ColssData.message_type);	
	
	 printf("COLSS  mode change from ws = %d \n",Interface->ColssData.message_type);	
	
	 printf("COLSS  mode from ccur = %d , \n",Interface->ColssData.ModeData.colss_current_mode );	
  	 printf("COLSS confirm message1= %s  \n",Interface->ColssData.MsgData.message1);
	 printf("COLSS confirm message2= %s  \n",Interface->ColssData.MsgData.message2);
	 printf("COLSS confirm message3= %s  \n",Interface->ColssData.MsgData.message3);
 	 // the following test which data to be copied into Interfece SM and copy	

	 // Change the opcode to tell InterfaceToGUI to tell the picture 
	 //that the processing was confirmed.
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	 
	 break;

      default:
	   
	   cout << "parseColssMessages: Received Unknown Message, opcode: " 
	             << hex << msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
