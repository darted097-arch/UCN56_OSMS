//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// Ticker.C             
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module increments a ticker every second to approximate elapsed time.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//

#include <iostream.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#ifdef LINUX_APP
#include <sys/types.h>	// CORENET (2012.07.05)
#include <sys/wait.h>	// CORENET (2012.07.05)
#endif

// User Defined Include Files:
//
#include "prototypes.h"
#include "InterProcSMObject.h"

#ifdef DEBUG
static   time_t ltime = 0;
static   time_t last_ltime = 0;
#endif

static   struct timeval   timeout;        // for select "sleep" function


// create (attach to) shared memory objects
InterProcSMObject       IPO(INTERPROC_KEY, sizeof(interProc), 0);


void stopAllButMe();
void zombieZapper(int);

//
// ***************************************************************************
//

int main()
{
   // Set the interrupt handler for the signal sent to a
   //parent when one of its children is terminating.
   if(signal(SIGCHLD, SIG_IGN) != SIG_IGN)
      signal(SIGCHLD, zombieZapper);

   int running = 1;
   while (1 == running)
   {
      IPO.incNumTicks(); 
#ifdef DEBUG
      last_ltime = ltime;
      ltime = getLocalTime();
      cout << "     ticker: elapsed time in ticks = " << dec << IPO.getNumTicks() 
           << "   ltime = " << ctime(&ltime) << endl;
      if (1 != (ltime - last_ltime))
      {
	 cout << "***** ticker: elapsed time = " << dec << IPO.getNumTicks() 
              << "   ltime = " << ctime(&ltime) << endl;
      }
#endif
      timeout.tv_sec  = 1;
      timeout.tv_usec = 0;
      // use a null select, instead of sleep, for a more precise sleep time
      // wait a second.
      if (select(0, 0, 0, 0, &timeout) < 0)
         cout << "***** ticker: Error from select() call" << endl;

      // Check if WSPCS still exists.  If not, exit
      if (IPO.getPidWSPCS() != getppid())
      {
         cout << "**** ERROR **** Ticker: The WSPCS process can not be found."
              << endl;
#ifdef DEBUG
         cout << "Process Group Ids for CommLink1, CommLink2, InterfaceToGUI, "
              << "rtm, control, Timer, Ticker, WSPCS are " << endl; 
         cout << dec << getpgid(IPO.getPidCommLink1()) << " " 
              << getpgid(IPO.getPidCommLink2()) << " " 
              << getpgid(IPO.getPidInterfaceToGUI()) << " "
              << getpgid(IPO.getPidRTM()) << " "
              << getpgid(IPO.getPidControl()) << " "
              << getpgid(IPO.getPidFDDIstart()) << " "
              << getpgid(IPO.getPidTimer()) << " "
              << getpgid(IPO.getPidTicker()) << " "
              << getpgid(IPO.getPidWSPCS()) << endl;
#endif
         cout << "**** Killing all processess with process group id = " 
	      << dec << getpgrp() << ".  EXITING ****" << endl;

         stopAllButMe();

	 sleep(5);
	 running = 0;
      }
   } // endwhile
} // end Ticker

void stopAllButMe()
{
   cout << endl;
   cout << "***************************************\n";
   cout << "***************************************\n";
   cout << "***************************************\n";
   cout << "* Ticker: Terminating WSPCS Processes *\n";
   cout << "***************************************\n";
   cout << "***************************************\n";
   cout << "***************************************\n\n";

   flush(cout);

   if (IPO.getPidRTM())
   {
      cout << "\n   Killing RTM" << endl;
      kill(IPO.getPidRTM(),SIGINT);
      waitpid(IPO.getPidRTM(),NULL,0);
      IPO.setPidRTM(0);
      cout << "      RTM killed" << endl;
   }
   else
   {
      cout << "   Not Killing RTM.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidControl())
   {
      cout << "   Killing Control" << endl;
      kill(IPO.getPidControl(),SIGINT);
      waitpid(IPO.getPidControl(),NULL,0);
      IPO.setPidControl(0);
      cout << "      Control killed" << endl;
   }
   else
   {
      cout << "   Not Killing Control.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidTimer())
   {
      cout << "   Killing Timer" << endl;
      kill(IPO.getPidTimer(),SIGINT);
      waitpid(IPO.getPidTimer(),NULL,0);
      IPO.setPidTimer(0);
      cout << "      Timer killed" << endl;
   }
   else
   {
      cout << "   Not Killing Timer.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidInterfaceToGUI())
   {
      cout << "   Killing InterfaceToGUI" << endl;
      kill(IPO.getPidInterfaceToGUI(),SIGINT);
      waitpid(IPO.getPidInterfaceToGUI(),NULL,0);
      IPO.setPidInterfaceToGUI(0);
      cout << "      InterfaceToGUI killed" << endl;
   }
   else
   {
      cout << "   Not Killing InterfaceToGUI.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidCommLink1())
   {
      cout << "   Killing CommLink1" << endl;
      kill(IPO.getPidCommLink1(),SIGINT);
      waitpid(IPO.getPidCommLink1(),NULL,0);
      IPO.setPidCommLink1(0);
      cout << "      CommLink1 killed" << endl;
   }
   else
   {
      cout << "   Not Killing CommLink1.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidCommLink2())
   {
      cout << "   Killing CommLink2" << endl;
      kill(IPO.getPidCommLink2(),SIGINT);
      waitpid(IPO.getPidCommLink2(),NULL,0);
      IPO.setPidCommLink2(0);
      cout << "      CommLink2 killed" << endl;
   }
   else
   {
      cout << "   Not Killing CommLink2.  Don't have a Pid" << endl;
   }
   sleep(1);

   // Be sure the rtm and its processes are killed
   system("stop.ws");

   cout << "*****************************************\n";
   cout << "* Ticker: WSPCS Child Processes Killed **\n";
   cout << "*****************************************\n";
   flush(cout);

}

void zombieZapper(int signum)
{
   int  status;
   int temp;

   cout << "***** Ticker zombieZapper() *** received signal "
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


