
// *******************************************
// parseDBDownMessages.C
// ver 1.0 - 2021.1.15
// Author - Ji Hoon Park

#include <fstream.h>
#include <fcntl.h>
#include <iostream.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>

#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "DBCommLinkObject.h"
#include "InterProcSMObject.h"
#include "systemParameters.h"


extern   InterProcSMObject    IPO;
extern   DBCommLinkObject     DBCommLink; 


#include "prototypes.h"

void DBCommLinkObject::parseDBDownMessages()
{
    char rcvBuffer[4096];
    memset(rcvBuffer, 0x00, sizeof(rcvBuffer));
   
    int len = 4096, rcvlen = 0;

    // read from the Concurrent system 
    rcvlen = u_read(ReadSocket, rcvBuffer, len);   

    if(rcvlen <= 0)
    {
        cout << " parseDBDownMessages in u_read() TIMEOUT!!!" << endl;
	return false; //210118_jhp
    }

    if( (strncmp(rcvBuffer, "TT", 2)) == 0 && (rcvBuffer[6] == 0x01))
    {

        char szPort[MAX_PORTNO_LEN], szDBFileName[20];
	memset(szPort, 0x00, sizeof(szPort));
	memset(szDBFileName, 0x00, sizeof(szDBFileName));

	getPortNo(szPort);
        
	if( !strcmp( szPort, "3333" ))       sprintf(szDBFileName, "%s", "dbimage_3.bin"); 
	else if( !strcmp( szPort, "4444" ))  sprintf(szDBFileName, "%s", "dbimage_4.bin"); 
	else if( !strcmp( szPort, "5555" ))  sprintf(szDBFileName, "%s", "dbimage_5.bin");
	else if( !strcmp( szPort, "6666" ))  sprintf(szDBFileName, "%s", "dbimage_6.bin");
        else                                 sprintf(szDBFileName, "%s", DB_FILENAME); 

        if( -1 == unlink( szDBFileName ))
	{
            if(ENOENT != errno) 
	    {
	        cout << "Error deleting " << szDBFileName << ". errno = " << errno << endl;
	        cout << " Recommand manually deleting " << szDBFileName << endl;
	    }
	}
    }
    else if( (strncmp(rcvBuffer, "TT", 2)) == 0 && (rcvBuffer[2] == rcvBuffer[6])) // End of packet
    {
               
    }

} 
