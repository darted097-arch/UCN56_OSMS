//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseCALMessages.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the CAL messages.  This includes
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
struct CAL_DATA1{
        int             CalPage;
        CAL_MEMBER      CALine[15];
        };

void parseCALMessages(messageObject & msg)
{
   CAL_DATA1 *pCALData;
   int line;
   int j;
   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
     // to ccur
    case REQUEST_CAL_DATA:
    case CAL_PAGE_OFF:
    case REQUEST_CAL_NEXT_DATA:
    case REQUEST_CAL_PREV_DATA:
#ifdef DEBUG
         cout << " request C A L  Display" << endl;
         cout << "opcode= " << hex << msg.getOpcode() << endl;         
#endif
         break;

     // from ccur	
    case SEND_CAL_DATA:
#ifdef DEBUG
         cout <<" C A L Display sent from Host!"<<endl;
#endif
	 pCALData = (CAL_DATA1 *)&(Interface->CAL_Data);
         // get the data from the message into the shared memory area
	  msg.getData(&(Interface->CAL_Data), sizeof(Interface->CAL_Data));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         pCALData->CalPage = ntohl(pCALData->CalPage);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  for (line=1; line<=15; line++)
	  {
	     pCALData->CALine[line-1].Time[11] = '\0';
	     pCALData->CALine[line-1].AlarmProb[7] = '\0';
	     for (j = 0; j < 8; j++)
	      {
	         if (isspace (pCALData->CALine[line-1].AlarmPid[j]) )
	          {
		    // fix up the pid from the message to be null terminated
		     pCALData->CALine[line-1].AlarmPid[j] =  '\0';
		     break;
	          }// endif
	      }
#ifdef DEBUG
	     cout << "Time = " << pCALData->CALine[line-1].Time << " " 
		  << "Prob = " << pCALData->CALine[line-1].AlarmProb << " "
		  << "Alrm = " << pCALData->CALine[line-1].AlarmPid << endl;
#endif
	  }
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(SEND_CAL_DATA);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
   }
}
