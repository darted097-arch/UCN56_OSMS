#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/pstat.h>

#include "DBCommLinkObject.h"
#include "systemParameters.h"

DBCommLinkObject DBCommLink;

int main(int argc, char * argv[])
{
    int       CommLinkNum;
    char      CommLinkName[MAX_DB_COMMLINK_NAME_LEN];  // DBCommLink name
    char      hostConnectName[MAX_HOSTNAME_LEN];
    char      hostPortNo[MAX_PORTNO_LEN];
    char      szSendUse[5];   

    memset(CommLinkName, 0x00, sizeof(CommLinkName));
    memset(hostConnectName, 0x00, sizeof(hostConnectName));
    memset(hostPortNo, 0x00, sizeof(hostPortNo));
    memset(szSendUse, 0x00, sizeof(szSendUse));

    if(5 != argc) // 3 -> 4(add PortNo) -> 5(add Send Use) 
    {
	cerr << argv[0] << ": ***** ERROR Incorrent number of arguments" << endl;
	exit(1);
    }

    //Origin : "CommLink"(File Name) , "CommLink1"(arg1) , "HostNo"(arg2) , "HostName"(arg3), "0"(arg4), ...
    
    //Modify : "DBCommLink"(File Name) , "DBCommLink1"(arg1) , "HostNo(arg2) , "HostName"(arg3) , "PortNo"(arg4), "1"(arg5), "0"(arg6)  
    
    sscanf(argv[0], "%s", CommLinkName);    // string : DBCommLink1
    sscanf(argv[1], "%d", &CommLinkNum);    // int : 1 or 2 
    sscanf(argv[2], "%s", hostConnectName); // string : ygn6c1
    sscanf(argv[3], "%s", hostPortNo);      // string : 6666 
    sscanf(argv[4], "%s", szSendUse);       // string : 1 or 2  

    DBCommLink.setnum(CommLinkNum);
    DBCommLink.setname(CommLinkName);
    DBCommLink.sethostName(hostConnectName);
    DBCommLink.setportNo(hostPortNo);
    DBCommLink.setsendUse(szSendUse);  

    cout << "DBCommLinkNum = " << dec << DBCommLink.getnum()
	 << " DB CommLinkName = " << DBCommLink.getname()
	 << " to connect with " << DBCommLink.gethostName()
	 << " port no " << DBCommLink.getportNo() 
         << " send use " << DBCommLink.getsendUse() << endl;   

    DBCommLink.Connect();

    cout << "Type <CTRL> C to terminate" << endl;

    DBCommLink.mainLoop();
}
