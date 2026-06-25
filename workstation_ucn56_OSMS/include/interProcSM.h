#ifndef _INTERPROCSM
#define _INTERPROCSM
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// interProcSM.h        
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// This structure contains shared memory variables that are not used in 
// the GUI.  This data will be in a shared memory segment.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files: NONE
//
//
// User Defined Include Files: NONE
//
#include "systemParameters.h"
#include "CommLinkObject.h"
#include "DBCommLinkObject.h"
//
// External Variables: NONE
//
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
//
// There is one HOST structure for each CommLink that contains useful
//information about each connection. 
struct  HOST {
   char 	hostName[MAX_HOSTNAME_LEN];	// Name of host
   int  	hostNum;	// Number of host, not necessarily CPU number.
				//1 = host 1, which corresponds to CommLink 1
				//2 = host 2, which corresponds to CommLink 2
   int		cpuNum;		// from message headers
   int  	connected;	// 1 = connected, 0 = not connected to host
   int  	failoverState;	// bigger is better state
   int		masterStatus;	// -1 slave, 0 unknown, 1 master
   int		primaryStatus;	// -1 backup, 0 unknown, 1 primary
   time_t	timeOfLastRead;	// time data was last read from host
   int		numTicksOfLastRead;	// number of ticks on last read
   char		portNo[MAX_PORTNO_LEN];  // PortNo of host
};

struct  interProc {
   int  pidDBWSPCS;             // Process id  ujson add
   int  pidDBCommLink1;         // Process id  ujson add
   int  pidDBCommLink2;         // Process id  ujson add
   HOST	DBhosts[NUM_HOSTS];	// host connection information

   int  pidCommLink1;           // Process id
   int  pidCommLink2;           // Process id
   int  pidRTM;                 // Process id
   int  pidControl;             // Process id
   int  pidInterfaceToGUI;      // Process id
   int  pidFDDIstart;           // Process id
   int  pidTicker;           	// Process id
   int  pidTimer;           	// Process id
   int  pidWSPCS;               // Process id of parent of all procs.
   int	receivedDBdownload;	// TRUE if database has been downloaded 
				//from server
   int	primaryLocked;		// TRUE means primary host is locked, 
				//not determined via algorithm
   HOST	hosts[NUM_HOSTS];	// host connection information
   int  numTicks;               // number of ticks since starting WSPCS
   char MODEM[MODEM_OPTION_LEN+1];
   char SEND[5];             // 1 = SEND, 0 = NO SEND
};

#endif
