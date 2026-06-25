#ifndef	_COMMLINKOBJECT_H
#define	_COMMLINKOBJECT_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// CommLinkObject.h
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// This file defines the CommLinkObject class.
// ***************************************************************************
//
// System Include Files:
//
#include <netinet/in.h>
#include <iostream.h>
#include <map>				// by feel9
//
// User Defined Include Files:
//
#include "Typedefs.h"
#include "systemParameters.h"
#include "messageObject.h"

#include "ucn56Parameters.h"
#include "ucn56Dynamic.h"
//
// External Variables:
//
//
//
// Local Constant Definitions:
//
#define COMMLINK_ERROR_FILE	"CommLink.err"	// name of error file

// hostName[] in interfaceStructure.pdat must be dimensioned with the same 
//value as NUM_HOSTS, i.e., 2.
const int	NUM_HOSTS	= 2;		// number of hosts
#define HOST_NAME_A "ygn6c1"                // Primary host name
#define HOST_NAME_B "ygn6c2"                // Secondary host name

const u_port_t	LISTEN_PORT	= 50;	// Well known port on CCUR
const time_t	PACKET_TIMEOUT	= 30;	// seconds before datalink timeout
const time_t	FAILOVER_TIMEOUT= -60;	// on Failover, host time difference
                                        // that will restart workstation S/W

const long	SLEEPTIME	= 20000;// microseconds to wait between reads
const int	HOST_TRY_LIMIT	= 1;	// # of tries before switching hosts
const int       HOST_RETRY_WAIT	= 5;	// seconds to wait before retrying host
const int	HOST_CONNECT_WAIT = 60; // seconds to wait after retrys exhausted
					//before trying to connect again.
const int	HOSTREAD_TRY_LIMIT	= 1;	// # tries for reads from host
          //seconds to wait before retrying read
const int	HOSTREAD_RETRY_WAIT	= 2;	

const int	CNCT_TRY_LIMIT	= 10;	// # of tries to connect()
const int	CNCT_RETRY_WAIT	= 3;	// seconds to wait between connect tries

const int	MAX_COMMLINK_NAME_LEN = 20;	// Len of CommLink Process name

const int	PRIMARY_COMMLINK = 1;
const int	BACKUP_COMMLINK = -1;
const int	UNKNOWN_COMMLINK = 0;

const int	SLAVE_CPU = 0x00FF;
const int	MASTER_CPU = 1;
const int	UNKNOWN_CPU = 0;

const int 	totalrcvTagIDNum = 6901;		// by feel9

// From Client
const ubyte DBDOWNLOADERROR		= 0x01;
const ubyte DBDOWNLOADREQ		= 0x02;
//const ubyte DBDOWNLOADCOMPLED		= 0x03;
const ubyte DBUPDATEREADY		= 0x03;

// From Server
const ubyte DBDOWNLOADING		= 0x11;
const ubyte DBDYNDATADOWNLOADING	= 0x12;
const ubyte DBDOWNLOADHEARTBEAT		= 0x13;
const ubyte FILEDOWNLOADING		= 0x14;

   // Buffers for local processing
typedef union _Local {
    WSmessageHeader		header;
    WSmessageTemplate		message;   
    WSresponseMessage		response;
    char			char_buf[WS_MESSAGE_LENGTH];
    unsigned int 		totalMessage[WS_MESSAGE_LENGTH/sizeof(int)];
};

//ucn56 EOF

const uint32 NALL		= 20000; // 20201212
const uint32 NOOFRECORDS	= NALL;

// Erds Message Header
typedef struct {
    ubyte   opcode;	        // service type
    ubyte   status;	        // SvrStatus_e or error number (0 : OK)
    uint16  numberOfSomething;	// #bytes, #points, etc 
    uint32  messageLength;	// in bytes 
    uint32  dbRevNo;		// database revision number
    uint32  epochSeconds;	// Epoch time 
} ErdsMessageHeader;

// The complete main database download reply from server
typedef struct {
    ErdsMessageHeader	header;	// message information
    U56_RECORD 		data[NOOFRECORDS];	// db records
} ErdsDbDownloadReply;

// The dynamic database update response from server
typedef struct {
   ErdsMessageHeader	header;		// message information
   DYNAMIC_RECORD	data[NOOFRECORDS];	// Dynamic data structure
} ErdsDbDynamicDataReply;

// The SOE or Alarm data download from server
typedef struct {
   ErdsMessageHeader	header;		// message information
   char *		pFileData;	// SOE|Alarm file data structure
} ErdsFileDataReply;

typedef struct {
   U56_RECORD	DbPoint[NALL]; // 20201212
   DYNAMIC_RECORD DynPoint[NALL]; // 20201212
} DATA;

// ucn 56 


//
// Function Prototypes:
//
class CommLinkObject
{
public:
    CommLinkObject();		// constructor
   ~CommLinkObject();		// destructor
   static void	Connect();	// high level connect, no return until connected
   static void	mainLoop();
   static void  setWriteSocket(); // connect to EOFEOF  host
   //260325 by kkuk
   static void  reconnectSocket();
   static void  flushSocketBuffer(int fd);
   
   // ucn56 EOF
   static bool CommFunc();	//parse ucn56 EOF DATA from rcvbuffer
   static int recvData(int fd, void *data, unsigned int len);
   static int recvSvcData(int fd, void *data, unsigned int len);
   static bool processDbDownLoading (ErdsMessageHeader *pHeader);
   static bool processDynDownLoading (ErdsMessageHeader *pHeader);
   static bool processFileDownloading (ErdsMessageHeader *pHeader);
   static u_short Checksum(u_char *packet, int nSize);
   static void allDb_ntohlh(unsigned int dbSize);
   static void dynDb_ntohlh(unsigned int dynSize);
   static float ntohf(uint32 netfloat);
   static void ucn56DBUpdate();
   static void UpdateDynamicDb(unsigned int dynSize);

	 // by feel9 
	 static void	Connect_old();
	 static int		connectWSToHost_old();	// connect to host
	 static int		u_connect_old(u_port_t port, char * hostName);
	 static ssize_t	u_read_old(int fd, char *buf, size_t size);
	 static void		parseHostMessages_old();	// parse messages from host
	 static void parseTagID(char *buf, int buflen);	//parse TagID from rcvbuffer
	 static bool parseEOF();	//parse EOF DATA from rcvbuffer
	 static bool WritePIDataFile(char *Date, char *Time);	//Saving File
	 static float RealToFloat(char *chReal);					//Real -> float
   //static void		ws2ccurlink_old();		// process messages to host
   //static void		closeAndReconnect_old();	// close sockets, init vars,

// Access methods

   static void 	setnum(int v){num = v;};
   static void	setname(char * n){strncpy(name, n, MAX_COMMLINK_NAME_LEN);};
   static void	sethostName(char * n){strncpy(hostName, n, MAX_HOSTNAME_LEN);};
   static void  setconnected(){connected = TRUE;};
   static void  resetconnected(){connected = FALSE;};
   static void  setportNum(char *n) {strncpy(portNum, n, MAX_PORTNO_LEN);};
   static void  setsendUse(char * n){strncpy(sendUse, n, 5);};
   
   static int	getnum(){return num;};
   static char *getname(){return name;};
   static char *gethostName(){return hostName;};
   static char *getportNum() {return portNum;};
   static char *getsendUse(){return sendUse;}; 
   static int	isconnected(){return connected;};


private:
   static int		connectWSToHost();	// connect to host
   static void		initCommLink();
   static void		parseHostMessages();	// parse messages from host
   static void		ws2ccurlink();		// process messages to host
   static void		closeAndReconnect();	// close sockets, init vars,
						// reconnect.

   static int	writeMessageToHost(messageObject & msg); // directly to host
   
   // connect to host, low level
   static int		u_connect(u_port_t port, char * hostName);
   static int		u_ignore_sigpipe();	// ignore pipe write errors

   // enhanced read function 
   static ssize_t	u_read(int fd, char *buf, size_t size);

   static _Local Local;

   static int   bytes_read;
   static int   total_bytes_read;
   
   static int	msgSize;
   static int	gotMsgSize;
   static int	bytesToRead;

   static time_t lastMessageTime; // time of the last host message
   static int 	ReadSocket;	// TCP/IP socket for reading
   static int	WriteSocket;	// TCP/IP socket for writing
   
   static int	num;		// Which CommLink this is, 1 or 2
   static char	name[MAX_COMMLINK_NAME_LEN]; // Process name of this CommLink
   static char	hostName[MAX_HOSTNAME_LEN]; // Name of host connected to
   static int	connected;	// TRUE means connected to host
   static time_t	timeOfLastRead; // time datawas last read form host
   static char  portNum[MAX_PORTNO_LEN]; // portNo of host connected to
   static char  sendUse[5];
   static int   isDbDown;
};
#endif
