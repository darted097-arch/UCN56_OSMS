//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// CommLinkObject.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module is the implementation of the CommLink Class defined 
//  in CommLink.h.
//
//  The CommLink task creates a shared segment for the display database 
//  & fills it with data obtained dynamically from the Concurrent host software.
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
//#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// corenet ujson 2017.0106
#include <sys/time.h>
//#include <time.h>			// time calls
//#include <sys/uio.h>

#include <stdlib.h>			// exit
#include <iostream.h>			// io calls
#include <fstream.h>
#include <strings.h>
#include <unistd.h>			// close
#include <sys/stat.h>			// chmod

#include <sys/socket.h>			// socket info
#include <signal.h>

#ifdef LINUX_APP
#include <errno.h>	// for Linux (2012.07.05. CORENET )
#include <sys/types.h>	// for Linux (2012.07.05. CORENET )
#include <fcntl.h>	// for Linux (2012.07.05. CORENET )
#endif
//
// User Defined Include Files:
//
#include "Typedefs.h"
#include "interface.h"
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "database.h"			// database structure
#include "messageParameters.h"		// message parameters
#include "messageQueueObject.h"
#include "sharedMemoryObject.h"
#include "CommLinkObject.h"
#include "DatabaseObject.h"
#include "messageObject.h"
#include "InterProcSMObject.h"

#include "DatabaseObject.h"				// by feel9 

// ucn56
#include "ucn56Parameters.h"		// generic system parameters
#include "ucn56Dynamic.h"

//#define DEBUG
//
// External Variables:
//
extern	messageQueueObject	WS_MsgQue;

//
// Local Constant Definitions:
//
//
// Function Prototypes:
//
#include "prototypes.h"
void terminate(int);
//
// ***************************************************************************
//

_Local CommLinkObject::Local;

int   CommLinkObject::bytes_read;
int   CommLinkObject::total_bytes_read;

int   CommLinkObject::msgSize;
int   CommLinkObject::gotMsgSize;
int   CommLinkObject::bytesToRead;

time_t CommLinkObject::lastMessageTime; 
int   CommLinkObject::ReadSocket;     
int   CommLinkObject::WriteSocket;   

int   CommLinkObject::num;          
char  CommLinkObject::name[MAX_COMMLINK_NAME_LEN]; 
char  CommLinkObject::hostName[MAX_HOSTNAME_LEN]; 
char  CommLinkObject::portNum[MAX_PORTNO_LEN];
int   CommLinkObject::connected;      
time_t CommLinkObject::timeOfLastRead;  
char  CommLinkObject::sendUse[5];
int  CommLinkObject::isDbDown;

//by feel9  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
extern  InterProcSMObject       IPO;    // Inter Process Shared Memory
extern  DatabaseObject  DBO;		// for Update Database
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct sockaddr_in sockaddr_to;

// ucn56 EOF
ErdsMessageHeader	MessageHeader;
ErdsDbDownloadReply	dbDownloadReply;
ErdsDbDynamicDataReply  dbDyDataReply;
ErdsFileDataReply	fileDataReply;
DATA  gData;

/*----------------------------*/ //2015.01.22
uint32 tmp1, tmp2;
/*----------------------------*/ //2015.01.22


#define SOCK_BUFF_SIZE 65535000

#define SEND_SIZE 20000


CommLinkObject::CommLinkObject()
{
   cout << "CommLinkObject Constructor:" << endl;
   initCommLink();
}

CommLinkObject::~CommLinkObject()
{
   cout << "CommLinkObject Destructor:" << endl;
// close sockets used to read and write data 
   if (0 == close(ReadSocket))
     cout << getname()
	  << " Closed Read Socket OK. " << endl;
   else
     cout << getname()
	  << " Error closing Read Socket" << endl;
   if (0 == close(WriteSocket))
     cout << getname()
	  << " Closed Write Socket OK. " << endl;
   else
     cout << getname()
	  << " Error closing Write Socket" << endl;
   flush(cout);
}

void CommLinkObject::initCommLink()
{
   // init socket numbers
   ReadSocket = -1;
   WriteSocket = -1;
   isDbDown = 0;

   // ********************************************************************************
   // Initialize Signals
   // ********************************************************************************
   // Set up to terminate on interrupt
   if(signal(SIGINT,SIG_IGN) != SIG_IGN)  
      signal(SIGINT,terminate); 
} //end initCommLink()


void CommLinkObject::ws2ccurlink()
{

   extern InterProcSMObject	IPO;
   extern INTERFACE *	 Interface;	// shared memory GUI variables

   // returned message length
   int	messageLength;
  
   // w/s to ccur message
   unsigned int	ccurMessage[(WS_MESSAGE_LENGTH+sizeof(int)-1)/sizeof(int)];
  
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	short opcode = 0;
	WSmessageHeader	*msgheader = (WSmessageHeader *)ccurMessage;
#ifdef LINUX_APP
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//   cout << "STEP 1 -----------------------" << endl;

   // only pull messages off the queue if this is the primary CommLink
   if (1 != IPO.getprimaryStatus(getnum()) )
   {
#ifdef DEBUG
      cout << "   " << getname()
           << "CommLinkObject::ws2ccur() not pulling messages "
           << "from WS_MsgQue Object.  This is not the Primary CommLink."
           << endl;
#endif
      return;
   }


//   cout << "STEP 2 -----------------------" << endl;

   // read the message off the unix message queue into a buffer and note
   //its length.
   messageLength = WS_MsgQue.receiveMessage(&ccurMessage, NOWAIT, 
					     WORKSTATION_MSG);

//   cout << "STEP 3 ----------------------- [%d]" << messageLength << endl;
   if (messageLength != -1)
   {
#ifdef DEBUG
      cout << getname()
	   << "CommLinkObject::ws2ccur() received " << dec << messageLength 
	   << " byte message from WS_MsgQue Object\n";
      cout << "[00]:" << hex << ccurMessage[0] << " "
	   << "[04]:" << ccurMessage[1] << " "
	   << "[08]:" << ccurMessage[2] << " "
	   << "[12]:" << ccurMessage[3] << " "
	   << "[16]:" << ccurMessage[4] << " " 
           << "[20]:" << ccurMessage[5] << " " << endl;
      cout << "[24]:" << ccurMessage[6] << " "
	   << "[28]:" << ccurMessage[7] << " "
	   << "[32]:" << ccurMessage[8] << " "
	   << "[36]:" << ccurMessage[9] << " " << endl;
      flush(cout);

// yrlee for DEBUG 20061210
/*

      for (int iii=0; iii < messageLength/4; iii++)
      {
	  cout << dec << iii << ":" << hex <<ccurMessage[iii] << endl;
	  if (iii==5) cout << "------------" << endl;
	  if (iii==78) cout << "------------" << endl;
      }
*/

#endif

  
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	//cout << "SEND mspb = [" << hex << msgheader->mspbStatus << "]" << endl;
	msgheader->opcode = htons(msgheader->opcode); 
	msgheader->numberOfSomething = htons(msgheader->numberOfSomething); 
	msgheader->mspbStatus = htons(msgheader->mspbStatus); 
	msgheader->disabilityIndex = htonl(msgheader->disabilityIndex); 
//msgheader->messageSize = htons(msgheader->messageSize); 
#endif
	opcode = msgheader->opcode; 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//   cout << "STEP 4 ----------------------- " << endl;
      // Do not send messages if the WS is not ready, i.e., all inits have
      //been done.
      if(READY_WS_STATE != (Interface->System.wsState & READY_WS_STATE))
      {
	 if (DBDEF_NEW_PB_WS_STATE != Interface->System.wsState)
	 {
            cout << "   " << getname()
	         << "/n***** CommLinkObject::ws2ccur() not sending message "
                 << "from WS_MsgQue Object.  The WS is not in the READY state."
	         << endl << endl;
	    return;
         }
      }

//   cout << "STEP 5 ----------------------- " << endl;
      // put the message into a message object for easier access
      messageObject	CCCmsg(TO_HOST_MSG_ID, "CCCmsg");
      int	msgSize;
      CCCmsg.setMessage(&ccurMessage, messageLength);
      msgSize = CCCmsg.getMsgSize();

      // check for a size match
      if(msgSize != messageLength)
      {
	 cout << endl << getname()
	      << "***** CommLinkObject::ws2ccurlink() - WARNING *****\n";
	 cout << "  Length of message received from the unix message queue "
	      << "does not match the msgSize stored in the message." << endl
	      << "    Opcode, msgSize, length received = "
	      << hex << CCCmsg.getOpcode() << dec << " " << CCCmsg.getMsgSize()
	      << " " << messageLength << endl
              << "      The size stored in the message will be used!" << endl;
      }
 
      if (TO_HOST_MSG_LEN < msgSize)
      {
         msgSize = TO_HOST_MSG_LEN;
	 cout << "\n" << getname() 
	      << "***** CommLinkObject::ws2ccurlink() - WARNING *****\n";
         cout << "  Attempting to send a message to the server larger than "
	      << dec << TO_HOST_MSG_LEN << " bytes\n";
         cout << "*****  Only " << TO_HOST_MSG_LEN << " bytes will be sent\n";
      }

//   cout << "STEP 6 ----------------------- " << endl;
  
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	msgheader->messageSize = htons(msgheader->messageSize); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      // send the message out to the host
      // Only 1K messages can be accepted by the Concurrent server
// by feel9
      //if (write(WriteSocket, ccurMessage, msgSize) < 0)
      //   cout << "Error sending message to CCUR" << endl;
//#ifdef DEBEG
      //else
         printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SND(to CCUR)... opcode [%04x]\n", opcode);
         cout << "Successfully sent " << dec << msgSize 
             << " message to socket " << hex << WriteSocket 
	    << " message: " << ccurMessage << endl;
//#endif
   }
   flush(cout);

}


void terminate(int)  
{
// Normal termination
   cout << "terminate()\n";
   exit(0);
}

// by feel9 /////////////////////
//Global Values//////////////////////////
#define MAXDATA 35840								
//#define MAXDATA 40000								
//char rcvTotalData[MAXDATA] = {0,};
//char rcvOldTotalData[MAXDATA] = {0,};
char rcvTotalData[MAXDATA];
char rcvOldTotalData[MAXDATA];
int totalCopyData = 0;
int maxDataRcv = 0;
int curDataRcv = 0;
int rcvTagIDOK = 0;
int rcvTagIDNum = 0;
//int totalrcvTagIDNum = 6904;			//in CommLinkObject.h
bool rcvDataStart = false;
bool file_save_need = false;
bool bDataCopyFirst = false;
map<string, string> AllMap;
map<string, string> ::iterator map_Pos;
///////////////////////////////////////////////

void CommLinkObject::mainLoop()
{
// ucn56 delete 
/*
   ifstream infile;

   //RCV_TAGID.txt File Check!
   infile.open("RCV_TAGID.txt", ifstream::nocreate);

   if(!infile.is_open())
   {
  	rcvTagIDOK = 0;
  	rcvTagIDNum = 0;
   }
   else
   {
  	rcvTagIDNum = totalrcvTagIDNum;
  	rcvTagIDOK = 2;
   }
	 
   infile.close();
  
   maxDataRcv = totalrcvTagIDNum / 100;
   memset(rcvOldTotalData, 0x00, MAXDATA);
*/  
   //for receive Broadcast Data
   while(1)
   {
	//usleep(5000);
	if(false == CommFunc());

   }//End of while
    
}

// ucn56 EOF
//
// !!!!!!! ucn56 Database Update
void CommLinkObject::ucn56DBUpdate()
{
   extern CommLinkObject      CommLink;

   int nread, slen;
   char * sendUse;
   U56_RECORD *p_record;
   float   analogValue;
   int	   digitQuality;
   int     i, ipnIndex;
   int 	   sendLen = 0;

   char ptr[250000] = {0x00, };
   //char *ptr;

   ipnIndex = 0;
   nread = 0;

   slen = sizeof(sockaddr_to);
   sendUse = CommLink.getsendUse();

   ptr[0] = 'S';
   ptr[1] = 'S';
   nread = 2;

//   cout << getname() << " CommLinkObject::ucn56DBUpdate START!!!" << endl;

   // all point scan
   for (i = 0; i < NALL ; i++)
   {
      p_record = &(gData.DbPoint[i]);

      if (p_record->pid[0] == ' ' || p_record->pid[0] == 0)
         continue;

      ipnIndex = find_pid(p_record->pid);

//      cout << getname() << " p_record->pid = " << p_record->pid 
//	   << " ipnIndex = " << ipnIndex << endl;

      if (ipnIndex < 0)
	 continue;
 
      if (p_record->sig_type >= 16) // digital 
      {
	 if (p_record->newsta >=0 && p_record->newsta <7)
	    p_record->newsta = 0;
        
//	 cout << "p_record->curstt = " << p_record->curstt
//	      << " p_record->newsta = " << p_record->newsta << endl;

	 DBO.setvalue(ipnIndex, p_record->curstt);
	 DBO.setnewsta(ipnIndex, p_record->newsta);

	 memcpy(ptr+nread, p_record->pid, 12);
	 nread += 12;
	 ptr[nread] = 'D';
	 nread += 1;
	 memcpy(ptr+nread, &p_record->curstt, 4);
	 nread += 4;
	 memcpy(ptr+nread, &p_record->newsta, 4);
	 nread += 4;

      }
      else
      {
	 if (p_record->newsta >=0 && p_record->newsta <7)
	     p_record->newsta = 0;

//	 cout << "p_record->curmet = " << p_record->curmet
//	      << " p_record->newsta = " << p_record->newsta << endl;

	 DBO.setvalue(ipnIndex, p_record->curmet);
	 DBO.setnewsta(ipnIndex, p_record->newsta);

	 memcpy(ptr+nread, p_record->pid, 12);
	 nread += 12;
	 ptr[nread] = 'A';
	 nread += 1;
	 memcpy(ptr+nread, &p_record->curmet, 4);
	 nread += 4;
	 memcpy(ptr+nread, &p_record->newsta, 4);
	 nread += 4;
      }
   }


// DEBUG 202203 corenet
#if 0

if (nread > 2)
{
   ptr[nread] = 'E';
   nread += 1;
   ptr[nread] = 'E';
   nread += 1;

   cout << "nread = " << nread << endl;

   char tmpid[13] = {0x00, };
   char isAD[2] = {0x00, };
   char isStart[3] = {0x00, };
   int curstt, newsta;
   float curmet;
   int d_nread = 0;
   int count = 0;

   memcpy(isStart, ptr, 2);
   d_nread += 2;

   if(strncmp(isStart, "SS", 2) == 0) 
   {
       cout << "--------------- START ----------------" << endl;
       for (count = 0; ;count++) 
       {
	   memcpy(isStart, ptr+d_nread, 2);

	   if(strncmp(isStart, "EE", 2) == 0)
	       break;

           memcpy(tmpid, ptr+d_nread, 12);
           d_nread += 12;
           memcpy(isAD, ptr+d_nread, 1);
           d_nread += 1;
           memcpy(&curmet, ptr+d_nread, 4);
           d_nread += 4;
           memcpy(&newsta, ptr+d_nread, 4);
           d_nread += 4;

	   cout << " num = " << count << " pid = " << tmpid << " AD = " << isAD << " Value = " << curmet << " State = " << newsta << endl;
       }
       cout << "--------------- END ----------------" << endl;

   }
}

#else

// corenet 20220321
   // send Use 1
   if (nread > 2) {
       ptr[nread] = 'E';
       nread += 1;
       ptr[nread] = 'E';
       nread += 1;

       if(!strcmp(sendUse, "1"))
       {
	  cout << "WriteSocket = " << WriteSocket
	       << " send data len = " << nread << endl;

          int s_count = 0;
          
	  while(1)
	  { 
   	      if (nread > SEND_SIZE) 
	      {
                  sendLen = sendto(WriteSocket, ptr+(s_count*SEND_SIZE), SEND_SIZE, 0, (struct sockaddr *)&sockaddr_to, slen);
	          nread -= SEND_SIZE;
	          s_count++;
                  //cout << "send 1111111 " << sendLen << endl;
              }
	      else 
	      {
                  sendLen = sendto(WriteSocket, ptr+(s_count*SEND_SIZE), nread, 0, (struct sockaddr *)&sockaddr_to, slen);
                  //cout << "send 2222222 " << sendLen << endl;
	          break;
	      }

	      usleep(500);
	  }

          // sendLen = sendto(WriteSocket, ptr, nread, 0, (struct sockaddr *)&sockaddr_to, slen);
          //cout << "send OK !!!!!!!!!!!!!! " << sendLen << endl;
       }
   }
#endif
}


// !!!!!!  CommFunc
bool CommLinkObject::CommFunc()
{
   char	hostIp[64];
   char	str[256];
   int	No;

//   cout << getname() << " CommLinkObject::CommFunc() START!!" << endl;

   static uint32 _dbupdate_sec[2] = {0};

   int rcvlen;
   unsigned int len = sizeof(ErdsMessageHeader);
   ErdsMessageHeader *pHeader = &(MessageHeader);

   //********* recv HEADER 
   rcvlen = recvData (ReadSocket, pHeader, len);

   if (rcvlen <= 0) {
      // ptrSHM->hostInfo[0].epochSeconds = 0;
      // ptrSHM->hostInfo[1].epochSeconds = 0;
      isDbDown = 0;
      //cout << getname() << " CommLinkObject::CommFunc recvData timeout error rcvlen = " << rcvlen << endl;

      return false;
   }
   else if (rcvlen != len) {
      //ptrSHM->hostInfo[0].epochSeconds = 0;
      //ptrSHM->hostInfo[1].epochSeconds = 0;

      cout << getname() << " CommLinkObject::CommFunc recvData() Header size error rcvlen = " << rcvlen << endl;
      cout << getname() << " CommLinkObject::CommFunc recvData() Header size error len = " << len << endl;

      return false;
   }

   //cout << getname() << " recv HEADER O.K." << endl;

   //********* recv HEADER O.K.
   pHeader->numberOfSomething = ntohs(pHeader->numberOfSomething);
   pHeader->messageLength = ntohl(pHeader->messageLength);
   pHeader->dbRevNo = ntohl(pHeader->dbRevNo);
   pHeader->epochSeconds = ntohl(pHeader->epochSeconds);

   // strcpy(hostIp, ptrSHM->hostInfo[0].hostIP);

   //********* next process
   switch (pHeader->opcode)
   {
      case DBDOWNLOADING :
	 if (processDbDownLoading (pHeader))
	 {
	    cout << getname() << " DOWNLOAD DB COMPLETED" << endl;
	    //ucn56DBUpdate();
	 }
         //usleep(1000);
         usleep(500);
         break;

      case DBDYNDATADOWNLOADING :
	 if (processDynDownLoading (pHeader))
	 {
	    //cout << getname() << " DOWNLOAD DYNAMIC DB COMPLETED" << endl;
	    ucn56DBUpdate();
	 }
         break;

      case DBDOWNLOADHEARTBEAT :
	 //cout << getname() << " RECEIVED HEART BEAT" << endl;
         break;

      case FILEDOWNLOADING :
	 if (processFileDownloading (pHeader))
	 {
	    // cout << getname() << " DOWNLOAD ALM,SOE FILES COMPLETED" << endl;
	    ;
	 }
         break;
      default:
	 // cout << getname() << " opcode error = " << hex << pHeader->opcode << endl;
	 break;

   }

   return true;
}


// !!!! recvData Function
int CommLinkObject::recvData(int fd, void *data, unsigned int len)
{
//   cout << getname() << " CommLinkObject::recvData() START!!" << endl;

   int k;
   struct timeval 	timeout;
   fd_set  readset;
   int m_maxFd = 0, BytesReceived;

   timeout.tv_sec = 15;
   timeout.tv_usec = 0;
   FD_ZERO(&readset);
   FD_SET(fd, &readset);
   m_maxFd = fd;

   while(1) 
   {
      k = select(m_maxFd + 1, &readset, NULL, NULL, &timeout);
//      cout << getname() << " CommLinkObject::recvData select k = " << k << endl;

      if (k == -1) 
      {
	 cout << getname() << " CommLinkObject::recvData select error" << endl;
	 return -1;
      }
      else if (k == 0)
      {
	 return 0;
      }

      if (!FD_ISSET(fd, (fd_set *)&readset)) 
      {
	 cout << getname() << " CommLinkObject::recvData FD_ISSET error" << endl;
	 return -2;
      }

      break;
   }


   BytesReceived = recvSvcData(fd, data, len);

//   cout << getname() << " CommLinkObject::recvData BytesReceived = " << dec << BytesReceived << endl;

   return (BytesReceived);
}


//  !!! recvSvcData Function
int CommLinkObject::recvSvcData (int fd, void *data, unsigned int len)
{
//   cout << getname() << " CommLinkObject::recvSvcData() START!!" << endl;

   extern CommLinkObject      CommLink;

   int nleft, nread, toberead, slen;
   char *ptr;
   char *buf;
   char * sendUse;

   nleft = len;
   ptr = (char *)data;

   // Start Data Receive
   struct sockaddr_in sockaddr_from;
   memset(&sockaddr_from, 0, sizeof(struct sockaddr_in));
   slen = sizeof(sockaddr_from);

   sendUse = CommLink.getsendUse();

   while (nleft > 0 )
   {
      // usleep(10000);
      usleep(1000);
      // usleep(500);

      toberead = (nleft > SOCK_BUFF_SIZE ? SOCK_BUFF_SIZE : nleft);

      nread = recvfrom(ReadSocket, (char *)ptr, toberead, 0, (struct sockaddr *)&sockaddr_from, &slen);

      if (nread <= 0) 
      {

	 if (nleft == len) 
	 {
	    //cout << getname() << " Receive()  nleft == len error" << endl;
	    return -1;
	 }
         else 
         {
	    //cout << getname() << " Receive()  len(" << len << ")nleft(" << nleft << ") error" << endl;
	    return (len-nleft);
         }
      }

// corenet 20220321
/*
      // send Use 1
      if(!strcmp(sendUse, "1"))
      {
         sendto(WriteSocket, ptr, nread, 0, (struct sockaddr *)&sockaddr_to, slen);
//       sendto(WriteSocket, buf, rcvBytes, 0, (struct sockaddr *)&sockaddr_to, slen);            
      }
*/


      nleft -= nread;
      ptr   += nread;
   }

   return (len - nleft);
}



// !!!!! processDbDownLoading Function
bool CommLinkObject::processDbDownLoading (ErdsMessageHeader *pHeader)
{
//   cout << getname() << " CommLinkObject::processDbDownLoading() START!!" << endl;

   int rcvlen;
   char str[256];
   uint32 len = pHeader->messageLength;
   U56_RECORD	*p_data = &(dbDownloadReply.data[0]);
   u_short chksum, recvchksum = 0;

   extern INTERFACE *    Interface;

   cout << "Before processDbDownLoading : "
	<< "len(" << len << ") status(" << pHeader->status 
	<< ")" << endl;

   //********* recv DB Data
   rcvlen = recvData (ReadSocket, p_data, len);

   if (rcvlen <= 0) 
   {
      //cout << getname() << "processDbDownLoading : Receive error" << endl;
      return false;
   }
   else if (rcvlen != len) 
   {
      //cout << getname() << "processDbDownLoading : Data Size error" << endl;
      return false;
   }

   //********* recv CheckSum
   rcvlen = recvData (ReadSocket, &chksum, sizeof(chksum));

   if (rcvlen <= 0) 
   {
      //cout << getname() << "processDbDownLoading : Receive Checksum error" << endl;
      return false;
   }
   else if (rcvlen != sizeof(chksum)) 
   {
      //cout << getname() << "processDbDownLoading : Checksum Size error" << endl;
      return false;
   }

   //********** check ...
   chksum = ntohs(chksum);
   recvchksum = Checksum((u_char*)p_data, len);
   if (chksum != recvchksum) 
   {
      //cout << getname() << "processDbDownLoading : Checksum mismatch!!!" << endl;
      return false;
   }

   //********* DB Data Recv & Checksum OK....

   if (isDbDown == 0) 
   {
      memcpy((char *)&(gData.DbPoint[0]), (char *)p_data, len); // gData save
      allDb_ntohlh(len);	// valuse conversion
      isDbDown = 1;

      if (strcmp(getname(), "CommLink1") == 0)
          Interface->System.cpuNum = 1;
      else Interface->System.cpuNum = 2;
   }

   return true;
}


// !!!! processDynDownLoading Function
bool CommLinkObject::processDynDownLoading (ErdsMessageHeader *pHeader)
{
//   cout << getname() << " CommLinkObject::processDynDownLoading() START!!" << endl;

   int rcvlen;
   char str[256];
   uint32 len = pHeader->messageLength;
   DYNAMIC_RECORD  *p_data = &(dbDyDataReply.data[0]);
   u_short chksum, recvchksum = 0;

   //********* recv Dynamic Data
   rcvlen = recvData (ReadSocket, p_data, len);

   if (rcvlen <= 0) 
   {
      //cout << getname() << " processDynDownLoading : Receive error" << endl;
      return false;
   }
   else if (rcvlen != len) 
   {
      //cout << getname() << " processDynDownLoading : Data Size error" << endl;
      return false;
   }

   //********* recv CheckSum
   rcvlen = recvData (ReadSocket, &chksum, sizeof(chksum));

   if (rcvlen <= 0) 
   {
      //cout << getname() << " processDynDownLoading : Receive Checksum error" << endl;
      return false;
   }
   else if (rcvlen != sizeof(chksum)) 
   {
      //cout << getname() << " processDynDownLoading : Checksum  Size error" << endl;
      return false;
   }

   //********** check ...
   chksum = ntohs(chksum);
   recvchksum = Checksum((u_char*)p_data, len);
   if (chksum != recvchksum) 
   {
      //cout << getname() << " processDynDownLoading : Checksum mismatch!!!" << endl;
      return false;
   }

   //********** DbRev Check .... Not Use

   //********* Dynamic Data Recv & Checksum & DbRev OK.....
   memcpy((char *)&(gData.DynPoint[0]), (char *)p_data, len); // gData save
   dynDb_ntohlh(len);	// valuse 

   UpdateDynamicDb(len); // DATABASE Update

   return true;
}



// !!!! processFileDownloading Function
bool CommLinkObject::processFileDownloading (ErdsMessageHeader *pHeader)
{
//   cout << getname() << " CommLinkObject::processFileDownloading() START!!" << endl;

   int rcvlen;
   char buf[256];
   uint32 len = pHeader->messageLength;
   char	*p_data = fileDataReply.pFileData;
   u_short chksum, recvchksum = 0;

   p_data = new char[len];

   if (p_data == NULL) 
   {
      //cout << getname() << " processFileDownLoading : Memory allocation error" << endl;
      return false;
   }

   //********* rcv file Data
   rcvlen = recvData (ReadSocket, p_data, len);

   if (rcvlen <= 0)
   {
      //cout << getname() << " processFileDownLoading : Receive error" << endl;
      delete [] p_data;
      return false;
   }
   else if (rcvlen != len)
   {
      //cout << getname() << " processFileDownLoading : Data Size error" << endl;
      delete [] p_data;
      return false;
   }

   //********* rcv Checksum
   rcvlen = recvData (ReadSocket, &chksum, sizeof(chksum));

   if (rcvlen <= 0)
   {
      //cout << getname() << " processFileDownLoading : Receive Checksum error" << endl;
      delete [] p_data;
      return false;
   }
   else if (rcvlen != sizeof(chksum))
   {
      //cout << getname() << " processFileDownLoading : Checksum Size error" << endl;
      delete [] p_data;
      return false;
   }

   delete [] p_data;
   p_data = NULL;

   return true;
}



// !!!! Checksum Function
u_short CommLinkObject::Checksum(u_char *packet, int nSize)
{
   u_short checksum = 0;

   for (int i=0; i<nSize; i++)
      checksum += (u_short)packet[i];

   return checksum;
}


// !!!! allDb_ntohlh Function
void CommLinkObject::allDb_ntohlh(unsigned int dbSize)
{
   cout << getname() << " CommLinkObject::allDb_ntohlh() START!!" << endl;

   U56_RECORD *p_record;

   int iteration = dbSize / sizeof(U56_RECORD);

   for (int i = 0; i < iteration; i++)
   {
      p_record = &(gData.DbPoint[i]);

      p_record->sig_type = ntohl(p_record->sig_type);
      p_record->pipbitmap = ntohl(p_record->pipbitmap);
      p_record->dsllvl = ntohs(p_record->dsllvl);

      p_record->ctout1 = ntohs(p_record->ctout1);
      p_record->ctout2 = ntohs(p_record->ctout2);
      p_record->convno = ntohs(p_record->convno);
      p_record->icl = ntohs(p_record->icl);
      p_record->degree = ntohs(p_record->degree);
      p_record->PIPSPARE5 = ntohs(p_record->PIPSPARE5);

      p_record->hivsp = ntohs(p_record->hivsp);
      p_record->lovsp = ntohs(p_record->lovsp);
      p_record->almdly = ntohs(p_record->almdly);
      p_record->stat1 = ntohs(p_record->stat1);
      p_record->altype = ntohs(p_record->altype);
      p_record->chass = ntohs(p_record->chass);
      p_record->kirtp = ntohs(p_record->kirtp);
      p_record->rangeindex = ntohs(p_record->rangeindex);

      p_record->senunit = ntohl(p_record->senunit);

      p_record->bufferIndex = ntohl(p_record->bufferIndex);

      p_record->pdasIndex = ntohs(p_record->pdasIndex);

      p_record->iccmsIndex = ntohs(p_record->iccmsIndex);
      p_record->chassisAddress = ntohs(p_record->chassisAddress);
      p_record->cardAddress = ntohs(p_record->cardAddress);
      p_record->channelNumber = ntohs(p_record->channelNumber); 

      p_record->pisabitmap = ntohs(p_record->pisabitmap);
      p_record->PISASPARE4 = ntohs(p_record->PISASPARE4);
      p_record->atdptr = ntohs(p_record->atdptr);
      p_record->atdlay = ntohl(p_record->atdlay);

      p_record->oldsta = ntohl(p_record->oldsta);
      p_record->newsta = ntohl(p_record->newsta);
      p_record->initstat = ntohl(p_record->initstat);
      p_record->oldstt = ntohl(p_record->oldstt);

      p_record->curstt = ntohl(p_record->curstt);

      // all floating point 
      p_record->erange_lo = ntohf(*((uint32 *)&(p_record->erange_lo)));
      p_record->erange_hi = ntohf(*((uint32 *)&(p_record->erange_hi)));
      p_record->adfactor = ntohf(*((uint32 *)&(p_record->adfactor)));
      p_record->adoffset = ntohf(*((uint32 *)&(p_record->adoffset)));

      p_record->hhval = ntohf(*((uint32 *)&(p_record->hhval)));
      p_record->hval = ntohf(*((uint32 *)&(p_record->hval)));
      p_record->lval = ntohf(*((uint32 *)&(p_record->lval)));
      p_record->llval = ntohf(*((uint32 *)&(p_record->llval)));

      p_record->hhmet = ntohf(*((uint32 *)&(p_record->hhmet)));
      p_record->hmet = ntohf(*((uint32 *)&(p_record->hmet)));
      p_record->lmet = ntohf(*((uint32 *)&(p_record->lmet)));
      p_record->llmet = ntohf(*((uint32 *)&(p_record->llmet)));

      p_record->mfactor = ntohf(*((uint32 *)&(p_record->mfactor)));
      p_record->moffset = ntohf(*((uint32 *)&(p_record->moffset)));

      p_record->acon = ntohf(*((uint32 *)&(p_record->acon)));
      p_record->bcon = ntohf(*((uint32 *)&(p_record->bcon)));

      p_record->coeff0 = ntohf(*((uint32 *)&(p_record->coeff0)));
      p_record->coeff1 = ntohf(*((uint32 *)&(p_record->coeff1)));
      p_record->coeff2 = ntohf(*((uint32 *)&(p_record->coeff2)));
      p_record->coeff3 = ntohf(*((uint32 *)&(p_record->coeff3)));
      p_record->coeff4 = ntohf(*((uint32 *)&(p_record->coeff4)));
      p_record->coeff5 = ntohf(*((uint32 *)&(p_record->coeff5)));

      p_record->hidbval = ntohf(*((uint32 *)&(p_record->hidbval)));
      p_record->lodbval = ntohf(*((uint32 *)&(p_record->lodbval)));

      p_record->hidbmet = ntohf(*((uint32 *)&(p_record->hidbmet)));
      p_record->lodbmet = ntohf(*((uint32 *)&(p_record->lodbmet)));
      p_record->alrdl = ntohf(*((uint32 *)&(p_record->alrdl)));

      p_record->drftlm = ntohf(*((uint32 *)&(p_record->drftlm)));
      p_record->hislim = ntohf(*((uint32 *)&(p_record->hislim)));
      p_record->loslim = ntohf(*((uint32 *)&(p_record->loslim)));

      p_record->resistor = ntohf(*((uint32 *)&(p_record->resistor)));
      p_record->attenuator = ntohf(*((uint32 *)&(p_record->attenuator)));

      p_record->hi_OOR = ntohf(*((uint32 *)&(p_record->hi_OOR)));
      p_record->lo_OOR = ntohf(*((uint32 *)&(p_record->lo_OOR)));

      p_record->initval = ntohf(*((uint32 *)&(p_record->initval)));
      p_record->initmet = ntohf(*((uint32 *)&(p_record->initmet)));

      p_record->oldval = ntohf(*((uint32 *)&(p_record->oldval)));
      p_record->curval = ntohf(*((uint32 *)&(p_record->curval)));

      p_record->oldmet = ntohf(*((uint32 *)&(p_record->oldmet)));
      p_record->curmet = ntohf(*((uint32 *)&(p_record->curmet)));

      p_record->cursen = ntohf(*((uint32 *)&(p_record->cursen)));
   }
}

void CommLinkObject::dynDb_ntohlh(unsigned int dynSize)
{
   DYNAMIC_RECORD *p_dynrecord;
   U56_RECORD *p_record;

   int iteration = dynSize / sizeof(DYNAMIC_RECORD);

   for (int i = 0; i < iteration; i++)
   {
      p_record = &(gData.DbPoint[i]);
      p_dynrecord = &(gData.DynPoint[i]);

      p_dynrecord->pisabitmap = ntohs(p_dynrecord->pisabitmap);

      p_dynrecord->oldsta = ntohl(p_dynrecord->oldsta);
      p_dynrecord->newsta = ntohl(p_dynrecord->newsta);

      p_dynrecord->oldstt = ntohl(p_dynrecord->oldstt);
      p_dynrecord->curstt = ntohl(p_dynrecord->curstt);

      tmp1 = *((uint32 *)&(p_dynrecord->oldval));
      tmp2 = ntohl(tmp1);
      p_dynrecord->oldval = *((float *)&tmp2);

      tmp1 = *((uint32 *)&(p_dynrecord->curval));
      tmp2 = ntohl(tmp1);
      p_dynrecord->curval = *((float *)&tmp2);

      tmp1 = *((uint32 *)&(p_dynrecord->oldmet));
      tmp2 = ntohl(tmp1);
      p_dynrecord->oldmet = *((float *)&tmp2);

      tmp1 = *((uint32 *)&(p_dynrecord->curmet));
      tmp2 = ntohl(tmp1);
      p_dynrecord->curmet = *((float *)&tmp2);

      tmp1 = *((uint32 *)&(p_dynrecord->cursen));
      tmp2 = ntohl(tmp1);
      p_dynrecord->cursen = *((float *)&tmp2);
   }
}


// !!!! ntohf Function
float CommLinkObject::ntohf(uint32 netfloat)
{
   uint32 val = ntohl(netfloat);
   float *p = (float *)&val;
   return *p;
}


// !!! UpdateDynamicDb Function
void CommLinkObject::UpdateDynamicDb(unsigned int dynSize)
{
   //cout << getname() << " CommLinkObject::UpdateDynamicDb() START!!" << endl;

   U56_RECORD *p_record;
   DYNAMIC_RECORD *p_dynrecord;
   int i;

   int iteration = dynSize / sizeof(DYNAMIC_RECORD);

   for (i = 0; i < iteration; i++)
   {
      p_record = &(gData.DbPoint[i]);
      p_dynrecord = &(gData.DynPoint[i]);

      p_record->pisabitmap = p_dynrecord->pisabitmap;
      p_record->badata = p_dynrecord->badata;
      p_record->dlConnected = p_dynrecord->dlConnected;
/*
      p_record->oscan = p_dynrecord->oscan;
      p_record->ochek = p_dynrecord->ochek;
      p_record->enval = p_dynrecord->enval;
      p_record->unreli = p_dynrecord->unreli;
      p_record->pchnge = p_dynrecord->pchnge;
      p_record->oldoscan = p_dynrecord->oldoscan;
      p_record->oldochek = p_dynrecord->oldochek;
      p_record->oldenval = p_dynrecord->oldenval;

      // Added later
      p_record->llim = p_dynrecord->llim;
      p_record->hlim = p_dynrecord->hlim;
      p_record->lllim = p_dynrecord->lllim;
      p_record->hhlim = p_dynrecord->hhlim;
*/
      p_record->oldsta = p_dynrecord->oldsta;
      p_record->newsta = p_dynrecord->newsta;
      p_record->oldval = p_dynrecord->oldval;
      p_record->oldstt = p_dynrecord->oldstt;
      p_record->curval = p_dynrecord->curval;
      p_record->curstt = p_dynrecord->curstt;
      p_record->oldmet = p_dynrecord->oldmet;
      p_record->curmet = p_dynrecord->curmet;
      p_record->cursen = p_dynrecord->cursen;
   }
}




// by feel9
bool CommLinkObject::parseEOF()
{
  	char rcvbuffer[4096] = {0,};		
	char rcvDate[9] = {0,};
	char rcvTime[9] = {0,};

  	int len = 4096, rcvlen = 0;
	
	memset(&rcvbuffer, 0, sizeof(rcvbuffer));

	rcvlen = u_read(ReadSocket, rcvbuffer, len);				//select() & recvfrom()
	
	if(rcvlen <= 0)
	{
		cout << " parseEOF in u_read() TIMEOUT!!!" << endl;
		//sleep(1);
		return false; //201027_jhp
	}
		
	//start TAG RCV
	if(rcvTagIDOK == 0)
	{
		if(strncmp(rcvbuffer, "TT", 2) == 0 && rcvbuffer[3] == 0x01)
		{
			parseTagID(rcvbuffer, rcvlen);					//Make the TAG ID File.
			rcvTagIDOK = 1;
			return true;
		}
		else return false;
	}
	else if(rcvTagIDOK == 1)		//processing TAG RCV & DATA RCV
	{
		if(strncmp(rcvbuffer, "TT", 2) == 0 && rcvbuffer[3] == 0x01)
		{
			if (-1 == unlink("RCV_TAGID.txt"))
   			{
				if (ENOENT != errno)
	      		{
					cout << " Error deleting " << "RCV_TAGID.txt" << ". errno = " << errno
		      		<< endl;
					cout << " Recommend manually deleting " << "RCV_TAGID.txt"
					<< " and restarting." << endl;
	   			}
				else
				{
					cout << " " << "RCV_TAGID.txt" << " does not exist." << endl;
				}
			}
			parseTagID(rcvbuffer, rcvlen);
			rcvTagIDOK = 1;
			return true;
		}
		if(strncmp(rcvbuffer, "TT", 2) == 0)
		{
			parseTagID(rcvbuffer, rcvlen);
			return true;
		}
		else if(strncmp(rcvbuffer, "DD", 2) == 0 && rcvbuffer[3] == 0x01)			//TAG RCV END -> start DATA RCV
		{
			rcvTagIDOK = 2;
			rcvDataStart = true;
			memset(rcvTotalData, 0x00, MAXDATA);				
			totalCopyData = 0;
			memcpy(&rcvTotalData[totalCopyData], &rcvbuffer[4], rcvlen-4);
			totalCopyData += rcvlen-4;
			curDataRcv++;
				
			cout << "DD Data First RCV OK!!**************************" << endl;	
			return true;
		}
		else return false;
	}
	else if(rcvTagIDOK == 2)		
	{
		if(rcvDataStart == false)
		{
			if(strncmp(rcvbuffer, "TT", 2) == 0 && rcvbuffer[3] != 0x01)
			{
				cout << "1.TT RECV DATA NOT FIRST!!****************************" << endl;
				return false;
			}
			if(strncmp(rcvbuffer, "DD", 2) == 0 && rcvbuffer[3] != 0x01)
			{
				cout << "2.DD RECV DATA NOT FIRST!!****************************" << endl;
				return false;
			}
			else
			{
				rcvTagIDOK = 2;
				rcvDataStart = true;
				memset(rcvTotalData, 0x00, MAXDATA);

				totalCopyData = 0;
				memcpy(&rcvTotalData[totalCopyData], &rcvbuffer[4], rcvlen-4);

				totalCopyData += rcvlen-4;
				curDataRcv++;
				return true;
					
				//cout << "DD Data First RCV OK!!**************************" << endl;	
			}
		}
		if(strncmp(rcvbuffer, "DD", 2) == 0)
		{
			if (curDataRcv < maxDataRcv) 
			{
				memcpy(&rcvTotalData[totalCopyData], &rcvbuffer[4], rcvlen-4);
				totalCopyData += rcvlen-4;
				curDataRcv++;
			
				if (rcvbuffer[rcvlen-17] == 0x45 && rcvbuffer[rcvlen-18] == 0x45) 				//0x45´Â ½ÊÁø¼ö·Î 69. ÀÌÁø¼ö·Î 0100 0101, ¹®ÀڷÎ 'E'
				{
					memset(rcvTotalData, 0x00, MAXDATA);			//MAXDATA 35840
					totalCopyData = 0;
					curDataRcv = 0;
				}
				return true;
			}
			else 
			{
				memcpy(&rcvTotalData[totalCopyData], &rcvbuffer[4], rcvlen-4);
				totalCopyData += rcvlen-4;
				memcpy(rcvDate,   &rcvbuffer[rcvlen-8], 8);
				memcpy(rcvTime,   &rcvbuffer[rcvlen-16], 8);
			}
		}
		else return false;	
	}

	ifstream infile;
	infile.open("RCV_TAGID.txt", ifstream::nocreate);
		
	if(!infile.is_open())
	 {
		cout<<"RCV_TAGID.txt open error!************************"<<endl;
		return false;
	 }
		
	char strDataParse[16] = {0,};
	int curMemoryLocate = 0;
		
	while(infile)
	{
		infile.getline(strDataParse, 16);
		if(strlen(strDataParse) < 5) break;
		
		//DATA Parsing
		char rcvPid[9] = {0,};
		char rcvType = 0;
		char rcvOmsqs = 0;
		char rcvValue[4] = {0,};
		char rcvOldValue[4] = {0,};
		bool bcmp = false;

		memcpy(rcvPid, &strDataParse[0], 8);
		memcpy(&rcvType, &strDataParse[9], 1);			//ex) AFP00004:A 
		
		//Case = Type is Analog
		if(rcvType == 'A')
		{
			memcpy(rcvValue, &rcvTotalData[curMemoryLocate], 4);
			curMemoryLocate += 4;
			memcpy(&rcvOmsqs, &rcvTotalData[curMemoryLocate], 1);
			curMemoryLocate += 1;
		}
		else if(rcvType == 'D')
		{
			memcpy(rcvValue,  &rcvTotalData[curMemoryLocate], 1);
			curMemoryLocate += 1;
			memcpy(&rcvOmsqs, &rcvTotalData[curMemoryLocate], 1);
			curMemoryLocate += 1;
		}

		//Saving DATA
		float Value = 0;
		char saveString[256] = {0,};
		char newDate[32] = {0,};
		int i = 0;
		
		for( ; i<9 ;i++)
		{
			if(rcvPid[i] == ' ')
			{
				rcvPid[i] = '\0';
				break;
			}
		}
		
		if(2 >= strlen(rcvPid)) return false;
		
		float f_rcvValue = 0.0;
		short i_rcvValue = 0;
		short i_rcvValue_new = 0;
		int ipnIndex = 0;

		//CASE: bDataCopyFirst == true && bcmp == true
		if (rcvType == 'A') 		// ANALOG POINTs
		{
			//cout << "### Analog Save value 1 ###" << endl;
			f_rcvValue = RealToFloat(rcvValue);
			//sprintf(saveString, "%s,%s %s,%12e", rcvPid, rcvDate, rcvTime, f_rcvValue);
			//sprintf(saveString, "%s,%s %s,%d \t('A'rcvOmsq(10/16) : %d : 0x%x)", rcvPid, rcvDate, rcvTime, f_rcvValue, rcvOmsqs );
			//for Update Database in Shared Memory by Feel9 
			ipnIndex = find_pid(rcvPid);
                        //cout << "Analog ipnIndex : " << ipnIndex << " rcvPid : " << rcvPid << endl;
			if(ipnIndex >= 0) {
			   DBO.setvalue(ipnIndex, f_rcvValue);
			   DBO.setnewsta(ipnIndex, 0);
			}
		}
		else if (rcvType == 'D')		// DIGITAL POINTs
		{	
			//cout << "### Digital Save value 1 ###" << endl;
			if(rcvOmsqs >=63)
				rcvOmsqs = 25;
			i_rcvValue_new = (rcvValue[3] * 26) + rcvOmsqs;
			//sprintf(saveString, "%s,%s %s,%d", rcvPid, rcvDate, rcvTime, i_rcvValue_new);
			//sprintf(saveString, "%s,%s %s,%d \t('D'rcvOmsq(10/16) : %d : 0x%x)", rcvPid, rcvDate, rcvTime, i_rcvValue_new, rcvOmsqs, rcvOmsqs);
			//for Update Database in Shared Memory by Feel9 
			ipnIndex = find_pid(rcvPid);

                        //cout << "Digital ipnIndex : " << ipnIndex << " rcvPid : " << rcvPid << endl;
			// cout << "i_rcvValue_new = " << i_rcvValue_new << endl;
			if(ipnIndex >= 0) {
			   //cout << " rcvPid = " << rcvPid << " Value = " << rcvValue[3] << " State = " << rcvOmsqs << endl;
			   // DBO.setvalue(ipnIndex, i_rcvValue_new);
			   DBO.setvalue(ipnIndex, rcvValue[3]);
			   DBO.setnewsta(ipnIndex, rcvOmsqs);
			}
		}
		else 
			return false;
		AllMap[rcvPid] = saveString;

	}//End of While(infile)
	
	infile.close();

	totalCopyData = 0;
	curDataRcv = 0;

	return true;

}

// by feel9
void CommLinkObject::parseTagID(char *buf, int buflen)
{
	char fName[50];
	ofstream eventFile;
	int tagHeaderLen = 4;
	int bufLenCount = 4;
	int i = 0;
	char tagID[9] = {0,};						//PID = 8Bytes
	char tagID_AD[2] = {0,};				// Analog, Digital = 2Bytes
	
	sprintf(fName, "RCV_TAGID.txt");
	//eventFile.open(fName, ios::outlios::app);
	eventFile.open(fName, ios::app);
	chmod("RCV_TAGID.txt", 0777);
	
	if(!eventFile.is_open())
	{
		cout << "Retry RCV_TAGID File Open !!!"<<endl;
		eventFile.open(fName);
		if(!eventFile.is_open()) return ;
	}
	
	if(rcvTagIDOK == 0 || rcvTagIDOK == 1)
	{
		for(i = 0; i < 100; i++)								//recv bytes is 908byte. front 4bytes, last 4bytes is no counting. so 9byte read * 100 = 900 bytes.
		{
			strncpy(tagID, buf + bufLenCount, 8);				// 0~3 is TT ~ EE, 0x00, 0x01~.
			bufLenCount += 8;
			strncpy(tagID_AD, buf + bufLenCount, 1);
			bufLenCount += 1;

			eventFile << tagID << ":" << tagID_AD << endl;
			rcvTagIDNum++;
			
			if(bufLenCount + 4 >= buflen)	break;
			if(rcvTagIDNum == totalrcvTagIDNum)	break;					// totalrcvTagIDNum = 6904
		} 
	}
	
	eventFile.close();
	
	return ;
}

bool CommLinkObject::WritePIDataFile(char *Date, char *Time)
{
	char msg[256];

	if (Date == NULL || Time == NULL) {
		cout<<"WritePIDataFile() datetime ERROR***************"<<endl;;  
		return false;
	}
	
	if(file_save_need)
	{
		if (AllMap.empty()) {
			cout<<"WritePIDataFile() AllMap.empty() ERROR***************"<<endl;;  
			return false;
		}
	}
	else return false;

	//int i, j;
	char buff[256];
	//char filename[256], new_datetime[32], tmp_datetime[32];

/*
	memset(new_datetime, 0, 32);
	for (i = 0, j = 0; i < (int)strlen(datetime); i++) {
		if (datetime[i] == ':' || datetime[i] == '/' || datetime[i] == ' ') continue;
		else if (datetime[i] == 0) break;
		else {
			new_datetime[j++] = datetime[i];
		}
	}
	memcpy(tmp_datetime, new_datetime, 32);
	new_datetime[0] = tmp_datetime[4];
	new_datetime[1] = tmp_datetime[5];
	new_datetime[2] = tmp_datetime[0];
	new_datetime[3] = tmp_datetime[1];
	new_datetime[4] = tmp_datetime[2];
	new_datetime[5] = tmp_datetime[3];
*/


	ofstream RecvFile;

/*
	sprintf(buff, "%s", gData.pi_filepath);
	_mkdir(buff);

	sprintf(buff, "%s\\data_%s.csv", gData.pi_filepath, new_datetime);
	strcpy(filename, buff);
*/

	//RecvFile.open("PIDATA.txt", ios::app);
	RecvFile.open("PIDATA.txt", ios::out);
	if (! RecvFile.is_open()) {
		RecvFile.open ("PIDATA.txt");
		if (! RecvFile.is_open()){
			cout<<"PIDATA FILE OPEN ERROR!**************************"<<endl;
			return false;
		}
	}	

	string strBuf;					
	int data_count = 0;			

	for(map_Pos = AllMap.begin(); map_Pos != AllMap.end(); map_Pos++) {	
		strBuf = map_Pos->second;										
		RecvFile << strBuf << "\n";
		data_count++;
	}
	AllMap.clear();	

	RecvFile.flush();
	RecvFile.close();

	cout<<"\n\n******PIDATA WRITE SUCCESS!!***************\n"<<endl;
	cout<<"\n\n******data_count = " << data_count << "***************\n"<<endl;
	chmod("PIDATA.txt", 0777);

	IPO.setReceivedDBdownload(TRUE);

	return true;
}

float CommLinkObject::RealToFloat(char *chReal)
{
	float val, tmp[4];
	int S_V, E_V;
	unsigned char S_M = 0x80;
	unsigned char E_M = 0x7f;
	unsigned char cval[4];

	memcpy(cval, chReal, sizeof(cval));

	S_V = cval[0] & S_M;
	E_V = cval[0] & E_M;
	E_V = E_V - 64;

	if(S_V > 0){
		S_V = 1;
	}

	tmp[1] = cval[1];
	tmp[1] = tmp[1] / 256;
	tmp[2] = cval[2];
	tmp[2] = tmp[2] / (256*256);
	tmp[3] = cval[3];
	tmp[3] = tmp[3] / (256*256*256);
	val = tmp[1] + tmp[2] + tmp[3];
	val = (float) (pow(-1., S_V) * val * pow(16., E_V));

	return val;
}

// by feel9 
// returns on successful connection to a host.
void CommLinkObject::Connect()
{
   // CORENET ADD 202106
   char * sendUse;
   extern CommLinkObject	CommLink;
   // CORENET ADD END

   extern	InterProcSMObject	IPO;
   int status;

   IPO.setmasterStatus(getnum(),0);	// unknown master/slave
   IPO.setconnected(getnum(),FALSE);	// not connected
   IPO.setfailoverState(getnum(),0);	// No disability index
   
   while (0 >= (status = connectWSToHost()) )
   {
      cout << getname()
	   << "CommLinkObject::Connect() ERROR from connectWSToHost = " 
	   << dec << status << " Retrying to connect.\n";
   }

   IPO.setconnected(getnum(), TRUE);

   // CORENET ADD 202106
   sendUse = CommLink.getsendUse();

   if (!strcmp(sendUse, "1"))
   {
      setWriteSocket();
   }
   // CORENET ADD END

}

void CommLinkObject::setWriteSocket()
{
   cout << "CommLinkObject::setWriteSocket START !!!!" << endl;

   if( 0 > (WriteSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))
   {
      cerr << "***** ERROR : " << getname()
           << "CommLinkObject::setWriteSocket() : WriteSocket "
           << "socket\n";
      flush(cerr);
   }

   memset (&sockaddr_to, 0, sizeof(sockaddr_to));
   sockaddr_to.sin_port = htons(30000);

   sockaddr_to.sin_family = AF_INET;
   sockaddr_to.sin_addr.s_addr = inet_addr("10.93.20.66");

   cout << "CommLinkObject::setWriteSocket END !!!!   " << WriteSocket << endl;
}


// returns on successful connection to a host.
void CommLinkObject::Connect_old()
{
   extern	InterProcSMObject	IPO;
   int status;

   IPO.setmasterStatus(getnum(),0);	// unknown master/slave
   IPO.setconnected(getnum(),FALSE);	// not connected
   IPO.setfailoverState(getnum(),0);	// No disability index
   
   while (0 >= (status = connectWSToHost()) )
   {
      cout << getname()
	   << "CommLinkObject::Connect() ERROR from connectWSToHost = " 
	   << dec << status << " Retrying to connect.\n";
   }

   // provide separate sockets for reads and writes.
   if (0 > (WriteSocket = dup(ReadSocket)) )
   {
      cerr << "***** ERROR: " << getname()
	   << "CommLinkObject::Connect(): Can not dup Socket: "
	   << "Error = " << dec << errno << endl;
      cerr << "***** EXITING CommLink *****\n";
      exit(1);
   }
   IPO.setconnected(getnum(), TRUE);
}

// by feel9 
// returns a socket number for a completed connection or -1 for failure
//to complete socket connection.
int CommLinkObject::connectWSToHost()
{
   extern DatabaseObject DBO;
   extern INTERFACE *	 Interface;	// shared memory GUI variables
   extern InterProcSMObject	IPO;	// Inter Process shared memory
   extern CommLinkObject	CommLink;
   
   u_port_t	myPort;			// connection port number
   //u_port_t	myPrivatePort;		// individual port number
   int		error;			// error flag
   int		retryCount;	// how many times to retry
   ssize_t	bytesread;		// bytes received from host
   //int	mySocket;		
   char *	hostName;  // local copy of host this CommLink is connected to
   char         portNum[MAX_PORTNO_LEN];
   char         wsname[MAX_HOSTNAME_LEN+1]; // name of this workstation
   char *       wsname_ptr;
   int          readCount;
   time_t	currentTime;

   char		commandStr[MODEM_OPTION_LEN +1 + 10];
   char		modemStr[MODEM_OPTION_LEN + 1];

   // init some shared memory variables
   hostName = CommLink.gethostName(); 

   // the well known port number
   //myPort = LISTEN_PORT;		//by feel9
   strcpy(portNum, CommLink.getportNum());
   myPort = atoi(portNum);

   cout << "CommLinkObject::connectWSToHost() myPort = " << dec << myPort << endl;

   // initialize to no connection
   CommLink.resetconnected();								//value connected = FALSE;

   // initialize to no error
   error = TRUE;

   while (error)
   {
      while (!(CommLink.isconnected()) )				//return value->connected
      {
         cout << getname() << " CommLinkObject::connectWSToHost() "
	      << "Trying to connect to " << hostName << endl;
         // get the modem option string
         IPO.getMODEMOption(modemStr);
#ifdef DEBUG
         cout << getname() << "CommLinkObject::Connect() got modem string = "
              << modemStr << endl;
#endif
	 
         for (retryCount = 0; retryCount < HOST_TRY_LIMIT; retryCount++)
         {
	   // Try to connect and get a valid file descriptor for the socket
	   if ( (ReadSocket = u_connect(myPort, hostName)) < 0)	
	   {
	    	if (retryCount < HOST_TRY_LIMIT-1)
	       	{
                  cout << getname() 
	   	       << "CommLinkObject::connectWSToHost(): Retrying to "
		       << "connect to " << hostName << endl;
                  flush(cout);
	       	}
	       	sleep(HOST_RETRY_WAIT);	// wait a little bit
	    } //endif
            else
            {
               CommLink.setconnected();	// success
	       cout << getname() 
	  	    << "CommLinkObject::connectWSToHost(): Successfully "
		    << "connected to " << "socket = " << dec << ReadSocket 
		    << endl;
               break;
            }
         } //endfor

         if (!(CommLink.isconnected()) )
         {
            cerr << getname() 
		 << "***** ERROR CommLinkObject::connectWSToHost() "
		 << "Unable to connect to " << hostName << endl;
	    cerr << "   Waiting " << dec << HOST_CONNECT_WAIT 
	 	 << " seconds before trying to connect again" << endl;
	    flush(cerr);
	    sleep(HOST_CONNECT_WAIT);
         } //endif
      } //endwhile !connected

#if 0	// CORENET 2016.07.26
      for (wsname_ptr = wsname; wsname_ptr <= wsname+MAX_HOSTNAME_LEN ; ++wsname_ptr)
          *wsname_ptr = ' ';
      gethostname(wsname,MAX_HOSTNAME_LEN);
      for (wsname_ptr = wsname; wsname_ptr <= wsname+MAX_HOSTNAME_LEN ; ++wsname_ptr)
          if (*wsname_ptr == 0 ) *wsname_ptr = ' ';
      wsname[12] = 0;
#else
      int i, setflag = 0;
      memset(wsname, 0, sizeof(wsname));
      gethostname(wsname, sizeof(wsname));

      for (i = 0; i < sizeof(wsname); i++) {
	if (0 == isalnum(wsname[i])) setflag = 1;
	if (1 == setflag) wsname[i] = ' ';
      }
      wsname[12] = 0;
#endif

      error = FALSE;
      break;		// the only way out of error loop
   } //endwhile error

   // SUCCESS!!!
   cout << getname()
	 << " CommLinkObject::connectWSToHost(): Connection to host "
	 << "successfully\n";
   cout << "   Network Connection Established with socket " << dec 
	 << ReadSocket << endl;
   currentTime = getLocalTime();
	 IPO.settimeOfLastRead(getnum(), currentTime);
	 IPO.setNumTicksOfLastRead(getnum(), IPO.getNumTicks());
   cout << "   Connection has been made to host " << hostName 
	 << " at " << ctime(&currentTime) << endl;
   tzset();
   cout << "   Current Time Zone is " << tzname[0] << endl;
   flush(cout);
   
   return(ReadSocket);

} //end connectWSToHost



// returns a socket number for a completed connection or -1 for failure
//to complete socket connection.
int CommLinkObject::connectWSToHost_old()
{
   extern DatabaseObject DBO;
   extern INTERFACE *	 Interface;	// shared memory GUI variables
   extern InterProcSMObject	IPO;	// Inter Process shared memory
   extern CommLinkObject	CommLink;
   
   u_port_t	myPort;			// connection port number
   u_port_t	myPrivatePort;		// individual port number
   int		error;			// error flag
   int		retryCount;	// how many times to retry
   ssize_t	bytesread;		// bytes received from host
   int		mySocket;		
   char *	hostName;	// local copy of host this CommLink is 
				//connected to
   char         wsname[MAX_HOSTNAME_LEN+1]; // name of this workstation
   char *       wsname_ptr;
   int          readCount;
   time_t	currentTime;

   char		commandStr[MODEM_OPTION_LEN +1 + 10];
   char		modemStr[MODEM_OPTION_LEN + 1];

   // init some shared memory variables
   hostName = CommLink.gethostName(); 

   // the well known port number
   myPort = LISTEN_PORT;

   // initialize to no connection
   CommLink.resetconnected();

   // initialize to no error
   error = TRUE;

   while (error)
   {
      while (!(CommLink.isconnected()) )
      {
         cout << getname() << " CommLinkObject::connectWSToHost() "
	      << "Trying to connect to " << hostName << endl;
         // get the modem option string
         IPO.getMODEMOption(modemStr);
#ifdef DEBUG
         cout << getname() << "CommLinkObject::Connect() got modem string = "
              << modemStr << endl;
#endif
	 /*--------------------------------------> MODEM/FDDI ?? -------------- CORENET 2012.07.12
         // if there is a modem, call the lan rover in the modem option string
         if( (0 != strcmp("OFF", modemStr))  &&  (0 != strcmp("", modemStr)) )
         {  
            int i = 0;
            int fd = -1;

            // ensure connection status file for pplstat call exists
            //if(-1 == (fd = open("/var/ppl/ptmp", O_RDWR | O_CREAT | O_EXCL)) )
            if(-1 == (fd = open("/var/ppl/ptmp", O_RDWR | O_CREAT | O_EXCL, 666)) )	// for Linux (2012.07.05. CORENET )
            {
               if(EEXIST != errno)
               {
                  cout << "***** " << getname() << "CommLinkObject::initCommLink: "
                       << "Problem opening  /var/ppl/ptmp. errno = " << dec << errno 
		       << endl;
               }
               else // file exists
               {
                  chmod("/var/ppl/ptmp", 0666); // ensure correct protect keys
               }
            }
            else // open was successful
            {
               close(fd);
	       chmod("/var/ppl/ptmp", 0666);
            }

            cout << getname() << "CommLinkObject::Connect() attempting to dial "
                 << "modem." << endl;
            strcpy(commandStr, "ppl -o ");
            strcat(commandStr, modemStr);
            cout << getname() << "CommLinkObject::Connect() "
                 << " Issuing system command = system(" << commandStr << ")"
                 << endl;
            i = system(commandStr);
            cout << getname() << " CommLinkObject::Connect(): system call to " 
                 << "ppl returned " << hex << i << endl;
            if(WIFEXITED(i))
            {
               cout << getname() << " CommLinkObject::Connect(): " 
                    << "Status from ppl is " << hex << WEXITSTATUS(i) << endl;
               system("pplstat -n");
               if (0 == WEXITSTATUS(i)) 
               {
                  cout << getname() << " CommLinkObject::Connect(): " 
                       << " ppl returned successfully. Return code is 0." << endl;
               }
               else
               {
                  cout << getname() << " CommLinkObject::Connect(): " 
                       << "*** Call to ppl returned unsuccessfully. ppl return code = " 
                       << hex << WEXITSTATUS(i) << endl;
               }
            }
            else
            {
               perror("**** CommLinkObject::Connect() System call problem.");
               cout << getname() << " CommLinkObject::Connect(): system(" 
                    << commandStr << ") " << "returned " << hex << i << endl;
            }
         }
	 -------------------------------------*/

         for (retryCount = 0; retryCount < HOST_TRY_LIMIT; retryCount++)
         {
	    // Try to connect and get a valid file descriptor for the socket
	    if ( (mySocket = u_connect(myPort, hostName)) < 0)
	    {
	       if (retryCount < HOST_TRY_LIMIT-1)
	       {
                  cout << getname() 
		       << "CommLinkObject::connectWSToHost(): Retrying to "
		       << "connect to " << hostName << endl;
                  flush(cout);
	       }
	       sleep(HOST_RETRY_WAIT);	// wait a little bit
	    } //endif
            else
            {
               CommLink.setconnected();	// success
	       cout << getname() 
		    << "CommLinkObject::connectWSToHost(): Successfully "
		    << "connected to " << "socket = " << dec << mySocket 
		    << endl;
               break;
            }
         } //endfor
         if (!(CommLink.isconnected()) )
         {
            cerr << getname() 
		 << "***** ERROR CommLinkObject::connectWSToHost() "
                 << "Unable to connect to " << hostName << endl;
            cerr << "   Waiting " << dec << HOST_CONNECT_WAIT 
		 << " seconds before trying to connect again" << endl;
            flush(cerr);
	    sleep(HOST_CONNECT_WAIT);
         } //endif
      } //endwhile !connected

#if 0	// CORENET 2016.07.26
      for (wsname_ptr = wsname; wsname_ptr <= wsname+MAX_HOSTNAME_LEN ; ++wsname_ptr)
          *wsname_ptr = ' ';
      gethostname(wsname,MAX_HOSTNAME_LEN);
      for (wsname_ptr = wsname; wsname_ptr <= wsname+MAX_HOSTNAME_LEN ; ++wsname_ptr)
          if (*wsname_ptr == 0 ) *wsname_ptr = ' ';
      wsname[12] = 0;
#else
      int i, setflag = 0;
      memset(wsname, 0, sizeof(wsname));
      gethostname(wsname, sizeof(wsname));

      for (i = 0; i < sizeof(wsname); i++) {
	if (0 == isalnum(wsname[i])) setflag = 1;
	if (1 == setflag) wsname[i] = ' ';
      }
      wsname[12] = 0;
#endif

/*
// by feel9 
      cout << "sending workstation name to host === [" << wsname  << "]" << endl;
      flush(cout);

      if ((write(mySocket, wsname, MAX_HOSTNAME_LEN)) < 0)
      {
         cout << "Error sending workstation name to host\n";
      }
*/
      /*--------------------
      for(readCount=0,bytesread=0; readCount < 10 && bytesread == 0; readCount++)
      {
         bytesread = u_read(mySocket, &myPrivatePort, sizeof(u_port_t));
	 cout << "bytesread = " << dec << bytesread << endl;
	 cout << "myPrivatePort = " << dec << ntohs(myPrivatePort) << endl;
	 cout << "sizeof(u_port_1) = " << dec << sizeof(u_port_t) << endl;
         sleep(2);
      }
      --------------------*/
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      bytesread = u_read(mySocket, (char *)&myPrivatePort, sizeof(u_port_t));
      if (bytesread == 0) {
	 cout << "u_read() PEER CLOSE !!!!" << endl;
         sleep(2);
      }
      else if (bytesread < 0) {
	 cout << "u_read() TIME OUT !!!!" << endl;
	 sleep(2);
      }
      else {
	 cout << "bytesread = " << dec << bytesread << endl;
#ifdef LINUX_APP
	 cout << "myPrivatePort = " << dec << ntohs(myPrivatePort) << endl;
#else
	 cout << "myPrivatePort = " << dec << myPrivatePort << endl;
#endif
	 cout << "sizeof(u_port_t) = " << dec << sizeof(u_port_t) << endl;
      }
      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      // Connection made. The host should now send a private port number
      if (sizeof(u_port_t) != bytesread)
      {
         cerr << getname()
	      << " ***** ERROR: CommLinkObject::connectWSToHost() "
              << "Error reading private port number from host\n";
         if (0 != close(mySocket)) 
	 {
	    cerr << "   ***** ERROR: CommLinkObject::connectWSToHost(): "
		 << "Error closing the socket.  EXITING!!!\n";
	    exit(1);
	 }
         CommLink.resetconnected();
      } //endif
      else if (0 < myPrivatePort)
      {

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	  myPrivatePort = ntohs(myPrivatePort);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

         // the only way out
         cout << getname() 
	      << " CommLinkObject::connectWSToHost() Read private port = "
              << dec << myPrivatePort << endl;
         flush(cout);
         error = FALSE;
         break;		// the only way out of error loop
      }

      // We are here because the number of bytes read was not correct, or
      //the port read from the host was 0 or or negative.
      cout << getname() 
	   << " CommLinkObject::connectWSToHost(): Problem with private port. "
           << endl; 
      cout << "   Waiting " << dec << HOST_CONNECT_WAIT 
           << " before trying to connect again" << endl;
      flush(cout);
   } //endwhile error

   // close the well known port
   if (0 != close(mySocket))
   {
      cerr << getname()
	   << " CommLinkObject::connectWSToHost(): "
	   << "Error closing well known port\n";
      cerr << "***** EXITING " << getname() << " *****\n";
      exit(1);
   }
   resetconnected();
   myPort = myPrivatePort;

   // Reopen the connection
   // Reopen the connection on the private port on the responding host.
   //Do not loop on an error.  Instead return -1 and the caller handle it.
   cout << getname() 
	<< " CommLinkObject::connect trying to connect to "
        << hostName << " on the private port " << myPort << endl;
   for (retryCount = 0; retryCount < HOSTREAD_TRY_LIMIT; retryCount++)
   {
      if (0 > (ReadSocket = u_connect(myPort, hostName)) )
      {
         sleep (HOSTREAD_RETRY_WAIT);
      } //endif
      else
      {
         // SUCCESS!!!
         cout << getname()
	      << " CommLinkObject::connectWSToHost(): Connection to host "
	      << "successfully\n";
         cout << "   Network Connection Established with socket " << dec 
	      << ReadSocket << endl;
	 cout << "   Private port = " << dec << myPort << endl;
         currentTime = getLocalTime();
	 IPO.settimeOfLastRead(getnum(), currentTime);
	 IPO.setNumTicksOfLastRead(getnum(), IPO.getNumTicks());
         cout << "   Connection has been made to host " << hostName 
	      << " at " << ctime(&currentTime) << endl;
         tzset();
         cout << "   Current Time Zone is " << tzname[0] << endl;
         flush(cout);
	 setconnected();
         break; 
      } //endelse
   } //endfor

   return(ReadSocket);
} //end connectWSToHost

//by feel9 
int CommLinkObject::u_connect(u_port_t port, char * hostName)
{
   struct sockaddr_in	server;

   int	mySock;
   int	retval;
   int	retryCount;
   int  broadcast = 1;
   int  temp;

   cout << "CommLinkObject::u_connect() port = " << dec << port << endl;

   // if the sigpipe signal can't be ignored, or if host information
   //can not be gotten, or if there is an error in getting a 
   //descriptor for the socket, return an error indication.
   //NOTE: The socket call specifies the Protocol Family, i.e., PF_INET 
   //while the Address Family, i.e., AF_INET gets put into the 
   //sockaddr_in structure.

   if (0 != u_ignore_sigpipe())
   {
      cerr << "***** ERROR: " << getname() 
	   << "CommLinkObject::u_connect(): trouble ingnoring "
	   << "SIGPIPE signal" << endl;
      flush(cerr);
      return(-1);
   }

    cout << "#############################################\n"
	 << "Trying Create UDP Socket !!!!!\n" 
	 << "#############################################" << endl;

   if (0 > (mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) )
   {
      cerr << "***** ERROR: " << getname()
	   << "CommLinkObject::u_connect(): trouble getting "
	   << "socket\n";
      flush(cerr);
      return(-1);
   }

    cout << "\n#############################################\n"
	 << "UDP Socket Creating is SUCCESS!!!!!!\n" 
	 << "u_connect Sock ==== " << mySock <<"\n"
	 << "#############################################" << endl;

   // set socket for non-blocking I/O operations
   //fcntl(mySock, F_SETFL, O_NDELAY);
   fcntl(mySock, F_SETFL, O_NONBLOCK);
	
   //set Socket Option.
   setsockopt(mySock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

   // copy host information into structure for socket connection 
   memset((char *)&server.sin_addr, 0, sizeof(struct sockaddr_in));

   // for portability, convert from host to network byte order
   server.sin_port = htons(port);

   // set address family to be used to interpret addresses in later 
   //operations to the DARPA Internet addresses.
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);

/*
   if (strcmp("CommLink1", getname()) == 0) {
     cout << "COMMLINK1 CONNECT~~~~~~~~~" << endl;
     server.sin_addr.s_addr = inet_addr("10.93.20.5");
   }
   else {
     cout << "COMMLINK2 CONNECT~~~~~~~~~" << endl;
     server.sin_addr.s_addr = inet_addr("10.93.20.6");
   }
   sendto(mySock, 0x00, 1, 0, (struct sockaddr *)&server, sizeof(server));            
*/



   if (0 > (temp = bind(mySock, (struct sockaddr *)&server, sizeof(server))) )
   {
      cerr << "***** ERROR: " << getname()
	   << "CommLinkObject::u_connect(): trouble getting "
	   << "Binding\n";
      flush(cerr);
      return(-1);
   }
    cout << "\n#############################################\n"
	 << "Bindding Socket is SUCCESS!!!!!!\n" 
	 << "#############################################" << endl;

    cout << getname()
	 << "CommLinkObject::u_connect() Trying to connect to host: " 
         << hostName << " socket = " << dec << mySock << endl;

   return (mySock);
} //end u_connect


int CommLinkObject::u_connect_old(u_port_t port, char * hostName)
{
   struct sockaddr_in	server;
   struct hostent *	hp;
   int	mySock;
   int	retval;
   int	retryCount;

   // if the sigpipe signal can't be ignored, or if host information
   //can not be gotten, or if there is an error in getting a 
   //descriptor for the socket, return an error indication.
   //NOTE: The socket call specifies the Protocol Family, i.e., PF_INET 
   //while the Address Family, i.e., AF_INET gets put into the 
   //sockaddr_in structure.
   if (0 != u_ignore_sigpipe())
   {
      cerr << "***** ERROR: " << getname() 
	   << "CommLinkObject::u_connect(): trouble ingnoring "
	   << "SIGPIPE signal" << endl;
      flush(cerr);
      return(-1);
   }

   if (!(hp = gethostbyname(hostName)))
   {
      cerr << "***** ERROR: " << getname()
	   << "CommLinkObject::u_connect(): trouble getting "
	   << "memory structure for host" << endl;
      flush(cerr);
      return(-1);
   }

   if (0 > (mySock = socket(PF_INET, SOCK_STREAM, 0)) )
   {
      cerr << "***** ERROR: " << getname()
	   << "CommLinkObject::u_connect(): trouble getting "
	   << "socket\n";
      flush(cerr);
      return(-1);
   }

   // set socket for non-blocking I/O operations
   //fcntl(mySock, F_SETFL, O_NDELAY);
   fcntl(mySock, F_SETFL, O_NONBLOCK);

   // copy host information into structure for socket connection 
   memcpy((char *)&server.sin_addr, hp->h_addr_list[0], hp->h_length);

   // for portability, convert from host to network byte order
   server.sin_port = htons(port);

   // set address family to be used to interpret addresses in later 
   //operations to the DARPA Internet addresses.
   server.sin_family = AF_INET;

   cout << getname()
	<< "CommLinkObject::u_connect() Trying to connect to host: " 
        << hostName << " socket = " << dec << mySock << endl;
   for (retryCount = 0; retryCount < CNCT_TRY_LIMIT; retryCount++)
   {
      while ( (-1 == (retval = 
	           connect(mySock, (struct sockaddr *)&server, sizeof(server)))
	      ) && (EINTR == errno) 
	    ) 
      {
         // keep trying if the connect was interrupted by a signal before the
         //connect sequence was complete.
         cout << getname()
	      << "CommLinkObject::u_connect() Looping while trying to connect "
              << "being interrupted by a signal\n";
      } //endwhile
#ifdef DEBUG
      cout << getname()
	   << " CommLinkObject::u_connect(): connect returned with retval = "
	   << dec << retval << " and errno = " << errno << endl;
#endif
      switch (errno)
      {
	 case EINPROGRESS:
	    cout << getname()
		 << " CommLinkObject::u_connect(): connect() to " << hostName 
	         << " in progress" << endl;
	    sleep(CNCT_RETRY_WAIT);	// wait a little bit
	    break;
         case EALREADY:
#ifdef DEBUG
	    cout << getname()
		 << " CommLinkObject::u_connect(): connect() to " << hostName
	         << " still in progress" << endl;
#endif
	    sleep(CNCT_RETRY_WAIT);	// wait a little bit
            break;
         case EISCONN:
	    cout << getname()
		 << " CommLinkObject::u_connect(): connect() to " << hostName
		 << " SUCCESSFUL!!!" << endl;
	    retryCount = CNCT_TRY_LIMIT;
	    break;
         default:
	    cout << getname()
		 << " CommLinkObject::u_connect(): ERROR with connect() to "
		 << hostName << " Retrying..." << endl;
	    sleep(CNCT_RETRY_WAIT);	// wait a little bit
	    break;
      }
   } //endfor
   if (-1 == retval  &&  !(EISCONN == errno))
   {
      cerr << "***** ERROR " << getname()
	   << " CommLinkObject::u_connect() CAN NOT CONNECT to "
	   << "host = " << hostName << ". socket = " << dec << mySock << endl;
      flush(cerr);
      close(mySock);
      return (-1);
   }
   return (mySock);
} //end u_connect


#ifdef DEBUG
 #ifndef U_IG_SP_DEBUG
  #define U_IG_SP_DEBUG
 #endif
#endif
/* return 1 if error, 0 if OK */
int CommLinkObject::u_ignore_sigpipe()
{
   struct sigaction act;

   // A SIGPIPE signal is raised if a process sends on a broken stream.  
   //This would normally cause a process to exit.  An end-of-file 
   //condition (zero bytes read)_ is returned if a process tries 
   //to read on a broken stream.

   // if we can't check the action of the SIGPIPE signal, return error
   if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
   {
#ifdef U_IG_SP_DEBUG
      cout << getname()
	   << "CommLinkObject::u_ignore_sigpipe() Can not check the action of "
	   << "the SIGPIPE signal. Return 1, i.e., error.\n";
#endif
      return 1;
   }

   // if the action associated with writing a pipe with no one to read,
   //i.e., sending on a broken stream, is the default action, (usually exit)
   //change it to ignore the signal. if that can not be done, return an error.
   if (act.sa_handler == SIG_DFL) {
      act.sa_handler = SIG_IGN;
      if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
      {
#ifdef U_IG_SP_DEBUG
      cout << getname()
	   << "CommLinkObject::u_ignore_sigpipe() Can not change the action of "
	   << "the SIGPIPE signal. Return 1, i.e., error.\n";
#endif
         return 1;
      }
   }
#ifdef U_IG_SP_DEBUG
      cout << getname()
	   << "CommLinkObject::u_ignore_sigpipe() Success in setting to ignore "
	   << "the SIGPIPE signal. Return 0\n";
#endif
   return 0;
}

#ifndef U_READ_DEBUG
#define U_READ_DEBUG
#endif
// by feel9 
ssize_t CommLinkObject::u_read(int fd, char *buf, size_t size)
{
   extern CommLinkObject      CommLink; 

   int retval = 0;
   int datacnt = 0;
   int dataflag = 0;
   int waitcnt = 0;
   char * sendUse;

   fd_set reads;
   struct timeval timeout;
   int result;

   FD_ZERO(&reads);
   FD_SET(fd, &reads);
	
   timeout.tv_sec = 15;
   timeout.tv_usec = 0;
	
   result = select(fd+1, &reads, 0, 0, &timeout);
   if (result == -1) {
			cout << getname() << "  select ERROR1" << endl;
      flush(cout);
			return -1;
   }
   else if (result == 0) {
			cout << getname() << "  select TIMEOUT" << endl;
      flush(cout);
			return -1;
   }
   else if (!FD_ISSET(fd, &reads)) {
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

   if((rcvBytes = recvfrom(ReadSocket, buf, toberead, 0, (struct sockaddr *)&sockaddr_from, &slen)) < 0)
   {
	cout << " recvfrom ERROR  ***************" << endl;
   	return -1;
   }
   else
   {
        if (rcvBytes > 0)
	{
            sendUse = CommLink.getsendUse();
        
            cout << " >>>>>>>>>>> CommLink -> SEND USE : " << sendUse << " <<<<<<<<<<<<" << endl;

            if(!strcmp(sendUse, "1"))
	    {
               sendto(WriteSocket, buf, rcvBytes, 0, (struct sockaddr *)&sockaddr_to, slen);            
   	    }
	}

	cout << "CommLink::u_read rcvBytes = " << rcvBytes << endl;
  	return rcvBytes;	
   }

}

ssize_t CommLinkObject::u_read_old(int fd, char *buf, size_t size)
{
   
   int retval = 0;
   int datacnt = 0;
   int dataflag = 0;
   int waitcnt = 0;

   fd_set reads;
   struct timeval timeout;
   int result;

   FD_ZERO(&reads);
   FD_SET(fd, &reads);
	
   timeout.tv_sec = 2;
   timeout.tv_usec = 0;
	
   result = select(fd+1, &reads, 0, 0, &timeout);
   if (result == -1) {
			cout << getname() << "  select ERROR1" << endl;
      flush(cout);
			return -1;
   }
   else if (result == 0) {
			cout << getname() << "  select TIMEOUT" << endl;
      flush(cout);
			return -1;
   }
   else if (!FD_ISSET(fd, &reads)) {
			cout << getname() << "  select ERROR2" << endl;
     	flush(cout);
			return -1;
   }
	
   while (size > datacnt) {
	retval = read(fd, (char *)(buf+datacnt), size-datacnt);

        if (retval == -1) {
	     if (errno == EWOULDBLOCK || errno == EAGAIN) {
	     	cout << getname() << " u_read() WAIT !!! " << endl;
	     }
        }
	else if (retval == 0) {
	     cout << getname() << " u_read() PEER CLOSE !!! " << endl;
	     return 0;
	}
	else {
	     datacnt += retval;
	     waitcnt = 0;
	}
	if (size == datacnt) {
	     //cout << getname() << " u_read() retval = " << dec << datacnt
	     //     << ",  u_read() size = " << dec << size << endl;
 	     return datacnt;
	}
	else {
	     if (waitcnt > 2) {
	         cout << getname() << " u_read() TIMEOUT !!!  " << dec << waitcnt << endl;
	         return -1;
	     }
	     usleep(100000);
	     //usleep(1000000);
	     waitcnt++;
        }
   }
   cout << getname() << " U-read() unknown ERROR  " << dec << waitcnt << endl;
   return -1;

}

// returns result of write to socket
int CommLinkObject::writeMessageToHost(messageObject & msg)
{
// by feel9 
   unsigned int	totalMessage[TO_HOST_MSG_LEN/sizeof(int)];
   int		retval;
   int		msgSize;


   // get the message into the local union
   msg.getMessage(&totalMessage,sizeof(totalMessage));
   msgSize = msg.getMsgSize();

   cout << getname()
        << "CommLinkObject::writeMessageToHost() "
        << "about to write a " << dec << msg.getMsgSize() 
	<< " byte message to the host. OPCODE =[0x" << hex << msg.getOpcode() << "]"
	<< endl;

#ifdef DEBUG
   cout << "[00]:" << hex << totalMessage[0] << " "
        << "[04]:" << totalMessage[1] << " "
        << "[08]:" << totalMessage[2] << " "
        << "[12]:" << totalMessage[3] << " "
        << "[16]:" << totalMessage[4] << " "
        << "[20]:" << totalMessage[5] << " " << endl;
   cout << "[24]:" << totalMessage[6] << " "
        << "[28]:" << totalMessage[7] << " "
        << "[32]:" << totalMessage[8] << " "
        << "[36]:" << totalMessage[9] << " " << endl;
#endif

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSmessageHeader	*msgheader = (WSmessageHeader *)totalMessage;
   msgheader->opcode = htons(msgheader->opcode); 
   msgheader->numberOfSomething = htons(msgheader->numberOfSomething); 
   msgheader->mspbStatus = htons(msgheader->mspbStatus); 
   msgheader->disabilityIndex = htonl(msgheader->disabilityIndex); 
   msgheader->messageSize = htons(msgheader->messageSize); 
  #endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // send the message to the host directly, without
   //putting it on the message queue
   // Only 1K messages can be accepted by the Concurrent server
   if(TO_HOST_MSG_LEN < msgSize)
   {
   msgSize = TO_HOST_MSG_LEN;
   cout << getname()
        << "CommLinkObject::writeMessageToHost() "
        << " processing message larger than " << dec << TO_HOST_MSG_LEN 
        << endl     
        << "  Only " << TO_HOST_MSG_LEN << " bytes will be sent to the host."
        << endl;
   }
// by feel9
//   if ((retval = write(WriteSocket, totalMessage, msgSize)) < 0)
//   {
//      cout << "writeMessageToHost: Error sending message "
//           << "back to host\n";
//   }
//#ifdef DEBEG

         printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SND(to CCUR)... opcode [%04x]\n", msg.getOpcode());
         cout << "Successfully sent " << dec << msgSize 
             << " message to socket " << hex << WriteSocket 
	     << endl;
//#endif
#ifdef DEBUG
   else
   {
       cout << "writeMessageToHost: Sent " << dec << msgSize << " message "
            << "with opcode = " << hex << msg.getOpcode() << " back to host\n";
   }
#endif
	 retval = 1;
   return retval;

}





//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
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
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


