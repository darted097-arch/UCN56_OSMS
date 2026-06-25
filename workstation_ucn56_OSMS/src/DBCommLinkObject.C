//
// **********************************************************
// DBCommLinkObject.C
// ver 1.0 - 2021.01.13
// Author : Ji Hoon Park
// **********************************************************
//
// *****************************************************************
// This module is the implementation of the CommLink Class defined in DBCommLink.h

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <unistd.h>

#include "interface.h"
#include "messageParameters.h"
#include "Typedefs.h"
#include "databaseParameters.h"
#include "database.h"
#include "sharedMemoryObject.h"
#include "DBCommLinkObject.h"
#include "DatabaseObject.h"
#include "InterProcSMObject.h"
#include "systemParameters.h"

void terminate(int);

// *****************************************************************

int    DBCommLinkObject::bytes_read;
int    DBCommLinkObject::total_bytes_read;

int    DBCommLinkObject::msgSize;
int    DBCommLinkObject::gotMsgSize;
int    DBCommLinkObject::bytesToRead;

int    DBCommLinkObject::ReadSocket;
int    DBCommLinkObject::WriteSocket;

int    DBCommLinkObject::num;
char   DBCommLinkObject::name[MAX_DB_COMMLINK_NAME_LEN];
char   DBCommLinkObject::hostName[MAX_HOSTNAME_LEN];
char   DBCommLinkObject::portNo[MAX_PORTNO_LEN];
int    DBCommLinkObject::connected;
char   DBCommLinkObject::sendUse[5];

ofstream DBCommLinkObject::eventFile;
ofstream DBCommLinkObject::tagIDFile;

extern DatabaseObject        DBO;             // for Update Database
extern InterProcSMObject     IPO;             // Inter Process Shared Memory

#define SOCK_BUFF_SIZE       4096 

DBCommLinkObject::DBCommLinkObject()
{
    cout << "DB CommLink Object Constructor " << endl;
    initDBCommLink();
}

DBCommLinkObject::~DBCommLinkObject()
{
    cout << "DBCommLinkObject Destructor:" << endl;
    // close sockets used to read data -> No Write
    if( 0 == close(ReadSocket))
        cout << getname() << " Closed Read Socket OK. " << endl;
    else
        cout << getname() << " Error Closing Read Socket" << endl;

    if( 0 == close(WriteSocket))
	cout << getname() << " Closed Write Socket OK. " << endl;
    else
	cout << getname() << " Error closing Write Socket" << endl;

    flush(cout);
}

void DBCommLinkObject::initDBCommLink()
{
    ReadSocket = -1;
    WriteSocket = -1;
    // ***********************************************
    // Initialize Signals
    // ***********************************************
    // Set up to terminate on interrupt
    if(signal(SIGINT, SIG_IGN) != SIG_IGN)
	signal(SIGINT,terminate);
}

void terminate(int)
{
    //Normal termination
    cout << "terminate()\n";
    exit(0);
}

void DBCommLinkObject::Connect()
{
    extern      InterProcSMObject        IPO;
    int         status = 0;

    while(0 >= (status = connectEOFToHost()))
    {
        cout << getname() 
	     << "DBCommLinkObject::Connect() ERROR from connectEOFToHost = "
	     << dec << status << " Retrying to connect.\n";
    }

    if(0 > (WriteSocket = dup(ReadSocket)) )
    {
        cerr << "***** ERROR: " << getname() << "DBCommLinkObject::Connect(): Can not dup Socket: "
	     << "Error = " << dec << errno << endl;
        cerr << "***** EXITING DBCommLink *****\n";
	exit(1);
    }
}

int DBCommLinkObject::u_ignore_sigpipe()
{
    struct sigaction act;

    if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
    {
        cout << getname()
	     << "DBCommLinkObject::u_ignore_sigpipe() Cannot check the action of "
	     << "the SIGPIPE signal. Return 1, i.e., error.\n";

	return 1; 
    }

    if (act.sa_handler == SIG_DFL)
    {
        act.sa_handler = SIG_IGN;

	if(sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
	{
            cout << getname()
		 << "DBCommLinkObject::u_ignore_sigpipe() Cannot change the action of "
		 << "the SIGPIPE signal. Return 1, i.e., error.\n";

            return 1;
	}
    }

    cout << getname()
	 << "DBCommLinkObject::u_ignore_sigpipe() Success in setting to ignore "
	 << "the SIGPIPE signal. Return 0\n";

    return 0;
}

int DBCommLinkObject::u_connect(u_port_t port, char * hostName)
{
    struct sockaddr_in    server;
    
    int mySock;
    int retval;
    int retryCount;
    int broadcast = 1;
    int temp;

    // If the sigpipe signal can't be ignored, or if host information
    // cannot be gotten, or if there is an error in getting a descriptor for the socket,
    // return an error indication.

    // NOTE: The socket call specifies the Protocol Family, i.e., PF_INET
    // while the Address Family, i.e., AF_INET gets put into the sockaddr_in structure.

    if (0 != u_ignore_sigpipe())
    {
        cerr << "***** ERROR: " << getname()
	     << "DBCommLinkObject::u_connect() : trouble ignoring "
	     << "SIGPIPE signal" << endl;
        flush(cerr);
        return(-1);
    }

    cout << "################################################\n"
	 << "Trying Create UDP Socket !!!!!\n"
	 << "################################################" << endl;

    if( 0 > (mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))
    {
        cerr << "***** ERROR : " << getname()
	     << "DBCommLinkObject::u_connect() : trouble getting "
	     << "socket\n";
	flush(cerr);
	return(-1);
    }
    cout << "\n################################################\n"
	 << "UDP Socket Creating is SUCCESS !!!!!\n"
	 << "u_connect Socket ==== " << mySock << "\n"
	 << "################################################" << endl;

    // Set socket for non-blocking I/O operations
    fcntl(mySock, F_SETFL, O_NONBLOCK);

    // Set socket option.
    setsockopt(mySock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    // Copy Host information into structure for socket connection.
    memset((char *)&server.sin_addr, 0, sizeof(struct sockaddr_in));

    // for portability, convert from host to network byte order.
    server.sin_port = htons(port);

    // set address family to be used to interpret addresses in later
    // operations to the DARPA Internet addresses.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(0 > (temp = bind(mySock, (struct sockaddr *)&server, sizeof(server))))
    {
        cerr << "***** ERROR : " << getname()
	     << "DBCommLinkObject::u_connect() : trouble getting "
	     << "Binding\n";

        flush(cerr);
	return(-1);
    }
   
    cout << "\n##################################################\n"
	 << "Binding Socket is SUCCESS !!!!!!\n"
	 << "###################################################" << endl;

    cout << getname()
	 << "DBCommLinkObject::u_connect() Trying to connect to host : "
	 << hostName << " Socket = " << dec << mySock << endl;

    return (mySock);
} //end u_connect

ssize_t DBCommLinkObject::u_read(int fd, unsigned char *buf, size_t size)
{
    extern DBCommLinkObject      DBCommLink;

    int retval = 0, datacnt = 0, dataflag = 0, waitcnt = 0;
    
    fd_set reads;
    struct timeval timeout;
    int result;
    char * sendUse;

    FD_ZERO(&reads);
    FD_SET(fd, &reads);
    
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;

    result = select(fd+1, &reads, 0, 0, &timeout);

    if( result == -1 )
    {
        cout << getname() << "  select ERROR1" << endl;
	flush(cout);
	return -1;
    }
    else if( result == 0 )
    {
        cout << getname() << "  select TIMEOUT" << endl;
	flush(cout);
	return -1;
    }
    else if( !FD_ISSET(fd, &reads) )
    {
        cout << getname() << "  select ERROR2" << endl;
	flush(cout);
	return -1;
    }

    //Start Data Receive
    struct sockaddr_in sockaddr_from;
    int nleft, toberead, rcvBytes, slen;

    memset(&sockaddr_from, 0, sizeof(struct sockaddr_in));
    slen = sizeof(sockaddr_from);

    nleft = size;
 
    toberead = (nleft > SOCK_BUFF_SIZE ? SOCK_BUFF_SIZE : nleft);

    if((rcvBytes = recvfrom(ReadSocket, buf, toberead, 0, 
			        (struct sockaddr *)&sockaddr_from, &slen)) < 0)
    {
        cout << " recvfrom ERROR ********************" << endl; 
    }
    else // forward packet to EOF Client
    {
        sendUse = DBCommLink.getsendUse();

	if(!strcmp(sendUse, "1"))
	{
	    sendto(ReadSocket, buf, toberead, 0, (struct sockaddr *)&sockaddr_from, slen);
        }
	return rcvBytes;
    }
}

int DBCommLinkObject::connectEOFToHost()
{
    extern InterProcSMObject   IPO;  // Inter Process shared memory
    extern DBCommLinkObject    DBCommLink; 

    u_port_t      myPort;      // connection port number
    int           error;       // error flag
    int           retryCount;  // retry times
    ssize_t       bytesread;   // bytes received from host
    char *        hostName;    // local copy of host this DBCommLink is connected to
    char          portNo[MAX_PORTNO_LEN];      
    char          wsname[MAX_HOSTNAME_LEN+1]; // name of this workstation
    char          wsname_ptr;
    int           readCount;
    time_t        currentTime;
      
    // init some shared memory variables
    hostName = DBCommLink.gethostName();
   

    // added PortNo in DB_WSPCS.cfg
    // Return char *
    strcpy(portNo, DBCommLink.getportNo());

    myPort = atoi(portNo); 

    // initialize to no error
    error = TRUE;
    
    while(error)
    {
        while( !(DBCommLink.isconnected()) )
        {  
	    cout << getname() << " DBCommLinkObject::connectEOFToHost() "
	         << "Trying to connect to " << hostName << endl;
 
	    for( retryCount = 0; retryCount < HOST_TRY_LIMIT; retryCount++ )
	    {
                // Try to connect and get a valid file descriptor for the socket
	        if( (ReadSocket = u_connect(myPort, hostName)) < 0)
	        {
                    if( retryCount < HOST_TRY_LIMIT-1)
		    {

		    }
	        }
	        else // UDP Connect Success 
                {
                    DBCommLink.setconnected();
		    cout << getname() << "DBCommLinkObject::connectEOFToHost(): Successfully "
		    << "connected to " << "socket = " << dec << ReadSocket << endl;

                    break;
	        }
	    } // endfor
	    if(!(DBCommLink.isconnected()) )
	    {
                cerr << getname() << "***** ERROR DBCommLinkObject::connectEOFToHost() "
			      << "Unable to connect to " << hostName << endl;

                cerr << "   Waiting " << dec << HOST_CONNECT_WAIT
		     << " seconds before trying to connect again" << endl;

                flush(cerr);
	        sleep(HOST_CONNECT_WAIT);
	    } // endif
        } //endwhile !connected
    
        int i, setflag = 0;
        memset(wsname, 0x00, sizeof(wsname));
        gethostname(wsname, sizeof(wsname));

        for( i = 0; i < sizeof(wsname); i++ )
        {
            if( 0 == isalnum(wsname[i])) setflag = 1;
	    if( 1 == setflag) wsname[i] = ' ';
        }
        wsname[12] = 0;

        error = FALSE;
        break;
    } //endwhile error

    // SUCCESS
    cout << getname() 
	 << " DBCommLinkObject::connectEOFToHost(): Connection to host"
	 << " successfully\n";
    cout << " UDP Connection Network Established with socket " << dec << ReadSocket << endl;
    cout << " Connection has been made to host " << hostName << endl;
    
    flush(cout);

    return (ReadSocket);
}

void DBCommLinkObject::mainLoop()
{
    while(1)
    {
	// parseEOF receives entire DB Download Data from the Host
	// and Delete & Create DB Image File.
        if(false == parseEOF());
    }
}

bool DBCommLinkObject::parseEOF()
{
    unsigned char rcvBuffer[4096];
    memset(rcvBuffer, 0x00, sizeof(rcvBuffer));

    int len = 4096, rcvlen = 0;

    // read from the Concurrent system
    rcvlen = u_read(ReadSocket, rcvBuffer, len);

    if(rcvlen <= 0)
    {
        cout << " parseEOF in u_read() TIMEOUT!!!" << endl;
        return false;
    }
    else
    {
        cout << ">>>> Receive Packet from PMS <<<<" << endl;
    }
    parseTagID(rcvBuffer, rcvlen);   

    return true;
}

void DBCommLinkObject::parseTagID(unsigned char *buf, int buflen)
{
    int nHeaderLen = 2+2+2+2; // 2 : TT, 2 : Total Idx, 2 : Start Idx, 2 : End Idx   
    int nBufLenCount = nHeaderLen;

    char * portNo;
    int i = 0;
    int fdes;

    extern DBCommLinkObject     DBCommLink; 

    // Common Analog & Digital
    unsigned char picassoId[4];
    unsigned char pid[12], desc[36], sig_type[4], pipbitmap[4];
    unsigned char tagID[12] = {0,};

    // Analog Only
    unsigned char disfmt[8], eng_units[16], met_units[16], erange_lo[4], erange_hi[4], mfactor[4];
    unsigned char moffset[4], hhval[4], hval[4], lval[4], llval[4], hhmet[4], hmet[4], lmet[4]; 
    unsigned char llmet[4], lIPN[2], hIPN[2]; 
    
    // Digital Only 
    unsigned char message1[12], message2[12]; 
    
    // Common Pisa
    unsigned char pisabitmap[4]; 
    char newsta[4], value[4];
    
    char szTemp[2];
    
    char szPort[MAX_PORTNO_LEN] = {0,};
    char szDBFileName[20] = {0,};
    char szRcvTagID[20] = {0,}; 
    portNo = DBCommLink.getportNo();

    if( !strcmp( portNo, "3333" ))       sprintf(szDBFileName, "%s", "dbimage_3.bin" );
    else if( !strcmp( portNo, "4444" ))  sprintf(szDBFileName, "%s", "dbimage_4.bin" );
    else if( !strcmp( portNo, "5555" ))  sprintf(szDBFileName, "%s", "dbimage_5.bin" );
    else if( !strcmp( portNo, "6666" ))  sprintf(szDBFileName, "%s", "dbimage_6.bin" );
    else                                 sprintf(szDBFileName, "%s", "dbimage_3.bin" );

    sprintf(szRcvTagID, "%s", "RCV_TAGID.txt");

    char szTotIdx[5] = {0,};
    char szCurStartIdx[5] = {0,};
    char szCurEndIdx[5] = {0,};

    sprintf((char*)szTotIdx, "%02x%02x", buf[2], buf[3]);
    sprintf((char*)szCurStartIdx, "%02x%02x", buf[4], buf[5]);
    sprintf((char*)szCurEndIdx, "%02x%02x", buf[6], buf[7]);

    int nTotalIdx = 0;
    int nCurStartIdx = 0;
    int nCurEndIdx = 0;
    int nSigType = 0;

    sscanf(szTotIdx, "%x", &nTotalIdx);
    sscanf(szCurStartIdx, "%x", &nCurStartIdx);
    sscanf(szCurEndIdx, "%x", &nCurEndIdx);

    if(strncmp((char*)buf, "TT", 2) == 0 ) 
    {
        if( nCurStartIdx == 1 )	
        {
            if( -1 == unlink( szDBFileName )) // Fail to delete DB File 
            {
               if( ENOENT != errno )
	       {
                   cout << "Error Deleting " << szDBFileName << ". errno = " << errno << endl;
	           cout << " Recommand manually deleting " << szDBFileName << endl;
	       }
	       else 
	           cout << " " << szDBFileName << " does not exist." << endl;
            }
	    else cout << szDBFileName << " DB is successfully deleted." << endl;

            if( -1 == unlink( szRcvTagID )) // Fail to delete RCV_TAGID.txt file
	    {
                if( ENOENT != errno )
		{
                   cout << "Error Deleting " << szDBFileName << ". errno = " << errno << endl;
	           cout << " Recommand manually deleting " << szRcvTagID << endl;
		}
		else
	           cout << " " << szRcvTagID << " does not exist." << endl;
	    }
	    else cout << szRcvTagID << " File is successfully deleted." << endl;

	    eventFile.open(szDBFileName, ios::binary | ios::app );
            
	    tagIDFile.open(szRcvTagID, ios::app);
            chmod("RCV_TAGID", 0777);

	    // Retry DB File Open
            if( !eventFile.is_open()) eventFile.open(szDBFileName, ios::binary | ios::app );

	    // Retry RCV_TAGID.txt file open
	    if( !tagIDFile.is_open()) tagIDFile.open(szRcvTagID);
        }

	for( i=nCurStartIdx; i<=nCurEndIdx; i++ ) // 8 PID per 1 Packet 
        {
            // PICASSO ID
	    memset(picassoId, 0x00, sizeof(picassoId));
            eventFile.write((char*)picassoId, sizeof(picassoId));

	    // PID
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        memset(pid, 0x00, sizeof(pid));
		strncpy((char*)pid, (char*)(buf + nBufLenCount), 8); // PID
	    }	
	    else memset(pid, 0x00, sizeof(pid));
	    
	    eventFile.write((char*)pid, sizeof(pid)); 
	    nBufLenCount += 8;

	    // SIG TYPE
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                //strncpy((char*)sig_type, (char*)(buf + nBufLenCount), 2); // SIG_TYPE
                //sprintf((char*)sig_type, "%x%x", buf[nBufLenCount], buf[nBufLenCount+1]);
                sig_type[0] = 0x00;
		sig_type[1] = 0x00;
		sig_type[2] = buf[nBufLenCount];
		sig_type[3] = buf[nBufLenCount+1];
	        //sig_type[4] = 0x00;

                char szTemp[4] = {0,};
		sprintf((char*)szTemp, "%x%x", buf[nBufLenCount], buf[nBufLenCount+1]);

                //cout << "sig_type : " << szTemp << endl; 
		
		//sscanf((char*)sig_type, "%x", &nSigType);
                sscanf((char*)szTemp, "%x", &nSigType);

		//cout << "nSigType : " << nSigType << endl;
	        if(nSigType >= 10) tagIDFile << pid << ":" << "D" << endl;      
	        else               tagIDFile << pid << ":" << "A" << endl;
	    }
            else if((buf+nBufLenCount) == 0x00)
	    {
		memset(sig_type, 0x00, sizeof(sig_type));
	    }
	    nBufLenCount += 2;

	    // DESC
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                strncpy((char*)desc, (char*)buf + nBufLenCount, 36); // DESCRIPTION
            }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
		memset(desc, 0x00, sizeof(desc)); 
	    }
	    eventFile.write((char*)desc, sizeof(desc)); 
	    nBufLenCount += 36;
           
            // TAGID
            memset(tagID, 0x00, sizeof(tagID));
	    eventFile.write((char*)tagID, sizeof(tagID));

	    // PIP BITMAP
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            { 
	        strncpy((char*)pipbitmap, (char*)(buf + nBufLenCount), 4); // PIPBITMAP
            }
            else if( (buf+nBufLenCount) == 0x00)
	    {
		memset(pipbitmap, 0x00, sizeof(pipbitmap));
	    }
	    eventFile.write((char*)pipbitmap, sizeof(pipbitmap));
	    nBufLenCount += 4;

	    eventFile.write((char*)sig_type, sizeof(sig_type)); 

	    // DISPLAY FORMAT    
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        char szParse[8] = {0,};
	        strncpy((char*)szParse, (char*)(buf + nBufLenCount), 8);

	        int nDisFmt = atoi(szParse);
	        if(nDisFmt == 0) sprintf((char*)disfmt, "%s", "%6.0f");
	        else if(nDisFmt == 1) sprintf((char*)disfmt, "%s", "%6.1f");
       	        else if(nDisFmt == 2) sprintf((char*)disfmt, "%s", "%6.2f");
	        else if(nDisFmt == 3) sprintf((char*)disfmt, "%s", "%6.3f");
	        else if(nDisFmt == 4) sprintf((char*)disfmt, "%s", "%6.4f");
	        else if(nDisFmt == 5) sprintf((char*)disfmt, "%s", "%8.1e");
	        else sprintf((char*)disfmt, "%s", "?????"); 
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(disfmt, 0x00, sizeof(disfmt));
	    }
	    eventFile.write((char*)disfmt, sizeof(disfmt));
	    nBufLenCount += 8;
            
	    // ENGLISH UNITS
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)eng_units, (char*)buf + nBufLenCount, 16);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {  
	        memset(eng_units, 0x00, sizeof(eng_units));
            }	
	    eventFile.write((char*)eng_units, sizeof(eng_units));
	    nBufLenCount += 16;

	    // METRIC UNITS
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            { 
	        strncpy((char*)met_units, (char*)(buf + nBufLenCount), 16);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(met_units, 0x00, sizeof(met_units));
	    }
	    eventFile.write((char*)met_units, sizeof(met_units));
	    nBufLenCount += 16;

	    // ENGINEERING RANGE LO LIMIT
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)erange_lo, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(erange_lo, 0x00, sizeof(erange_lo));
            }
	    eventFile.write((char*)erange_lo, sizeof(erange_lo));
	    nBufLenCount += 4;
            
	    // ENGINEERING RANGE HI LIMIT
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)erange_hi, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00) 
	    {
	        memset(erange_hi, 0x00, sizeof(erange_hi)); 
            }	
	    eventFile.write((char*)erange_hi, sizeof(erange_hi));
	    nBufLenCount += 4;
	    
	    // M-FACTOR ( METRIC -> ENGLISH CONVERSION FACTOR )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)mfactor, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(mfactor, 0x00, sizeof(mfactor));
            }	
   	    eventFile.write((char*)mfactor, sizeof(mfactor));
            nBufLenCount += 4;
            
	    // M_OFFSET ( METRIC -> ENGLISH CONVERSION OFFSET ) 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)moffset, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(moffset, 0x00, sizeof(moffset));
            }
	    eventFile.write((char*)moffset, sizeof(moffset));
	    nBufLenCount += 4;
            
	    // HI HI LIMIT ENGLISH VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)hhval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(hhval, 0x00, sizeof(hhval));
	    }
	    eventFile.write((char*)hhval, sizeof(hhval));
	    nBufLenCount += 4;
            
	    // HI LIMIT ENGLISH VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00) 
	    {
	        memset(hval, 0x00, sizeof(hval));
            }	
	    eventFile.write((char*)hval, sizeof(hval));
	    nBufLenCount += 4;
            
	    // LO LIMIT ENGLISH VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)lval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(lval, 0x00, sizeof(lval));
	    }
	    eventFile.write((char*)lval, sizeof(lval));
	    nBufLenCount += 4;
           
	    // LO LO LIMIT METRIC VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)llval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(llval, 0x00, sizeof(llval));
            }
	    eventFile.write((char*)llval, sizeof(llval));
	    nBufLenCount += 4;
           
	    // HI HI LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hhmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(hhmet, 0x00, sizeof(hhmet));
            }	
	    eventFile.write((char*)hhmet, sizeof(hhmet));
	    nBufLenCount += 4;
            
	    // HI LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(hmet, 0x00, sizeof(hmet));
            }
	    eventFile.write((char*)hmet, sizeof(hmet));
	    nBufLenCount += 4;
            
	    // LO LIMIT METRIC VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)lmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(lmet, 0x00, sizeof(lmet));
            }	
	    eventFile.write((char*)lmet, sizeof(lmet));
	    nBufLenCount += 4;
            
	    // LO LO LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)llmet, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(llmet, 0x00, sizeof(llmet));
            }	
	    eventFile.write((char*)llmet, sizeof(llmet));
	    nBufLenCount += 4;
            
	    // L-IPN ( NON-ZERO = IPN OF VARIABLE SETPOINT )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)lIPN, (char*)(buf + nBufLenCount), 2);
            }	
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(lIPN, 0x00, sizeof(lIPN));
	    }
	    eventFile.write((char*)lIPN, sizeof(lIPN));
	    nBufLenCount += 2;
            
	    // H-IPN ( NON-ZERO = IPN OF VARIABLE SETPOINT )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hIPN, (char*)(buf + nBufLenCount), 2);
            }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
                memset(hIPN, 0x00, sizeof(hIPN));
            }
	    eventFile.write((char*)hIPN, sizeof(hIPN));
	    nBufLenCount += 2;
	  
	    // MESSAGE 1
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
                strncpy((char*)message1, (char*)(buf + nBufLenCount), 8); // message1
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
                memset(message1, 0x00, sizeof(message1)); 
	    }
	    eventFile.write((char*)message1, sizeof(message1));
	    nBufLenCount += 8;

	    // MESSAGE 2
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                strncpy((char*)message2, (char*)(buf + nBufLenCount), 8); // message2
            }
            else if( (buf+nBufLenCount) == 0x00)
            {	
                memset(message2, 0x00, sizeof(message2));
            }
	    eventFile.write((char*)message2, sizeof(message2));
            nBufLenCount += 8;
            
	    // PISA BITMAP
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)pisabitmap, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(pisabitmap, 0x00, sizeof(pisabitmap));
	    }
	    eventFile.write((char*)pisabitmap, sizeof(pisabitmap));
	    nBufLenCount += 4;
	
	    // Do not receive STATUS & VALUE packet. But must fit DB file format 
	    memset(newsta, 0x00, sizeof(newsta));
	    eventFile.write(newsta, sizeof(newsta));
	   
	    memset(value, 0x00, sizeof(value));
	    eventFile.write(value, sizeof(value));
            
	    // Check whether if 2Bytes of End is EE or NOT.
	    // If Yes, close DB image file.
	    if( (nCurEndIdx == nTotalIdx) && (buf[nBufLenCount] == 0x45) && (buf[nBufLenCount+1] == 0x45)) 
            {
                eventFile.close();
		cout << "DB( " << szDBFileName << " ) is successfully closed." << endl;
		
		tagIDFile.close();
		cout << szRcvTagID << " File is successfully closed." << endl;


		// ujson add 20210303
		sleep(1);
		cout << " System restart 5......" << endl;
		sleep(1);
		cout << " System restart 4......" << endl;
		sleep(1);
		cout << " System restart 3......" << endl;
		sleep(1);
		cout << " System restart 2......" << endl;
		sleep(1);
		cout << " System restart 1......" << endl;
		sleep(1);

		exit(-1);
	    }// end of loop
	} // end of packet-1
    }
    return;
}
// end of parseTagID

void swap32(char * pval)
{
    char tmp;
    tmp = *(pval+3);
    *(pval+3) = *(pval);
    *(pval) = tmp;
    tmp = *(pval+2);
    *(pval+2) = *(pval+1);
    *(pval+1) = tmp;
}

