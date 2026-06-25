//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseSecureMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes messages the Secure messages
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>

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
#include "messageQueueObject.h"
#include "messageObject.h"
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject      IGUI_MsgQue;
//
// Local Constant Definitions: NONE
//
// Function Prototypes:
//
//   
//
// ***************************************************************************
//
void parseSecureMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   SECURE_DATA * securedataptr; 
   SECURE_PROCESSING * secureprocessingptr; 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
   // Copy global buffer into local buffer which says the opcode

   switch (Msg.getOpcode())
   {
        // Send the same info for both messages; it won't hurt
        
	case PROCESS_SECURE_DATA:
	      
	   // get the workstation host name
	   gethostname (Interface->SecureRequest.data.theWorkstation,LEN_SYSTEMNAME);
	
	   // Copy the Interface structure to the message (opcode+data)
	   Msg.setData(&(Interface-> SecureRequest.data),sizeof(Interface-> SecureRequest.data));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   	   securedataptr = (SECURE_DATA *)Msg.getDataPtr(); 
	   securedataptr->signonoff = htonl(securedataptr->signonoff); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   break;

	case CONFIRM_SECURE_PROCESSING:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   secureprocessingptr = (SECURE_PROCESSING *)Msg.getDataPtr();
	   secureprocessingptr->accesslevel = ntohl(secureprocessingptr->accesslevel);
	   secureprocessingptr->accessstatus = ntohl(secureprocessingptr->accessstatus);
	   secureprocessingptr->utdvflag = ntohl(secureprocessingptr->utdvflag);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   Msg.getData(&(Interface->SecureConfirm.data),sizeof(Interface->SecureConfirm.data));

	   cout  << "accessstatus =" << Interface->SecureConfirm.data.accessstatus << endl;
           cout  << "accesslevel =" << Interface->SecureConfirm.data.accesslevel << endl;	   
           cout  << "utdvflag =" << Interface->SecureConfirm.data.utdvflag << endl;

	   IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	   break;

	case REQUEST_SECURE_SIGNOFF:

//	   Msg.getData(&(Interface->SecureConfirm.data),sizeof(Interface->SecureConfirm.data));
	   cout  << "parseSecureMessages() REQUEST_SECURE_SIGNOFF" << endl;


	   IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	   break;


	default:
	   
	   cout << "parseSecureMessages: Received Unknown Message, opcode: " 
	        << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
   
}
