//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseBDLMessages.C   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the BDL messages.  This includes
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
struct BDL_DATA1{
        int             BdlPage;
        BDL_MEMBER      BDLine[15];
        };

void parseBDLMessages(messageObject & msg)
{
   BDL_DATA1 *pBDLData;
   int line;
   int j;
   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {
     // to ccur
    case REQUEST_BDL_DATA:
    case BDL_PAGE_OFF:
    case REQUEST_BDL_NEXT_DATA:
    case REQUEST_BDL_PREV_DATA:
#ifdef DEBUG
         cout << " request B D L  Display" << endl;
         cout << "opcode= " << hex << msg.getOpcode() << endl;         
#endif
         break;

     // from ccur	
    case SEND_BDL_DATA:
#ifdef DEBUG
         cout <<" C A L Display sent from Host!"<<endl;
#endif
	 pBDLData = (BDL_DATA1 *)&(Interface->BDL_Data);
         // get the data from the message into the shared memory area
	  msg.getData(&(Interface->BDL_Data), sizeof(Interface->BDL_Data));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 pBDLData->BdlPage = ntohl(pBDLData->BdlPage);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	  for (line=1; line<=15; line++)
	  {
	     pBDLData->BDLine[line-1].AlarmProb[7] = '\0';
	     for (j = 0; j < 8; j++)
	     {
	        if (isspace (pBDLData->BDLine[line-1].AlarmPid[j]) )
	        {
		  // fix up the pid from the message to be null terminated
		  pBDLData->BDLine[line-1].AlarmPid[j] =  '\0';
		  break;
	         }// endif
	     }//endfor
	     pBDLData->BDLine[line-1].Chassis[7] = '\0';
#ifdef DEBUG
	     cout << "Prob = " << pBDLData->BDLine[line-1].AlarmProb << " "
		  << "Alrm = " << pBDLData->BDLine[line-1].AlarmPid << " " 
		  << "Chass= " << pBDLData->BDLine[line-1].Chassis << endl;	
#endif
	  }
	 // Change the opcode to tell InterfaceToGUI to tell the picture to display the data
	 msg.setOpcode(SEND_BDL_DATA);
	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	  IGUI_MsgQue.sendMessage(msg.pMsg, MSG_HEADER_LEN, msg.getType());
	  flush(cout);
   }
}
