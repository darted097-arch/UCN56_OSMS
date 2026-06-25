//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parsePRTSUMMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the Print Summary messages.  This includes
//			both messages from the host to the workstation and messages from
//			the workstation to the host.
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

// Local Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "messageObject.h"		//

// External Variables:

extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

//													
// **************************************************************************************
//													
// generic template for the local processing of workstation messages

// Prototype Definitions:
   
void parsePRTSUMMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSPRTSUMRequestData * psr_data_ptr; //
   WSPRTSUMConfirmData * psc_data_ptr; //
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   switch (Msg.getOpcode())
   {
      case PRINT_SUMMARY_MESSAGE:
         gethostname(Interface->PRTSUM_REQUEST_MESSAGE.data.theWorkstation,LEN_SYSTEMNAME);
         Msg.setData(&(Interface->PRTSUM_REQUEST_MESSAGE.data),
	    (sizeof(Interface->PRTSUM_REQUEST_MESSAGE.data.Option) +
	     sizeof(Interface->PRTSUM_REQUEST_MESSAGE.data.theWorkstation)) );
	 
#ifdef DEBUG
	 printf(" data option = %d",Interface->PRTSUM_REQUEST_MESSAGE.data);
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 psr_data_ptr = (WSPRTSUMRequestData *)Msg.getDataPtr();
	 psr_data_ptr->Option = htonl(psr_data_ptr->Option);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
 	 break;

      case PRINT_SUMMARY_CONFIRM:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 psc_data_ptr = (WSPRTSUMConfirmData *)Msg.getDataPtr();
	 psc_data_ptr->Option = ntohl(psc_data_ptr->Option);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

         Msg.getData(&(Interface->PRTSUM_CONFIRM_MESSAGE.data),sizeof(Interface->PRTSUM_CONFIRM_MESSAGE.data));
	 Interface->PRTSUM_CONFIRM_MESSAGE.data.message1[39]='\0';
	 Interface->PRTSUM_CONFIRM_MESSAGE.data.message2[39]='\0';
	 Interface->PRTSUM_CONFIRM_MESSAGE.data.message3[39]='\0';
	 
	 Interface->System.receivedData = TRUE;
	 
	 break;
	 
   } // end of opcode switch statement
   
}
