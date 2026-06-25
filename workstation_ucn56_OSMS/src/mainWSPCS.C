//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// mainWSPCS.C            
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <fstream.h>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/param.h>
//#include <sys/pstat.h> 
// for Linux (2012.07.05. CORENET )
#ifndef LINUX_APP
#include <sys/pstat.h> 
#else
//#include <sys/pstat.h> 
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
// User Defined Include Files:
//
#include "systemParameters.h"
#include "sharedMemoryObject.h"
#include "DatabaseObject.h"
#include "InterProcSMObject.h"
#include "interface.h"
#include "CommLinkObject.h"
#include "WSPCSObject.h"
//
// Global Variables:
//

INTERFACE	* Interface;

// Attach to the database and Interface shared memory so that it will 
//be removed when this task ends.
sharedMemoryObject	InterfaceSMO(INTERFACE_KEY,sizeof(INTERFACE),0);
DatabaseObject		DBO(DATABASE_KEY, (NUM_REC+1)*sizeof(RECORD),0);
InterProcSMObject	IPO(INTERPROC_KEY, sizeof(interProc), 0);
WSPCSObject		WSPCS;

// Local static variables
//
// Function Prototypes:
//
#include "prototypes.h"
void onInterrupt(int);
void databaseLoaded(int);
void zombieZapper(int);
//
// ***************************************************************************
//

int main(int argc, char * argv[])
{   				
   cout << "***** mainWSPCS *****  TYPE CTRL C TO TERMINATE\n";
#ifdef DEBUG
   cout << "   number of args is:" << dec << argc << endl;
   for (int i = 0; i < argc; i++)
   {
      cout << argv[0] << ": arg " << dec << i << " is " << argv[i] << endl;
   }
#endif
   flush(cout);


   {
      char hostnameArray[MAX_HOSTNAME_LEN];
      char portnoArray[MAX_PORTNO_LEN];

      memset(hostnameArray, 0x00, sizeof(MAX_HOSTNAME_LEN));
      memset(portnoArray, 0x00, sizeof(MAX_PORTNO_LEN));

      IPO.setHostNum(1, 1);
      IPO.setHostName(1, WSPCS.getHost1Name(hostnameArray));
      IPO.setPortNo(1, WSPCS.getHost1PortNo(portnoArray));

      IPO.setHostNum(2, 2);
      IPO.setHostName(2, WSPCS.getHost2Name(hostnameArray));
      IPO.setPortNo(2, WSPCS.getHost2PortNo(portnoArray));

      IPO.setprimaryLocked(FALSE);
   }

   // get the WSPCS defaults from the configuration file
   //These will override any defaults
   WSPCS.setWSPCSinfo("WSPCS.cfg");

   // Parse command line arguments for overrides.
   WSPCS.parseOptions(argc, argv);

   // Set the CTRL-C interrupt handler
   if (signal(SIGINT, SIG_IGN) != SIG_IGN)
      signal(SIGINT, onInterrupt);

   // Set the interrupt handler for the signal sent to a 
   //parent when one of its children is terminating.
   if(signal(SIGCHLD, SIG_IGN) != SIG_IGN)
      signal(SIGCHLD, zombieZapper);

   // Set the global Interface pointer to shared memory
   Interface = (INTERFACE *) InterfaceSMO.getAddr();

   // Init the INTERFACE CPU connection indacators to indicate
   //that we are not connected
   Interface->System.connected = FALSE;
   Interface->System.cpuNum = 0;	// CPU NONE
   IPO.setNumTicks(0);			// Number of ticks on start

   // Init the FDDI status variables to indicate that both
   // channels are down

   char tmpbuf[5];
   if (0 == strcmp(WSPCS.getFDDIOption(tmpbuf),"ON"))
   {
      Interface->System.FDDIChanA = 0;
      Interface->System.FDDIChanB = 0;
   }
   else
   {
      Interface->System.FDDIChanA = 1;
      Interface->System.FDDIChanB = 1;
   }

   cout << "WSPCS.mainLoop() START!!!" << endl;

   WSPCS.mainLoop();

}//end main()


void onInterrupt(int signum)
{
   signal(SIGINT, onInterrupt);
   cout << "\n***** WSPCS onInterrupt() *** received signal " 
	<< dec << signum << " ***\n";

   cout << endl;
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "***** Terminating WSPCS *****\n";
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "*****************************\n\n";

   flush(cout);

   // ujson add 20210303 ucn56 delete
/*
   cout << "   Waiting for DBWSPCS to stop" << endl;
   waitpid(IPO.getPidDBWSPCS(),NULL,0);  
   cout << "      DBWSPCS stopped" << endl;
   sleep(1);

   cout << "   Waiting for DBCommLink1 to stop" << endl;
   waitpid(IPO.getPidDBCommLink1(),NULL,0);  
   cout << "      DBCommLink1 stopped" << endl;
   sleep(1);

   cout << "   Waiting for DBCommLink2 to stop" << endl;
   waitpid(IPO.getPidDBCommLink2(),NULL,0);  
   cout << "      DBCommLink2 stopped" << endl;
   sleep(1);
*/

   cout << "   Waiting for RTM to stop" << endl;
   waitpid(IPO.getPidRTM(),NULL,0);  
   cout << "      RTM stopped" << endl;
   sleep(1);

   cout << "   Waiting for Control to stop" << endl;
   waitpid(IPO.getPidControl(),NULL,0);
   cout << "      Control stopped" << endl;
   sleep(1);

   cout << "   Waiting for Timer to stop" << endl;
   waitpid(IPO.getPidTimer(),NULL,0);
   cout << "      Timer stopped" << endl;
   sleep(1);

   cout << "   Waiting for InterfaceToGUI to stop" << endl;
   waitpid(IPO.getPidInterfaceToGUI(),NULL,0);
   cout << "      InterfaceToGUI stopped" << endl;
   sleep(1);

   cout << "   Waiting for CommLink1 to stop" << endl;
   waitpid(IPO.getPidCommLink1(),NULL,0);
   cout << "      CommLink1 stopped" << endl;
   sleep(1);

   cout << "   Waiting for CommLink2 to stop" << endl;
   waitpid(IPO.getPidCommLink2(),NULL,0);
   cout << "      CommLink2 stopped" << endl;
   sleep(1);

   cout << "   Waiting for Ticker to stop" << endl;
   waitpid(IPO.getPidTicker(),NULL,0);  
   cout << "      Ticker stopped" << endl;
   sleep(1);

   // Be sure the rtm and its processes are killed
   system("stop.ws");

   cout << "********** mainWSPCS ending...**********" << endl;
   exit(0);
}

void zombieZapper(int signum)
{
   int	status;
   int temp;

   cout << "***** WSPCS zombieZapper() *** received signal " 
	<< dec << signum << " ***\n";

   // complete the termination of a child that was killed above.
   // wait3 normally blocks until one or more children exit for any 
   //reason. Because this routine is called when a SIGCHLD signal arrives,
   //it will always be called after a child exits.  Just to be sure, 
   //WNOHANG ensures that an erroneous call does not hang the main 
   //routine by not blocking, that is, returning immediately, 
   //even if no process has exited.

   while((temp = wait3(&status, WNOHANG, (struct rusage *)0)) > 0)
   {
      cout << "zombieZapper cleaning up for pid = " 
	   << dec << temp << endl;
      flush(cout);
   }

   signal(SIGCHLD, zombieZapper);
} // end zombieZapper()

