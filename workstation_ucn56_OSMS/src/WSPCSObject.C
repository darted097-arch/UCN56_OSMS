//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// WSPCSObject.C        
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

#ifndef LINUX_APP
#include <sys/pstat.h>	// for Linux (2012.07.05. CORENET )
#else
#include <proc/readproc.h>	// for Linux (2012.07.05. CORENET )
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

// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//

WSPCSinfoStruct WSPCSObject::WSPCSinfo;

unsigned int    WSPCSObject::PBcounter;
unsigned int    WSPCSObject::BADinputCounter;
unsigned int    WSPCSObject::goodStatusCounter;
int             WSPCSObject::procStatusGood;
int             WSPCSObject::oldWsState;

// by feel9  >>>>>>>>>>>>>>>
extern  InterProcSMObject       IPO;    // Inter Process Shared Memory
static int 	newDBupdatesStarted = FALSE;
static int needToSignalDatabaseLoaded = TRUE;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

WSPCSObject::WSPCSObject()
{
   char hostnameArray[MAX_HOSTNAME_LEN];

   PBcounter = 0;
   BADinputCounter = 0;
   goodStatusCounter = 0;
   procStatusGood = GOODSTATUS;
   oldWsState = DEAD_WS_STATE;

   // get defualt host names from the include file
   setPrimaryHostName("");        // NULL string for host means NONE
   setHost1Name(HOST_NAME_A);
   setHost2Name(HOST_NAME_B);
   setFDDIOption("OFF");
   setMODEMOption("OFF");	// default to no modem
   setSENDOption("1");

   if (NULL == gethostbyname(getHost1Name(hostnameArray))) // host 1
   {
      cout << "***** ERROR ***** HOST_NAME_A is not a valid host\n";
      cout << "                  Fix appropriate .h file and re-make system\n";
   }

   if (NULL == gethostbyname(getHost2Name(hostnameArray))) // host 2
   {
      cout << "***** ERROR ***** HOST_NAME_B is not a valid host\n";
      cout << "                  Fix appropriate .h file and re-make system\n";
   }
/*
   //201027_jhp
   int i;
   for(i=0; i<=NUM_REC; i++)
   {
      DBO.setnewsta(i,0);
   }
*/
} // end WSPCSObject()



WSPCSObject::~WSPCSObject()
{
}



void WSPCSObject::parseOptions(int numOptions, char * argv[])
{
   extern InterProcSMObject	IPO;

   int optionCounter, j, optionIndex;
   
   char hostnameArray[MAX_HOSTNAME_LEN];
   char portnoArray[MAX_PORTNO_LEN];

   memset(hostnameArray, 0x00, sizeof(MAX_HOSTNAME_LEN));
   memset(portnoArray, 0x00, sizeof(MAX_PORTNO_LEN));

   cout << argv[0] << ":WSPCSObject::parseOptions()" << endl;

   if (1 == numOptions)
   {
      cout << argv[0] << " No input options." << endl;
   }
   else
   {
      cout << argv[0] << " Number of input(s) = " << dec << numOptions << endl;
   }

   for (optionCounter = 1; optionCounter < numOptions; optionCounter++)
   {
      cout << "   considering command line option " << dec 
	   << optionCounter << " = " << argv[optionCounter] 
	   << endl;
      for (optionIndex = 0; optionIndex < NUM_OPTIONS; optionIndex++)
      {
         if (0 == strcmp(options[optionIndex].str, argv[optionCounter]))
         {
            // found a valid command line option
            cout << "   found a valid option = " << argv[optionCounter] << endl;

            optionCounter++; // index to next command line argument

            if (TRUE == options[optionIndex].used)
            {
               cout << argv[0] << ": ***** WARNING ***** Option "
                    << options[optionIndex].str << " used was used previously."
                    << endl;
            }
#ifdef DEBUG
            cout << "   found option " << options[optionIndex].str
                 << " index = " << dec << optionIndex << endl;
#endif
            options[optionIndex].used = TRUE;
            switch (optionIndex)
            {
               case HOST1_OPTION_INDEX:
               {
                  cout << "      " << argv[0] << ": Setting CommLink1 host to "
                       << argv[optionCounter] << endl;
                  if (NULL != gethostbyname(argv[optionCounter]))
                  {
                     setHost1Name(argv[optionCounter]);
                     IPO.setHostName(1, argv[optionCounter]);
                  }
                  else
                  {
                     cout << "      " << argv[0] << ": Setting CommLink1 host "
                          << "to NULL because gethostbyname("
                          << argv[optionCounter] << ") FAILED." << endl;
                     setHost1Name("");
                     IPO.setHostName(1, "");
                  }
                  break;
               }
               case HOST2_OPTION_INDEX:
               {
                  cout << "      " << argv[0] << ": Setting CommLink2 host to "
                       << argv[optionCounter] << endl;
                  if (NULL != gethostbyname(argv[optionCounter]))
                  {
                     setHost2Name(argv[optionCounter]);
                     IPO.setHostName(2, argv[optionCounter]);
                  }
                  else
                  {
                     cout << "      " << argv[0] << ": Setting CommLink2 host "
                          << "to NULL because gethostbyname("
                          << argv[optionCounter] << ") FAILED." << endl;
                     setHost2Name("");
                     IPO.setHostName(2, "");
                  }
                  break;
               }
               case PRIMARY_OPTION_INDEX:
               {
                  if (NULL != gethostbyname(argv[optionCounter]))
                  {
                     cout << "      " << argv[0] << ": Setting PRIMARY "
                          << "CommLink to " << argv[optionCounter] << endl;
                     setPrimaryHostName(argv[optionCounter]);
                  }
                  else
                  {
                     cout << "      " << argv[0] << ": Setting Primary host "
                          << "to NULL because gethostbyname("
                          << argv[optionCounter] << ") FAILED." << endl;
                     setPrimaryHostName("");
                  }
                  break;
               }
               case FDDI_OPTION_INDEX:
               {
                  if ((0 == strcmp(argv[optionCounter],"ON")) || 
		      (0 == strcmp(argv[optionCounter],"OFF")))
                  {
                     cout << "      " << argv[0] << ": Setting FDDI "
                          << "option to " << argv[optionCounter] << endl;
                     setFDDIOption(argv[optionCounter]);
                  }
                  else
                  {
                     cout << "      " << argv[0] << ": Invalid FDDI option. "
                          << "Setting to OFF " << endl;
                     setFDDIOption("OFF");
                  }
                  break;
               }
	       case MODEM_OPTION_INDEX:
                  cout << "      " << argv[0] << ": Setting MODEM option to "
		       << argv[optionCounter] << endl; 
                  setMODEMOption(argv[optionCounter]);
                  IPO.setMODEMOption(argv[optionCounter]);
		  break;
               case CFGFILE_OPTION_INDEX:
                  // support for config file named on command line
                  cout << "      " << argv[0] << ": Option -CFGFILE not "
		       << "implemented." << endl;
                  break;
               case SEND_USE_INDEX:
                  cout << "      " << argv[0] << ": Setting SEND option to "
		       << argv[optionCounter] << endl;
                  setSENDOption(argv[optionCounter]);
		  IPO.setSENDOption(argv[optionCounter]);
		  break;
	       case PORT1_OPTION_INDEX:
		  cout << "     " << argv[0] << ": Setting CommLink1 port to "
		       << argv[optionCounter] << endl;
		  setHost1PortNo(argv[optionCounter]);
		  IPO.setPortNo(1, argv[optionCounter]);
		  break;
	       case PORT2_OPTION_INDEX:
		  cout << "     " << argv[0] << ": Setting CommLink2 port to "
		       << argv[optionCounter] << endl;
		  setHost2PortNo(argv[optionCounter]);
		  IPO.setPortNo(2, argv[optionCounter]);
		  break;
	       default:
               {
                  cout << "      " << argv[0] << ": Not a valid option.  "
                       << "This can never happen.\n";
                  break;
               }
            } //endswitch optionIndex
            break;      // stop looking at matching this option to the list.
         } //endif found option in list of options
      } //endfor compare input against all posible options
      if (optionIndex >= NUM_OPTIONS)
      {
         cout << argv[0] << ": ***** INVALID OPTION ***** " 
	      << argv[optionCounter] << endl;
         cout << "   Valid options are: " << endl;
	 for(j = 0; j < NUM_OPTIONS; j++)
	 {
	    cout << "      " << options[j].str << endl;
         }
      } // endif option not found in list of options
   } //endfor all agruments

   // Some additional checking for the command line overrides
   // If we don't have a valid host to connect to, exit
   if ( (0 == strcmp(IPO.getHostName(1), ""))  &&
        (0 == strcmp(IPO.getHostName(2), "")) )
   {
      cout << "***** ERROR ***** No valid hosts have been selected!\n";
      cout << "***** EXITING...\n\n";
      flush(cout);
      exit(1);
   }

   // Check the PRIMARY host for a valid entry.  Lock in the PRIMARY if valid
   //It must match one of the hosts
#ifdef DEBUG
   cout << " options[PRIMARY_OPTION_INDEX].used = "
        << options[PRIMARY_OPTION_INDEX].used << endl;
#endif
   if (options[PRIMARY_OPTION_INDEX].used)
   {
      int found;
#ifdef DEBUG
      cout << "  Setting PRIMARY CommLink to "
           << getPrimaryHostName(hostnameArray) << " from options\n";
#endif
//    Check if the named primary host is one of the hosts that can be
//connected to.
      found = FALSE;
      if (0 == strcmp(getPrimaryHostName(hostnameArray), ""))
      {
         cout << "Primary Host Selected is blank.  Primary will not "
	      << "be locked." << endl;
         IPO.setprimaryLocked(FALSE);
      }
      else // given primary is not blank
      {
         for (j = 1; j <= NUM_HOSTS; j++)
         {
            if (0 == strcmp(IPO.getHostName(j),
                         getPrimaryHostName(hostnameArray)))
            {
               found = TRUE;
               IPO.setprimaryStatus(j, PRIMARY_COMMLINK);
               IPO.setprimaryStatus((j%NUM_HOSTS)+1, BACKUP_COMMLINK);
               IPO.setprimaryLocked(TRUE);
#ifdef DEBUG
               cout << "   Just finished setting primary statuses for "
                    << "CommLink1 and CommLink2" << endl;
               cout << "   IPO.setprimaryStatus(" << dec << j << ", "
                    << PRIMARY_COMMLINK << ")" << endl;
               cout << "   IPO.setprimaryStatus(" << (j%NUM_HOSTS)+1 << ", "
                    << BACKUP_COMMLINK << ")" << endl;
               cout << "   IPO.setprimaryLocked(" << TRUE << ")" << endl;
               cout << "   IPO.getprimaryStatus(" << j << ") = "
                    << IPO.getprimaryStatus(j) << endl;
               cout << "   IPO.getprimaryStatus(" << (j%NUM_HOSTS)+1 << ") = "
                    << IPO.getprimaryStatus((j%NUM_HOSTS)+1) << endl;
               cout << "   IPO.getprimaryLocked() = "
                    << IPO.getprimaryLocked() << endl;
#endif
               break; // out of the for loop
            }// endif Primary hostname matches one of the given host names
         }// endfor number of hosts
         if (!found)
         {
            cout << argv[0] << ": Invalid PRIMARY connection "
                 << getPrimaryHostName(hostnameArray) << " selected" << endl;
            cout << "   ***** EXITING ***** " << endl;
            exit(1);
         }
      }// endelse given primary is not blank
   }// endif primary option used
} // end parseOptions



void WSPCSObject::mainLoop()
{
   extern INTERFACE *	Interface;

   cout << "\n***** WS STATE: setting wsState to initial STARTING state\n";
   Interface->System.wsState = STARTING_WS_STATE;
   processWsStateChange(oldWsState, Interface->System.wsState);
   oldWsState = Interface->System.wsState;

   // loop forever
   for (;;)     //forever
   {
      PBcounter = 0;
      BADinputCounter = 0;
      goodStatusCounter = 0;
      procStatusGood = GOODSTATUS;

      // fork and exec all tasks
      startAll();
      sleep(2);

      // keep monitoring for good status
      while(GOODSTATUS == procStatusGood)
      {
         if (0 == (goodStatusCounter % GOOD_STATUS_INTERVAL))
         {
            procStatusGood = goodStatus();
            goodStatusCounter = 0;
         }

	 if (!(Interface->System.wsState & SHOP_LISTS_BUILT_WS_STATE))
	 {
	    UpdateWarmStartStatus();
	 }

         if (Interface->System.wsState != oldWsState)
         {
            processWsStateChange(oldWsState, Interface->System.wsState);
            oldWsState = Interface->System.wsState;
            if (GOODSTATUS == procStatusGood)
            {
               Interface->System.wsState |= GOOD_PROCESSES_WS_STATE;
            }
         }

         if (0 == (PBcounter % DET_PB_INTERVAL))
         {
            setPBstatuses(); // this must be called before setConnected()
            setConnected();
            PBcounter = 0;
         }

         if (0 == (BADinputCounter % DET_BAD_INPUT_INTERVAL))
         {
            //determineIfBadInputs(); //201027_jhp
            BADinputCounter = 0;
         }

         PBcounter++;
         BADinputCounter++;
         goodStatusCounter++;

         //printf(" Interface->System.connected  !!!!!!!!![%d][%s][%s]\n", Interface->System.connected,
	 //	Interface->System.timeString, Interface->System.dateString);

         sleep(1);
      } // endwhile good status

      // let the rest of the system know that the processes are not good.
      cout << "\n***** WS STATE CHANGE: turning off GOOD_PROCESSES \n";
      Interface->System.wsState &= ~(GOOD_PROCESSES_WS_STATE);

      if (Interface->System.wsState != oldWsState)
      {
         processWsStateChange(oldWsState, Interface->System.wsState);
         oldWsState = Interface->System.wsState;
      }

      // stop all tasks
      stopAll();

      cout << "\n***** WS STATE CHANGE: setting wsState to STARTING state "
           << "after having stopped all processes\n";
      Interface->System.wsState = STARTING_WS_STATE;
      processWsStateChange(oldWsState, Interface->System.wsState);
      oldWsState = Interface->System.wsState;
   }//endforever
} // end mainLoop



void WSPCSObject::startAll()
{
   pid_t	pid;
   char		cmdstring[90];
   char		digitStr[2];
   char         szSendUse[5];

   extern InterProcSMObject     IPO;
   extern INTERFACE *		Interface;

   cout << endl;
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "** Starting WSPCS Processes *\n";
   cout << "*****************************\n";
   cout << "*****************************\n";
   cout << "*****************************\n\n";

   IPO.setPidWSPCS(getpid());
   IPO.setPidCommLink1(0);
   IPO.setPidCommLink2(0);
   IPO.setPidRTM(0);
   IPO.setPidControl(0);
   IPO.setPidInterfaceToGUI(0);
   IPO.setPidFDDIstart(0);
   IPO.setPidTicker(0);
   IPO.setPidTimer(0);

   // ujson add 20210303 ucn56 delete
//   IPO.setPidDBWSPCS(0);
//   IPO.setPidDBCommLink1(0);
//   IPO.setPidDBCommLink2(0);

   IPO.setReceivedDBdownload(FALSE);
   cout << "pidWSPCS = " << dec << IPO.getPidWSPCS() << endl; 
   flush(cout);

   
   //sprintf(cmdstring,"/usr/bin/rm -f /usr/local/procsee/recovery/*");
   sprintf(cmdstring,"/bin/rm -f /usr/local/procsee/recovery/*");	// (2012.07.05. CORENET )
   if (system(cmdstring) < 0)
   {
      perror("Could not remove Picasso recovery files");
   }

#ifdef DEBUG
   system("ll /usr/local/procsee/recovery/*");
   system("ps -ef |grep runtime");
#endif



   cout << "*** About to fork() Ticker ***" << endl;
   if (0 == (pid = fork()))
   { // child process ONLY
      cout << " Child of mainWSPCS about to start Ticker" << endl;
      execlp("Ticker", "Ticker", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start Ticker *****"
           << endl;
      exit(127);
   }

   // save the pid returned in the parent process.
   IPO.setPidTicker(pid);
   cout << " pidTicker = " << dec << IPO.getPidTicker() << endl; 

   cout << "*** About to fork() control ***" << endl;
   if (0 == (pid = fork()))
   {  // child process ONLY
      cout << " Child of mainWSPCS about to start control" << endl;
      execlp("control", "control", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start control *****"
           << endl;
      exit(127);
   }

   // save the pid returned in the parent process.
   IPO.setPidControl(pid);
   cout << " pidControl = " << dec << IPO.getPidControl() << endl; 
   sleep(2);

   cout << "*** About to fork() rtm ***" << endl;
   if (0 == (pid = fork()))
   {
      cout << " Child of mainWSPCS about to start RTM" << endl;
      execlp("rtm", "rtm", "-r", "../pictures_ucn56EOF/UCN5and6.pctx", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start rtm *****" << endl;
      exit(127);
   }
   // save the pid returned in the parent process.
   IPO.setPidRTM(pid);
   cout << " pidRTM = " << dec << IPO.getPidRTM() << endl; 
   //sleep(5);
   sleep(1);

   cout << "*** About to fork() Timer ***" << endl;
   if (0 == (pid = fork()))
   {
      cout << " Child of mainWSPCS about to start Timer" << endl;
      execlp("Timer", "Timer", "-a", "UCN5and6", "-p", "TheTimer", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start Timer *****\n";
      flush(cout);
      exit(127);
   }
   // save the pid returned in the parent process.
   IPO.setPidTimer(pid);
   cout << " pidTimer = " << dec << IPO.getPidTimer() << endl; 
   //sleep(10);
   sleep(1);

   UpdateWarmStartStatus();

// by feel9 
////////////////////////////////////////////////////////////////
   // don't start CommLink for bad host name
   if (0 != strcmp(IPO.getHostName(1),""))
   {
      cout << "*** About to fork() CommLink1 ***\n";
      flush(cout);

      if (0 == (pid = fork()))
      {
         cout << " Child of mainWSPCS about to start CommLink1\n";
         flush(cout);
         sprintf(digitStr, "%d", IPO.getHostNum(1));
         execlp("CommLink", "CommLink1", digitStr, IPO.getHostName(1), IPO.getPortNo(1), IPO.getSENDOption(szSendUse), (char *)0 );

         cout << "\n***** mainWSPCS: EXITING... Unable to start CommLink1\n";
         flush(cout);
         exit(127);
      } 
   }
   else
   {
      pid = 0;
      cout << "***** CommLink1 not started because of invalid host name\n";
   }
   // save the pid returned in the parent process.
   IPO.setPidCommLink1(pid);
   cout << " pidCommLink1 = " << dec << IPO.getPidCommLink1() << endl; 

   UpdateWarmStartStatus();
///////////////////////////////////////////////////////////////////////////////////
//

// DEBUG by feel9 
   sleep(1);
   // don't start CommLink for bad host name
   if (0 != strcmp(IPO.getHostName(2),""))
   {
      cout << "*** About to fork() CommLink2 ***\n";
      flush(cout);
      if (0 == (pid = fork()))
      {
         cout << " Child of mainWSPCS about to start CommLink2\n";
         flush(cout);
         memset(szSendUse, 0x00, sizeof(szSendUse));
	 sprintf(digitStr, "%d", IPO.getHostNum(2));
	 execlp("CommLink", "CommLink2", digitStr, IPO.getHostName(2), IPO.getPortNo(2), IPO.getSENDOption(szSendUse), (char *)0 );

         cout << "\n***** mainWSPCS: EXITING... Unable to start CommLink2\n";
         flush(cout);
         exit(127);
      } 
   }
   else
   {
      pid = 0;
      cout << "***** CommLink2 not started because of invalid host name\n";
   }
   // save the pid returned in the parent process.
   IPO.setPidCommLink2(pid);
   cout << " pidCommLink2 = " << dec << IPO.getPidCommLink2() << endl; 

   UpdateWarmStartStatus();
////////////////////////////////////////////////////////////////////////////////////////
//


   // Before forkexec of InterfaceToGUI, be sure we have the database
   //from the server.
   
   while(TRUE != IPO.getReceivedDBdownload())
   {
      cout << "WSPCS: Waiting for database download to complete..." 
	   << endl;

      // if we lost a process while waiting for the database to load,
      //don't wait here forever, return without all the processes started 
      //so that we can restart again.
      if (BADSTATUS == goodStatus()) 
      {
	 cout << "\n\n*****WSPCS received BAD STATUS from a process "
	      << "while waiting for database to download" << endl;
         cout << "InterfaceToGUI will not be started!" << endl << endl;
	 return;
      } //endif

      for (int i=0; i < GOOD_STATUS_INTERVAL; i++)
      {
   	 //cout << " SON TEST 111111 !!!!!!!!!!! " <<  endl; 
	 setConnected();

	 EOFHeartBeat();
	 if (TRUE != IPO.getReceivedDBdownload())
	 {
            if (Interface->System.connected)
	    {
               UpdateWarmStartStatus();
	       EOFHeartBeat();
	    }
	 }
	 else
	 {
	    break;
         }
	 sleep(1);
      }
   }

   cout << " SON TEST END  " <<  endl; 

   UpdateWarmStartStatus();
	 sleep(1);

// by feel9
	 EOFHeartBeat();
	 sleep(2);

   cout << " SON TEST END !!!!! " <<  endl; 

   cout << "*** About to fork() InterfaceToGUI ***" << endl;
   if (0 == (pid = fork()))
   {
      cout << " Child of mainWSPCS about to start InterfaceToGUI\n";
      flush(cout);
      execlp("InterfaceToGUI", "InterfaceToGUI", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start InterfaceToGUI *****\n";
      flush(cout);
      exit(127);
   }
   
   // save the pid returned in the parent process.
   IPO.setPidInterfaceToGUI(pid);
   cout << " pidInterfaceToGUI = " << dec << IPO.getPidInterfaceToGUI() << endl; 
   UpdateWarmStartStatus();

// by feel9 
	 EOFHeartBeat();

   char tmpbuf[5];
   if( 0 == strcmp(getFDDIOption(tmpbuf),"ON") )
   {
      sleep(1);
      cout << "*** About to fork() FDDIstart ***\n";
      flush(cout);
      if (0 == (pid = fork()))
      {
         cout << " Child of mainWSPCS about to start FDDIstart\n";
         flush(cout);
         //execl("/usr/bin/ksh", "FDDIstart", "-c", "export TERM=vt52; tail -f ./fddi.in | fddimon seah1 | FDDIparse | sysLog -l fddi.out -s 5000 >/dev/null", 0);
         execl("/bin/bash", "FDDIstart", "-c", "export TERM=vt52; tail -f ./fddi.in | fddimon seah1 | FDDIparse | sysLog -l fddi.out -s 5000 >/dev/null", 0);	// 2012.07.05
         cout << "\n***** mainWSPCS: EXITING... Unable to start FDDIstart *****\n";
         flush(cout);
         exit(127);
      }
   
      // save the pid returned in the parent process.
      IPO.setPidFDDIstart(pid);
      cout << " pidFDDIstart = " << dec << IPO.getPidFDDIstart() << endl; 
   }
   sleep(1);

   UpdateWarmStartStatus();

// by feel9 
	 EOFHeartBeat();

   if (GOODSTATUS == goodStatus())
   {
      Interface->System.wsState |= GOOD_PROCESSES_WS_STATE;
   }

// ujson add 20210303  ucn56 delete
/*
   cout << "*** About to fork() DBWSPCS ***" << endl;
   if (0 == (pid = fork()))
   { // child process ONLY
      cout << " Child of mainWSPCS about to start DBWSPCS" << endl;
      execlp("DBWSPCS", "DBWSPCS", (char *)0);
      cout << "\n***** mainWSPCS: EXITING... Unable to start DBWSPCS *****"
           << endl;
      exit(127);
   }
   // save the pid returned in the parent process.
   IPO.setPidDBWSPCS(pid);
   cout << " pidDBWSPCS = " << dec << IPO.getPidDBWSPCS() << endl; 
*/
   sleep(4);

   cout << "***** mainWSPCS after forks and execs *****\n";
   cout << " pidWSPCS = " << dec << IPO.getPidWSPCS() 
        << " pidControl = " << dec << IPO.getPidControl() 
        << " pidTicker = " << IPO.getPidTicker() 
        << " pidRTM = " << IPO.getPidRTM() 
        << " pidTimer = " << IPO.getPidTimer() 
        << " pidCommLink1 = " << IPO.getPidCommLink1() 
        << " pidCommLink2 = " << IPO.getPidCommLink2() 
        << " pidInterfaceToGUI = " << IPO.getPidInterfaceToGUI()
        << " pidFDDIstart = " << IPO.getPidFDDIstart() << endl;
   cout << "**************************************\n";
   cout << "** Finished Starting WSPCS Processes *\n";
   cout << "**************************************\n";
   flush(cout);

// 
	 EOFHeartBeat();

   Interface->System.wsState |= READY_WS_STATE; //201027_jhp


} // end startAll() 

const int	NUMPROCS	= 9;		// by feel9 
//const int	NUMPROCS	= 11;

#ifndef LINUX_APP
static long	lastUserCycles[NUMPROCS];	
static long	lastSystemCycles[NUMPROCS];	
#endif

int WSPCSObject::goodStatus()
{
   extern InterProcSMObject       IPO;

#ifndef LINUX_APP
   struct	pst_status	pst[NUMPROCS];	// CORENET 2012.07.05
#endif
   int		i, count;
   int		pidArray[NUMPROCS];
   int		retval;


   pidArray[0] = IPO.getPidWSPCS();
   pidArray[1] = IPO.getPidControl();
   pidArray[2] = IPO.getPidRTM();

//F by feel9 20200212
   pidArray[3] = IPO.getPidCommLink1();
   pidArray[4] = IPO.getPidCommLink2();
   pidArray[5] = IPO.getPidInterfaceToGUI();
   pidArray[6] = IPO.getPidFDDIstart();
   pidArray[7] = IPO.getPidTicker();
   pidArray[8] = IPO.getPidTimer();

/*
   pidArray[3] = IPO.getPidCommLink2();
   pidArray[4] = IPO.getPidInterfaceToGUI();
   pidArray[5] = IPO.getPidFDDIstart();
   pidArray[6] = IPO.getPidTicker();
   pidArray[7] = IPO.getPidDBWSPCS();
   pidArray[8] = IPO.getPidDBCommLink1();
   pidArray[9] = IPO.getPidDBCommLink2();
   pidArray[10] = IPO.getPidTimer();
*/

   //retval = 0;
   retval = GOODSTATUS;	// CORENET 2012.07.05
   
   //>>>>>>>>>>>>>>>>>> // CORENET 2012.07.05
#ifdef LINUX_APP
   proc_t proc_info;
   static proc_t new_stat[NUMPROCS], old_stat[NUMPROCS];
   PROCTAB *proc = openproc(PROC_FILLSTAT);
   memset(&proc_info, 0, sizeof(proc_info));
   while (readproc(proc, &proc_info) != NULL) {
	for (i = 0; i < NUMPROCS; i++) {
		if (pidArray[i] == proc_info.tid) {
//			cout << endl;
//			cout << "****************** cmd = " << proc_info.cmd << endl;
//			cout << "****************** tid = " << dec <<  proc_info.tid << endl;
//			cout << "****************** state = " << proc_info.state << endl;
//			cout << "****************** pcpu = " << dec << proc_info.pcpu <<  endl;
//			cout << "****************** utime = " << dec << proc_info.utime <<  endl;
//			cout << "****************** stime = " << dec << proc_info.stime <<  endl;
//			cout << endl;
			memcpy((char *)&new_stat[i], (char *)&proc_info, sizeof(proc_t));
		}
	}
   }
   closeproc(proc);
   //<<<<<<<<<<<<<<<<<<<<<<<<
#endif

   for (i = 0; i < NUMPROCS; i++)
   {
      if (0 == pidArray[i]) continue;   // check next process if no pid
 
#ifdef LINUX_APP
      cout << "PROCESS STATE (" << dec << i << ") = [" << new_stat[i].state << "]" << endl; 
      if (new_stat[i].state != 'R' && new_stat[i].state != 'S'
      && new_stat[i].state != 'D' && new_stat[i].state != 'W') {
        cout << endl;
        cout << "PROCESS ERROR (" << dec << i << ") = [" << new_stat[i].state << "]" << endl; 
	return(BADSTATUS); 
      }
#else
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> HPUX
      count = pstat_getproc(&pst[i], sizeof(pst[i]), 0, pidArray[i]);
      if (-1 == count)
      {
	 cout << "goodStatus() error with pstat_getproc() for pid = "
	      << dec << pidArray[i] << endl;
	 perror("pstat_getproc()");
	 return(BADSTATUS);
      }

      switch((int)pst[i].pst_stat)
      {
	 case PS_SLEEP:
#ifdef DEBUG
            cout << "pid = " << dec << pst[i].pst_pid << " name = " 
		 << pst[i].pst_ucomm << " is sleeping (GOOD)\n";
#endif
	    retval = GOODSTATUS;
	    break;
         case PS_RUN:
#ifdef DEBUG
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is running (GOOD)\n";
#endif
	    retval = GOODSTATUS;
	    break;
	 case PS_STOP:
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is stopped (BAD)\n";
	    retval = BADSTATUS;
	    break;
	 case PS_ZOMBIE:
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is dead (BAD)\n";
	    retval = BADSTATUS;
	    break;
	 case PS_IDLE:
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is forming (BAD)\n";
	    retval = BADSTATUS;
	    break;
	 case PS_OTHER:
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is othering (BAD)\n";
	    retval = BADSTATUS;
	    break;
         default:
            cout << "pid = " << dec << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " is of unknown state (BAD)\n";
	    retval = BADSTATUS;
	    break;
      } // endswitch
      flush(cout);
#ifdef DEBUG
      cout << "  Status = " << dec << pst[i].pst_stat 
	   << " CPU total ticks = " << pst[i].pst_cptickstotal << endl;
      cout << "   proc util: " << pst[i].pst_cpu 
	   << " user cycles: " << pst[i].pst_usercycles.psc_hi 
	   << pst[i].pst_usercycles.psc_lo
	   << " system cycles: " << pst[i].pst_systemcycles.psc_hi 
	   << pst[i].pst_systemcycles.psc_lo
	   << endl;
#endif
      if (BADSTATUS == retval) return(retval);

      // Check for inactivity.
      // Check only Ticker, rtm, and InterfaceToGUI tasks which 
      //do not have long periods of inactivity.
      if ((pst[i].pst_pid == IPO.getPidRTM())  ||
	  (pst[i].pst_pid == IPO.getPidInterfaceToGUI())  ||
	  (pst[i].pst_pid == IPO.getPidTicker()) )
      {
	 // if no user or system cycles used since last check,
	 //and processor utilization is 0, assume process has a problem.
         if (lastUserCycles[i] == pst[i].pst_usercycles.psc_lo  &&
             lastSystemCycles[i] == pst[i].pst_systemcycles.psc_lo  &&
	     0 == pst[i].pst_cpu)
         {
            cout << "  NO ACTIVITY Status = " << dec << pst[i].pst_stat 
	         << " CPU total ticks = " << pst[i].pst_cptickstotal << endl;
            cout << "pid = " << pst[i].pst_pid << " name = "
		 << pst[i].pst_ucomm << " has NO ACTIVITY" << endl;
            cout << "   proc util: " << pst[i].pst_cpu 
	         << " user cycles: " << pst[i].pst_usercycles.psc_hi 
	         << pst[i].pst_usercycles.psc_lo
	         << " system cycles: " << pst[i].pst_systemcycles.psc_hi 
	         << pst[i].pst_systemcycles.psc_lo
	         << endl;
            return(BADSTATUS);
         } // endif
         else
         {
	    lastUserCycles[i] = pst[i].pst_usercycles.psc_lo;
	    lastSystemCycles[i] = pst[i].pst_systemcycles.psc_lo;
         } // endelse
      } // endif
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
   } // endfor

   return(retval);
} //end goodStatus()


void WSPCSObject::stopAll()
{
   extern InterProcSMObject       IPO;

   cout << endl;
   cout << "*****************************************\n";
   cout << "*****************************************\n";
   cout << "***** Killing WSPCS Child Processes *****\n";
   cout << "*****************************************\n";
   cout << "*****************************************\n";
   flush(cout);

   needToSignalDatabaseLoaded = TRUE;

// ujson add 20210303 ucn56 delete
/*
   if (IPO.getPidDBWSPCS()) 
   {
      cout << "\n   Killing DBWSPCS" << endl;
      kill(IPO.getPidDBWSPCS(),SIGINT);
      waitpid(IPO.getPidDBWSPCS(),NULL,0);
      IPO.setPidDBWSPCS(0);
      cout << "      DBWSPCS killed" << endl;
   }
   else
   {
      cout << "   Not Killing DBWSPCS.  Don't have a Pid" << endl;
   }
   sleep(1);


   if (IPO.getPidDBCommLink1()) 
   {
      cout << "\n   Killing DBCommLink1" << endl;
      kill(IPO.getPidDBCommLink1(),SIGINT);
      waitpid(IPO.getPidDBCommLink1(),NULL,0);
      IPO.setPidDBCommLink1(0);
      cout << "      DBCommLink1 killed" << endl;
   }
   else
   {
      cout << "   Not Killing DBCommLink1.  Don't have a Pid" << endl;
   }
   sleep(1);

   if (IPO.getPidDBCommLink2()) 
   {
      cout << "\n   Killing DBCommLink2" << endl;
      kill(IPO.getPidDBCommLink2(),SIGINT);
      waitpid(IPO.getPidDBCommLink2(),NULL,0);
      IPO.setPidDBCommLink2(0);
      cout << "      DBCommLink2 killed" << endl;
   }
   else
   {
      cout << "   Not Killing DBCommLink2.  Don't have a Pid" << endl;
   }
   sleep(1);
*/


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

   if (IPO.getPidTicker()) 
   {
      cout << "   Killing Ticker" << endl;
      kill(IPO.getPidTicker(),SIGINT);
      waitpid(IPO.getPidTicker(),NULL,0);
      IPO.setPidTicker(0);
      cout << "      Ticker killed" << endl;
   }
   else
   {
      cout << "   Not Killing Ticker.  Don't have a Pid" << endl;
   }
   sleep(1);

   // Be sure the rtm and its processes are killed
   system("stop.ws");

   cout << "*****************************************\n";
   cout << "***** WSPCS Child Processes Killed  *****\n";
   cout << "*****************************************\n";
   flush(cout);
}


void WSPCSObject::setConnected()
{

   extern InterProcSMObject	IPO;
   extern INTERFACE	*	Interface;

   IPO.setconnected(1, TRUE);		// by feel9
   IPO.setconnected(2, TRUE);		// by feel9
   Interface->System.connected = TRUE;	// by feel9 
   Interface->System.cpuNum = 1;			// by feel9 

} // end setConnected()

void WSPCSObject::setConnected_old()
{
   extern InterProcSMObject	IPO;
   extern INTERFACE	*	Interface;

//   cout << "***** setConnected() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  *****\n";
   if (PRIMARY_COMMLINK == IPO.getprimaryStatus(1)  &&
       TRUE == IPO.getconnected(1) )
   {
#ifdef DEBUG
      cout << "   setConnected(): CommLink1 is Primary and is "
	   << "connected to " << IPO.getHostName(1) << endl;
#endif
      Interface->System.connected = TRUE;
   }
   else
   {
      if (PRIMARY_COMMLINK == IPO.getprimaryStatus(2)  &&
	  TRUE == IPO.getconnected(2) )
      {
#ifdef DEBUG
         cout << "   setConnected(): CommLink2 is Primary and is "
	      << "connected to " << IPO.getHostName(2) << endl;
#endif
	 Interface->System.connected = TRUE;
      }
      else
      {
	 Interface->System.connected = FALSE;
	 Interface->System.cpuNum = 0; // CPU NONE
      }
   }
} // end setConnected()

static int	allPointsAreSetBad = FALSE;
void WSPCSObject::determineIfBadInputs()
{
   extern InterProcSMObject	IPO;
   extern DatabaseObject	DBO;

   int i;
   int setBad;

   setBad = FALSE;

   // if there is no primary CommLink, set all points BAD
   if (PRIMARY_COMMLINK != IPO.getprimaryStatus(1)  &&
       PRIMARY_COMMLINK != IPO.getprimaryStatus(2) )
   {
      setBad = TRUE;
   }

   if (readTimeTooLong())
   {
      setBad = TRUE;
   }

   if (setBad)
   {
      if (allPointsAreSetBad) {}
      else // all points are not bad
      {
         cout << " WSPCS() Setting points invalid\n";
         flush(cout);
         for (i = 0; i <= NUM_REC; i++)
         {
            DBO.setnewsta(i,BDACK);
         }
	 allPointsAreSetBad = TRUE;
      }
   }
   else
   {
      allPointsAreSetBad = FALSE;
   }
} // end determineIfBadInputs()

int WSPCSObject::readTimeTooLong()
{
   extern InterProcSMObject	IPO;

   int		primary;
   int		retval;
   int		now;

   now = IPO.getNumTicks();
   primary = getPrimaryConnection();

   if (primary > 0)
   {
      if ((now - IPO.getNumTicksOfLastRead(primary)) > MAX_READ_TIME_FOR_GOOD_DATA)
      {
         retval = TRUE;
      }
      else
      {
         retval = FALSE;
      }
   }
   else
   {
      retval = TRUE;
   }
   return retval;
} // end WSPCSObject::readTimeTooLong()

int WSPCSObject::getPrimaryConnection()
{
   extern InterProcSMObject	IPO;

   int retval;

   if (PRIMARY_COMMLINK == IPO.getprimaryStatus(1) &&
       PRIMARY_COMMLINK == IPO.getprimaryStatus(2) )
   {
      retval = 0;
   }
   else if (PRIMARY_COMMLINK == IPO.getprimaryStatus(1))
   {
      retval = 1;
   }
   else if(PRIMARY_COMMLINK == IPO.getprimaryStatus(2))
   {
      retval = 2;
   }
   else retval = 0;

   return retval;
} // end WSPCSObject::getPrimaryConnection()


void WSPCSObject::processWsStateChange(int oldState, int newState)
{
   cout << endl << "***** WS STATE CHANGE from " << hex << oldState
        << " to " << newState << endl;

   // State transition processing.
   switch(oldState)
   {
      case DEAD_WS_STATE:
	 cout << "   Processing old state DEAD_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case STARTING_WS_STATE:
	       cout << "   Processing new state STARTING_WS_STATE = " 
		    << newState << endl;
	       break;
            default:
	       INSmessage(newState);
               USTmessage(oldState, newState);
               break;
         } // endswitch
	 break;
      case STARTING_WS_STATE:
	 cout << "   Processing old state STARTING_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case BUILD_SL_WS_STATE:
	       cout << "   Processing new state BUILD_SL_DONE_WS_STATE = " 
		    << newState << endl;
	       break;
            case PB_DETERMINED_WS_STATE:
               cout << "   Processing new state PB_DETERMINED_WS_STATE = "
                    << newState << endl;
               cout << "      Primary / Backup is determined but not all "
                    << "processes have a good status." << endl;
               break;
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
	 } // endswitch
	 break;
      case PB_DETERMINED_WS_STATE:
	 cout << "   Processing old state PB_DETERMINED_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case STARTING_WS_STATE:
	       cout << "   Processing new state STARTING_WS_STATE = " 
		    << newState << endl;
	       break;
            default:
	       INSmessage(newState);
               USTmessage(oldState, newState);
               break;
         } // endswitch
	 break;
      case BSL_LOST_GP_WS_STATE:
	 cout << "   Processing old state BSL_LOST_GP_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case STARTING_WS_STATE:
	       cout << "   Processing new state STARTING_WS_STATE = " 
		    << newState << endl;
	       break;
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
         } // endswitch
      case BUILD_SL_WS_STATE:
	 cout << "   Processing old state BUILD_SL_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case BSL_LOST_GP_WS_STATE:
	       cout << "   Processing new state BSL_LOST_GP_WS_STATE = "
		    << newState << endl;
	       break;
	    case READY_WS_STATE:
	       cout << "   Processing new state READY_WS_STATE = "
		    << newState << endl;
	       break;
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
	 } // endswitch
	 break;
      case READY_WS_STATE:
	 cout << "   Processing old state READY_WS_STATE = "
	      << oldState << endl;
	 switch(newState)
	 {
	    case STARTING_WS_STATE:
	       cout << "   Processing new state STARTING_WS_STATE = "
		    << newState << endl;
	       break; 
	    case NEW_PB_WS_STATE:
	       cout << "   Processing new state NEW_PB_WS_STATE = "
		    << newState << endl;
	       break;
	    case DBDEF_READY_WS_STATE:
	       cout << "   Processing new state DBDEF_READY_WS_STATE = "
		    << newState << endl;
               cout << "     ** New database definition. **" << endl;
	       break;
	    case COM_LOST_WS_STATE:
	       cout << "   Processing new state COM_LOST_WS_STATE = "
		    << newState << endl;
	       cout << endl << "***** Lost host connection, everything else OK"
		    << endl << endl;
               break;
	    case PROC_LOST_WS_STATE:
	       cout << "   Processing new state PROC_LOST_WS_STATE = "
		    << newState << endl;
	       cout << endl << "***** Lost one or more processes."
		    << endl << endl;
               break;
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
	 } // endswitch
	 break;
      case COM_LOST_WS_STATE:
	 cout << "   Processing old state COM_LOST_WS_STATE = "
	      << oldState << endl;
	 switch(newState)
	 {
            case NEW_PB_WS_STATE:
	       cout << "   Processing new state NEW_PB_WS_STATE = "
		    << newState << endl;
               break;
	    case READY_WS_STATE:
            {
	       cout << "   Processing new state READY_WS_STATE = "
		    << newState << endl;
	       break;
            }
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
	 } // endswitch
	 break;
      case PROC_LOST_WS_STATE:
	 cout << "   Processing old state PROC_LOST_WS_STATE = " 
	      << oldState << endl;
	 switch(newState)
	 {
	    case STARTING_WS_STATE:
            {
	       cout << "   Processing new state STARTING_WS_STATE = "
		    << newState << endl;
	       break;
            }
	    default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
	 } // endswitch
         break;
      case NEW_PB_WS_STATE:
	 cout << "   Processing old state NEW_PB_WS_STATE = "
	      << oldState << endl;
	 switch(newState)
	 {
	    case READY_WS_STATE:
            {
	       cout << "   Processing new state READY_WS_STATE = "
		    << newState << endl;
	       break;
	    }
	    case DBDEF_NEW_PB_WS_STATE:
	       cout << "   Processing new state DBDEF_NEW_PB_WS_STATE = "
		    << newState << endl;
	       break;
            default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
         } // endswitch
	 break;
      case DBDEF_NEW_PB_WS_STATE:
	 cout << "   Processing old state DBDEF_NEW_PB_WS_STATE = "
	      << oldState << endl;
         switch(newState)
         {
	    case NEW_PB_WS_STATE:
	       cout << "   Processing new state NEW_PB_WS_STATE = "
		    << newState << endl;
	       break;
            default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
         } //endswitch
         break;
      case DBDEF_READY_WS_STATE:
	 cout << "   Processing old state DBDEF_READY_WS_STATE = "
	      << oldState << endl;
         switch(newState)
         {
	    case READY_WS_STATE:
	       cout << "   Processing new state READY_WS_STATE = "
		    << newState << endl;
	       break;
            default:
	       INSmessage(newState);
	       USTmessage(oldState, newState);
	       break;
         } // endswitch
         break;
      default:
	 cout << "   Processing INVALID old state, default: case" << endl;
	 USTmessage(oldState, newState);
         break;
   } // endswitch
   cout << endl;
   flush(cout);
} // end WSPCSObject::processWsStateChange(int oldState, int newState)


void WSPCSObject::USTmessage(int oldState, int newState)
{
   cout << "***** UNKNOWN STATE TRANSITION from "
        << hex << oldState << " to new state = " << newState << endl << endl;
}

void WSPCSObject::INSmessage(int newState)
{
   cout << "   Processing default: INVALID new state = " << newState << endl;
}

void WSPCSObject::UpdateWarmStartStatus()
{
   system("Execute -a UCN5and6 -r rtm -c \"{::UCN5and6.PicUCN_WarmStart.ShowProgress();}\" > /dev/null");
}

void WSPCSObject::setMODEMOption(const char * s)
{
   cout << "WSPCSObject::setMODEMOption: setting MODEM option to " << s 
	<< endl;
   if (MODEM_OPTION_LEN >= strlen(s))
   {
      strcpy(WSPCSinfo.MODEM, s);
   }
   else
   {
      cout << "WSPCSObject::setMODEMOption: ERROR Length of modem string "
           << s << " is " << dec << strlen(s) 
	   << ", which exceeds maximum length of " << dec << MODEM_OPTION_LEN 
	   << endl;
      cout << "EXITING..." << endl;
      exit(127);
   }
}

// feel9 
void WSPCSObject::EOFHeartBeat()
{
// This method is parseHostMessages's HEARTBEAT Role.

	//      case HEARTBEAT:
	// keep alive message, just update packet time
	
	//if (PRIMARY_COMMLINK == pb_status) 
	//{
  //         cout << getname() << ": Received Heartbeat message from CPU " 
	//        << dec << PHmsg.getcpuNumber() << " M/S, FS = " 
	//	<< PHmsg.getmspbStatus() << " " 
	//	<< PHmsg.getfailoverState() << endl;

	   extern	INTERFACE       *Interface;	// shared memory to Picasso variables
	   extern	DatabaseObject		DBO;	// Database object shared memory object

   	   Interface->System.wsState |= PB_DETERMINED_WS_STATE;		//add 20200212 by feel9
	   Interface->System.wsState |= DB_DEF_DONE_WS_STATE;
	   newDBupdatesStarted = FALSE;

	   // If we need to let it be known that the database is loaded.
	   if (TRUE == needToSignalDatabaseLoaded)
	   {
              int i;

              // after the database is loaded, update the PID strings for 
              //the variable alarm setpoints
              for(i = 0; i < NUM_REC; i++)
              {
                 if(DBO.isanalog(i))
                 {
                    if(0 != DBO.getlIPN(i))
                    {
                       DBO.setmessage1(i,DBO.getpid(DBO.getlIPN(i)));
#ifdef DEBUG
                       cout << getname() << ": For PID " << DBO.getpid(i)
                            << ", setting variable low alarm setpoint to "
                            << DBO.getpid(DBO.getlIPN(i)) << endl;
#endif
                    } // endif got a variable high alarm setpoint
                    if(0 != DBO.gethIPN(i))
                    {
                       DBO.setmessage2(i,DBO.getpid(DBO.gethIPN(i)));
#ifdef DEBUG
                       cout << getname() << ": For PID " << DBO.getpid(i)
                            << ", setting variable high alarm setpoint to "
                            << DBO.getpid(DBO.gethIPN(i)) << endl;
#endif
                    } // endif got a variable low alarm setpoint
                 } // endif isanalog
              } // endfor all ipns

	      pid_t ppid;

	      // IF CommLink was not spawned as a child task, 
	      // tell the user that the database has been downloaded,
	      // and to Manually start InterfaceToGUI
	      ppid = getppid();
	      //cout << endl << getname() << ": WSPCS ppid, pid = " << dec 
		  // << ppid << " " << dec << IPO.getPidWSPCS() << endl;

              //cout << getname() << ": setting" 
              //     << " IPO.setReceivedDBdownload to TRUE " << endl;

              IPO.setReceivedDBdownload(TRUE);
	      needToSignalDatabaseLoaded = FALSE;
	      
	      if (IPO.getPidWSPCS() != ppid)
	      {
	         cout << "*** EOF HEART BEAT SON " << endl;
	      }
	   }//endif needToSignal == TRUE
	//}//endif primary CommLink
	//else 
	//{
	//   if (BACKUP_COMMLINK == pb_status)
	//   {
	//      needToSignalDatabaseLoaded = FALSE;
	//   }
	//}
}

