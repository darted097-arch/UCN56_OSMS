//
// *********************************************
// mainDbWSPCS.C
// Ver 1.0 - 2021.01.07
// Author - J.H.Park
// *********************************************
//
// *********************************************
#include <iostream.h>
#include <fstream.h>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/pstat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "systemParameters.h"
#include "InterProcSMObject.h"
#include "interface.h"
#include "DBWSPCSObject.h"
#include "DBCommLinkObject.h"
#include "sharedMemoryObject.h"
#include "interface.h"
#include "InterProcSMObject.h"
#include "DatabaseObject.h"

#include "prototypes.h"

INTERFACE       * Interface;

sharedMemoryObject       InterfaceSMO(INTERFACE_KEY, sizeof(INTERFACE), 0);
InterProcSMObject        IPO(INTERPROC_KEY, sizeof(interProc), 0);
DBWSPCSObject            DBWSPCS;

void onInterrupt(int);
void zombieZapper(int);

// ***************************************************************

int main(int argc, char * argv[])
{
    cout << "***** mainDbWSPCS ***** Type [CTRL+C] to TERMINATE S/W\n";
    
    for (int i = 0; i < argc; i++)
    {
	cout << argv[0] << ": arg " << dec << i << " is " << argv[i] << endl;
    }

    flush(cout);

    char hostnameArray[MAX_HOSTNAME_LEN];
    char portnoArray[MAX_PORTNO_LEN];

    memset(hostnameArray, 0x00, sizeof(MAX_HOSTNAME_LEN));
    memset(portnoArray, 0x00, sizeof(MAX_PORTNO_LEN));

    IPO.setDBHostNum(1, 1);
    IPO.setDBHostName(1, DBWSPCS.getHost1Name(hostnameArray));
    IPO.setDBPortNo(1, DBWSPCS.getHost1PortNo(portnoArray));

    IPO.setDBHostNum(2, 2);
    IPO.setDBHostName(2, DBWSPCS.getHost2Name(hostnameArray));
    IPO.setDBPortNo(2, DBWSPCS.getHost2PortNo(portnoArray));
    
    DBWSPCS.setDBWSPCSinfo("DB_WSPCS.cfg");
    DBWSPCS.parseDBOptions(argc, argv);
 
    if(signal(SIGINT, SIG_IGN) != SIG_IGN)
	signal(SIGINT, onInterrupt);

    if(signal(SIGCHLD, SIG_IGN) != SIG_IGN)
	signal(SIGCHLD, zombieZapper);

    DBWSPCS.mainLoop();      
}// end main()

void onInterrupt(int signum)
{
    signal(SIGINT, onInterrupt);

    cout << "\n***** DBWSPCS onInterrupt() ***** received signal "
       << dec << signum << " ***\n";

    cout << endl;
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "***** Terminating DBWSPCS *****\n";
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "*******************************\n";
    
    flush(cout);

    system("stop.ws");

    cout << "********* mainDBWSPCS ending... *********" << endl;
    exit(0);
}

void zombieZapper(int signum)
{
/*
    int status;
    int temp;

    cout << "***** DBWSPCS zombieZapper() *** received signal " << dec << signum << " ***\n";

    while((temp = wait3(&status, WNOHANG, (struct rusage *)0)) > 0)
    {
        cout << "zombieZapper cleaning up for pid = " << dec << temp << endl;

	flush(cout);
    }
    signal(SIGCHLD, zombieZapper);
*/
}// end zombieZapper()
