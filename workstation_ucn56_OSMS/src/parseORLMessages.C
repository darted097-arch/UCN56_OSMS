//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseORLMessages.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the ORL messages.  This includes
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
#include <ctype.h>

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
struct ORL_DATA1{
        int             OrlPage;
        ORL_MEMBER      ORLine[15];
        };

void parseORLMessages(messageObject & msg)
{
   ORL_DATA1 *pORLData;
   int line;
   int j;
      
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
     // to ccur
    case REQUEST_ORL_DATA:
    case ORL_PAGE_OFF:
    case REQUEST_ORL_NEXT_DATA:
    case REQUEST_ORL_PREV_DATA:
#ifdef DEBUG
         cout << " request C A L  Display" << endl;
         cout << "opcode= " << hex << msg.getOpcode() << endl;         
#endif
         break;

     // from ccur	
    case SEND_ORL_DATA:
#ifdef DEBUG
         cout <<" O R L Display sent from Host!"<<endl;
#endif
	 pORLData = (ORL_DATA1 *)&(Interface->ORL_Data);
         // get the data from the message into the shared memory area
	  msg.getData(&(Interface->ORL_Data), sizeof(Interface->ORL_Data));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 pORLData->OrlPage = ntohl(pORLData->OrlPage);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  for (line=1; line<=15; line++)
	  {
	     pORLData->ORLine[line-1].AlarmProb[7] = '\0';
	     for (j = 0; j < 8; j++)
	   {
	      if (isspace (pORLData->ORLine[line-1].AlarmPid[j]) )
	      {
		  // fix up the pid from the message to be null terminated
		  pORLData->ORLine[line-1].AlarmPid[j] =  '\0';
		  break;
	      }// endif
	   }// endfor
#ifdef DEBUG
		cout  << "Prob = " << pORLData->ORLine[line-1].AlarmProb << " "
		  << "Alrm = " << pORLData->ORLine[line-1].AlarmPid << endl;
#endif
	  }

	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(SEND_ORL_DATA);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
   }
}
