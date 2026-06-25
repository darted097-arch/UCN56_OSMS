//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// mainCommLink.C       
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  Creates a shared segment for the display database & fills it with
//  data obtained dynamically from the Concurrent host software.
//  Typing <ctrl/C> (interrupt) will cause the program to terminate
//  cleanly by removing the shared segment before exiting.
//  If the connection to the remote socket times out, the program will
//  keep trying indefinitely.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <strings.h>

// by feel9
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#include <netdb.h>
#include <netinet/in.h>

// corenet ujson 2017.0106
#include <sys/time.h>
//#include <time.h>                     // time calls
//#include <sys/uio.h>

#include <stdlib.h>                     // exit
#include <iostream.h>                   // io calls
#include <fstream.h>
#include <strings.h>
#include <unistd.h>                     // close
#include <sys/stat.h>                   // chmod

#include <sys/socket.h>                 // socket info
#include <signal.h>
#include "Typedefs.h"
#include "interface.h"
#include "systemParameters.h"           // generic system parameters
#include "databaseParameters.h"         // database parameters
#include "database.h"                   // database structure
#include "messageParameters.h"          // message parameters
#include "messageQueueObject.h"
#include "sharedMemoryObject.h"
#include "CommLinkObject.h"
#include "DatabaseObject.h"
#include "messageObject.h"
#include "InterProcSMObject.h"

#include "DatabaseObject.h"   
#include "prototypes.h"
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//
// User Defined Include Files:
//
//#include "systemParameters.h"		// generic system parameters
//#include "databaseParameters.h"		// database parameters
//#include "messageParameters.h"		// message parameters
//#include "messages.h"			// message structures
//#include "interface.h"			// Picasso to shm structure
//#include "messageQueueObject.h"
//#include "sharedMemoryObject.h"
//#include "CommLinkObject.h"
//#include "DatabaseObject.h"
//#include "InterProcSMObject.h"

//
// Global Variables:
//
INTERFACE	*Interface;		// shared memory to Picasso variables

// Create Global Message Queue Objects
messageQueueObject IGUI_MsgQue(COMM2GUI_KEY, TRUE);	// TRUE = remove on exit
messageQueueObject WS_MsgQue(WS_MESSAGE_KEY, TRUE);	// TRUE = remove on exit
//messageQueueObject CC_MsgQue(CC_MESSAGE_KEY, TRUE);	// TRUE = remove on exit

// Create Global shared memory objects
DatabaseObject		     DBO(DATABASE_KEY,(NUM_REC+1)*sizeof(RECORD),0);
sharedMemoryObject InterfaceSMO(INTERFACE_KEY,sizeof(INTERFACE),0);
InterProcSMObject       IPO(INTERPROC_KEY, sizeof(interProc), 0);

// Create the CommLink object
CommLinkObject CommLink;

#define DEBUG

//
// Local Constant Definitions:
//
//
// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//
// LATER: These should be moved into the CommLinkObject.
int main (int argc, char * argv[])
{   				
   int		CommLinkNum;	// CommLink number passed in on command line
   char		CommLinkName[MAX_COMMLINK_NAME_LEN];	// CommLink name
   char		hostConnectName[MAX_HOSTNAME_LEN];
   char      	hostPortNo[MAX_PORTNO_LEN];
   char         szSendUse[5];

   memset(CommLinkName, 0x00, sizeof(CommLinkName));
   memset(hostConnectName, 0x00, sizeof(hostConnectName));
   memset(hostPortNo, 0x00, sizeof(hostPortNo));
   memset(szSendUse, 0x00, sizeof(szSendUse));

   if (5 != argc) 
   {
      cerr << argv[0] << ": ***** ERROR Incorrect number of arguments" << endl;
      exit(1);
   }

   sscanf(argv[0], "%s", CommLinkName);
   sscanf(argv[1], "%d", &CommLinkNum);
   sscanf(argv[2], "%s", hostConnectName);
   sscanf(argv[3], "%s", hostPortNo);
   sscanf(argv[4], "%s", szSendUse);  

   CommLink.setnum(CommLinkNum);
   CommLink.setname(CommLinkName);
   CommLink.sethostName(hostConnectName);
   CommLink.setportNum(hostPortNo);
   CommLink.setsendUse(szSendUse);

   cout << "CommLinkNum = " << dec << CommLink.getnum() 
	<< " CommLinkName = " << CommLink.getname() 
	<< " to connect with " << CommLink.gethostName()
	<< " port no " << CommLink.getportNum()
	<< " and decide to send packet to terminal(1:SEND, 2:NO SEND) "
        << " (" << CommLink.getsendUse() << ")" << endl; 

   cout << ">>>>>>>>>>>>>>>> SEND USE : " << szSendUse << " <<<<<<<<<<<<<<<<" << endl;

   Interface = (INTERFACE *) InterfaceSMO.getAddr();

   // Initialize the database revision to an unknown
   Interface->System.databaseRevision = DBO.getDBrev();
#ifdef DEBUG
	 cout << "\n############################" << endl;
	 cout << "CommLink StartUp!!!!!!!" << endl;
	 cout << "database Revision = " << Interface->System.databaseRevision << endl;
	 cout << "############################\n" << endl;
#endif


   // Make the connection to the Concurrent system
   CommLink.Connect();

   // Tell of allowable exit with <CTRL> C
   cout << "Type <CTRL> C to terminate" << endl;

   CommLink.mainLoop();
   
}//end mainCommLink()

/*
int CommLinkObject::u_ignore_sigpipe()
{
   struct sigaction act;

   if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
   {
      return 1;
   }

   if (act.sa_handler == SIG_DFL) {
      act.sa_handler = SIG_IGN;
      if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
      {
         return 1;
      }
   }
   return 0;
}
*/

/*
// by feel9 
int main_old (int argc, char * argv[])
{   				
   int		CommLinkNum;		// CommLink number passed in on command line
   char		CommLinkName[MAX_COMMLINK_NAME_LEN];	// CommLink name
   char		hostConnectName[MAX_HOSTNAME_LEN];

   if (3 != argc) 
   {
      cerr << argv[0] << ": ***** ERROR Incorrect number of arguments" << endl;
      exit(1);
   }

   sscanf(argv[2], "%s", hostConnectName);
   sscanf(argv[1], "%d", &CommLinkNum);
   sscanf(argv[0], "%s", CommLinkName);

   CommLink.setnum(CommLinkNum);
   CommLink.setname(CommLinkName);
   CommLink.sethostName(hostConnectName);

   cout << "CommLinkNum = " << dec << CommLink.getnum() 
	<< " CommLinkName = " << CommLink.getname() 
	<< " to connect with " << CommLink.gethostName() << endl;

   Interface = (INTERFACE *) InterfaceSMO.getAddr();

   // Initialize the database revision to an unknown
   Interface->System.databaseRevision = DBO.getDBrev();

   // Make the connection to the Concurrent system
   CommLink.Connect();

   // Tell of allowable exit with <CTRL> C
   cout << "Type <CTRL> C to terminate" << endl;
	 
   CommLink.mainLoop();
   
}//end mainCommLink()
*/

