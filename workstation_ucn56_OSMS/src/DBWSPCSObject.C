
#include <iostream.h>
#include <fstream.h>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "DBWSPCSObject.h"
#include "DBCommLinkObject.h"
#include "InterProcSMObject.h"

DBWSPCSinfoStruct DBWSPCSObject::DBWSPCSinfo;

extern InterProcSMObject     IPO;   // Inter Process Shared Memory

DBWSPCSObject::DBWSPCSObject()
{
    char hostnameArray[MAX_HOSTNAME_LEN];
    char hostPortnoArray[MAX_PORTNO_LEN];

    setHost1Name(HOST_NAME_A);
    setHost2Name(HOST_NAME_B);
    setSENDOption("1");

    if( NULL == gethostbyname(getHost1Name(hostnameArray))) // HOSTNAME 1 
    {
        cout << "***** ERROR ***** HOST_NAME_A is not a valid host\n";
	cout << "                  Fix appropriate .h file and re-make system\n";
    }
    
    if( NULL == gethostbyname(getHost2Name(hostnameArray))) // HOSTNAME 2
    {
        cout << "***** ERROR ***** HOST_NAME_B is not a valid host\n";
	cout << "                  Fix appropriate .h file and re-make system\n";
    }

    if( NULL == getHost1PortNo(hostPortnoArray))
    {
        cout << "***** ERROR ***** HOST_PORT_A is not a valid host port1\n";
	cout << "                  Fix appropriate .h file and re-make system\n";
    }
    if( NULL == getHost2PortNo(hostPortnoArray))
    {
        cout << "***** ERROR ***** HOST_PORT_B is not a valid host port2\n";
	cout << "                  Fix appropriate .h file and re-make system\n";
    }
}


DBWSPCSObject::~DBWSPCSObject()
{
}

void DBWSPCSObject::parseDBOptions(int numOptions, char * argv[])
{
    extern InterProcSMObject IPO; 
    
    int optionCounter, j, optionIndex;

    char hostnameArray[MAX_HOSTNAME_LEN];
    char portnoArray[MAX_PORTNO_LEN];
   
    memset(hostnameArray, 0x00, sizeof(MAX_HOSTNAME_LEN));
    memset(portnoArray, 0x00, sizeof(MAX_PORTNO_LEN));

    cout << argv[0] << ":DBWSPCSObject::parseDBOptions()" << endl;

    if( 1 == numOptions )
    {
        cout << argv[0] << " No input options." << endl;
    }
    else
    {
        cout << argv[0] << " Number of input(s) = " << dec << numOptions << endl;
    }

    for( optionCounter = 1; optionCounter < numOptions; optionCounter++)
    {
	cout << "  considering command line option " << dec
	     << optionCounter << " = " << argv[optionCounter]
	     << endl;
 
        for(optionIndex = 0; optionIndex < NUM_OPTIONS; optionIndex++) 
        {
	    if( 0 == strcmp(options[optionIndex].str, argv[optionCounter] ))
	    {
                cout << "   found a valid option = " << argv[optionCounter] << endl;
	        optionCounter++; // index to next command line argument
 
                options[optionIndex].used = TRUE;
                switch(optionIndex)
	        {
                    case HOST1_NAME_INDEX:
                    {
                        cout << "     " << argv[0] << ": Setting CommLink1 host to "
			     << argv[optionCounter] << endl;
		    
                        if(NULL != gethostbyname(argv[optionCounter]))
		        {
		            setHost1Name(argv[optionCounter]);    	
		            IPO.setDBHostName(1, argv[optionCounter]);
		        }
                        else
		        {
                            cout << "     " << argv[0] << ": Setting CommLink1 host "
                                 << "to NULL because gethostbyname(" << argv[optionCounter] << ") FAILED." << endl;
		    
		            setHost1Name("");
		            IPO.setDBHostName(1, "");
		        }
		        break;
		    }
		    case HOST1_PORTNO_INDEX:
		    {
		        cout << "     " << argv[0] << ": Setting CommLink1 port to "
		             << argv[optionCounter] << endl;

                        setHost1PortNo(argv[optionCounter]);
	                IPO.setDBPortNo(1, argv[optionCounter]);	
		        break;
	 	    } 
		    case HOST2_NAME_INDEX:
		    {
		        cout << "     " << argv[0] << ": Setting CommLink2 host to "
			     << argv[optionCounter] << endl;

                        if(NULL != gethostbyname(argv[optionCounter]))
		        {
                            setHost2Name(argv[optionCounter]);    
		            IPO.setDBHostName(2, argv[optionCounter]);
		        }
		        else
		        {
                            cout << "     " << argv[0] << ": Setting CommLink2 host "
			         << "to NULL because gethostbyname(" << argv[optionCounter] << ") FAILED." << endl;
		            setHost2Name("");
		            IPO.setDBHostName(2, "");
		        }
		        break;
                    }
		    case HOST2_PORTNO_INDEX:
		    {
		        cout << "     " << argv[0] << ": Setting CommLink2 port to "
			     << argv[optionCounter] << endl;

                        setHost2PortNo(argv[optionCounter]);
	                IPO.setDBPortNo(2, argv[optionCounter]);	
		        break;
	 	    }
                    case SEND_USE_INDEX:
		    {
                        cout << "     " << argv[0] << ": Setting Send Use "
			     << argv[optionCounter] << endl;
	
			setSENDOption(argv[optionCounter]);
			IPO.setSENDOption(argv[optionCounter]);
                        break;
		    }
		    default:
		    {
		        cout << "     " << argv[0] << ": Not a valid option.   "
			 << "This can never happen.\n";
		        break;
		    }
	        }
	        break;
	    }
        }
        if (optionIndex >= NUM_OPTIONS)
        {
            cout << argv[0] << ": ***** INVALID OPTION ***** "
	         << argv[optionCounter] << endl;

            cout << "   Valid options are: " << endl;
	    for(j = 0; j < NUM_OPTIONS; j++)
	    {
                cout << "     " << options[j].str << endl;
	    }
        }
    }
    char szSendOption[5];
    memset(szSendOption, 0x00, sizeof(szSendOption));
    IPO.getSENDOption(szSendOption);

    if ( (0 == strcmp(IPO.getDBHostName(1), "")) &&
	 (0 == strcmp(IPO.getDBHostName(2), "")) && 
	 (0 == strcmp(IPO.getDBPortNo(1),   "")) &&
	 (0 == strcmp(IPO.getDBPortNo(2),   "")) &&
	 (0 == strcmp(szSendOption, "")))
    {
        cout << "***** ERROR ***** No valid hosts have been selected!\n";
	cout << "***** EXITING...\n\n";
	flush(cout);
	exit(1);
    }

}

void DBWSPCSObject::mainLoop()
{
    startAll();

    for (;;)
	sleep(1);
}

void DBWSPCSObject::startAll()
{
    pid_t     pid;
    char      digitStr[2];
    char      szSendUse[5];

    cout << endl;
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "** Starting DBWSPCS Processes *\n";
    cout << "*******************************\n";
    cout << "*******************************\n";
    cout << "*******************************\n\n";

    // IPO.setPidDBWSPCS(getpid());
    IPO.setPidDBCommLink1(0);
    IPO.setPidDBCommLink2(0);

    cout << "pidDBWSPCS = " << dec << IPO.getPidDBWSPCS() << endl;
 
    char szHost1Name[MAX_HOSTNAME_LEN];
    char szHost2Name[MAX_HOSTNAME_LEN];

    char szHost1PortNo[MAX_PORTNO_LEN];
    char szHost2PortNo[MAX_PORTNO_LEN];

    memset(szHost1Name, 0x00, sizeof(MAX_HOSTNAME_LEN));
    memset(szHost2Name, 0x00, sizeof(MAX_HOSTNAME_LEN));

    memset(szHost1PortNo, 0x00, sizeof(MAX_PORTNO_LEN));
    memset(szHost2PortNo, 0x00, sizeof(MAX_PORTNO_LEN));

    if( (0 != strcmp(IPO.getDBHostName(1), ""))
	&& ( 0 != strcmp(IPO.getDBPortNo(1), "")) ) 
    {
        cout << "*** About to fork() DBCommLink1 ***\n";
	flush(cout);

	if( 0 == ( pid = fork()))
	{
	    cout << " Child of mainDBWSPCS about to start DBCommLink1\n";            
            flush(cout);
            sprintf(digitStr, "%d", IPO.getDBHostNum(1));
            memset(szSendUse, 0x00, sizeof(szSendUse));
	    execlp("DBCommLink", "DBCommLink1", digitStr, IPO.getDBHostName(1), IPO.getDBPortNo(1), IPO.getSENDOption(szSendUse), (char *)0 );
            cout << "\n***** mainDBWSPCS: EXITING... Unable to start DBCommLink1\n";
	    flush(cout);
	    exit(127);
        }
    }
    else
    {
        pid = 0;	
        cout << "***** CommLink1 not started because of invalid hostname or portno\n";
    }
    IPO.setPidDBCommLink1(pid);
    cout << " pidCommLink1 = " << dec << IPO.getPidDBCommLink1() << endl;

    if( (0 != strcmp(IPO.getDBHostName(2), ""))
       & ( 0 != strcmp(IPO.getDBPortNo(2), "")) )  
    {
        cout << "*** About to fork() DBCommLink2 ***\n";
	flush(cout);

        if (0 == (pid = fork()))
	{
            cout << " Child of mainDBWSPCS about to start DBCommLink2\n";
	    flush(cout);
	    sprintf(digitStr, "%d", IPO.getDBHostNum(2));
            memset(szSendUse, 0x00, sizeof(szSendUse));
	    execlp("DBCommLink", "DBCommLink2", digitStr, IPO.getDBHostName(2), IPO.getDBPortNo(2), IPO.getSENDOption(szSendUse), (char *)0 );
	    cout << "\n***** mainDBWSPCS: EXITING... Unable to start DBCommLink2\n";
	    flush(cout);
	    exit(127);
	}
    }
    else
    {
        pid = 0;
	cout << "***** CommLink2 not started because of invalid hostname or portno\n";
    }
    IPO.setPidDBCommLink2(pid);
   
    cout << "***** mainDBWSPCS after forks and execs *****\n";
    cout << " pidDBWSPCS = " << dec << IPO.getPidDBWSPCS()
	 << " pidCommLink1 = " << IPO.getPidDBCommLink1()
	 << " pidCommLink2 = " << IPO.getPidDBCommLink2() << endl;
    cout << "****************************************\n";
    cout << "** Finished Starting DBWSPCS Processes *\n";
    cout << "****************************************\n";
    
    flush(cout);
}

void DBWSPCSObject::stopAll()
{
/*
    extern InterProcSMObject     IPO;

    cout << endl;
    cout << "*******************************************\n";
    cout << "*******************************************\n";
    cout << "***** Killing DBWSPCS Child Processes *****\n";
    cout << "*******************************************\n";
    cout << "*******************************************\n";

    flush(cout);

    if( IPO.getPidDBCommLink1() )
    {
        cout << "   Killing CommLink1" << endl;
	kill(IPO.getPidDBCommLink1(), SIGINT);
	waitpid(IPO.getPidDBCommLink1(), NULL, 0);
	IPO.setPidDBCommLink1(0);
	cout << "      DBCommLink1 killed" << endl;
    }
    else
    {
        cout << "   Not Killing CommLink1.  Don't have a Pid" << endl;
    } 
    sleep(1);

    if( IPO.getPidDBCommLink2() )
    {
        cout << "   Killing CommLink2" << endl;
	kill(IPO.getPidDBCommLink2(), SIGINT);
	waitpid(IPO.getPidDBCommLink2(), NULL, 0);
        IPO.setPidDBCommLink2(0);
	cout << "      DBCommLink2 killed" << endl; 
    }
    else
    {
        cout << "   Not Killing CommLink2.  Don't have a Pid" << endl; 
    }
    sleep(1);
    
   // system("stop.ws");

    cout << "*******************************************\n";
    cout << "***** DBWSPCS Child Processes Killed  *****\n";
    cout << "*******************************************\n";
    
    flush(cout);
*/
}
