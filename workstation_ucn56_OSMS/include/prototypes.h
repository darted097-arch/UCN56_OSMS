#ifndef _PROTOTYPES_H
#define _PROTOTYPES_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// prototypes.h         
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This file contains all the function prototypes.  Prototypes for system 
// utilities are not included here.  Prototypes for objects are found in the 
// object header, i.e., ".h" files.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
#include <sys/msg.h>
//
// User Defined Include Files:
//
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
// None
//
// Prototypes
//
//
// Application Prototypes
//
// InterfaceToGUI Prototypes
//
void	connectToGUI(int status, const char * msg);	// Initialize rtm
int	cyclicProcessing(int id);	// Runs each cycle to update database
void	disconnectFromGUI(int status, const char * msg);// called when rtm goes down
time_t	getLocalTime();	// converts local time to seconds since EPOCH
void	initInterfaceToGUI();

#ifdef _DATA_QUEUE_OBJECT_H
//void	parsePictureMessages();
void	parsePictureMessages(DataQueueObject & DQ);
#endif

// parse message Prototypes
#ifdef _MESSAGE_OBJECT_H
void	parseSecureMessages(messageObject & msg);
void	parseBarMessages(messageObject & msg);
void	parseRPCCEAMessages(messageObject & msg);
void	parseIncoreMessages(messageObject & msg);
void	parseBOPDMDMessages(messageObject & msg);
void	parseGroupMessages(messageObject & msg);
void	parsePRTSUMMessages(messageObject & msg);
void	parsePrreptMessages(messageObject & msg);
//void	parseHDSRMessages(messageObject & msg);
void	parseDBUpdateMessages(messageObject & msg);
void    parseSVUpdateMessages(messageObject & msg);
void	parseSinglePointMessages(messageObject & msg);
void	parsePenMessages(messageObject & msg);	
void	parseDBLoadMessages(messageObject & msg);
void	parseDBC_Messages(messageObject & msg);
void    parseUTDVMessages(messageObject & msg);
void    parseXRBPMessages(messageObject & msg);            // parse XRBP Messages
void    parseXRBPAMessages(messageObject & msg);           // parse XRBPAUTO Messages
void    parseCecorMessages(messageObject & msg);           // parse the Cecor message
void    parseDTrendMessages(messageObject & msg);     // parse the digital trend messages **jklee**
void    parseDspmMessages(messageObject & msg);            // parse DSPM message
void    parseCslMessages(messageObject & msg);             // parse CSL message
void    parseColssMessages(messageObject & msg);           // fire bear done
void    process_ColssMessageConfirmed(messageObject & msg);// fire bear done
void    parseVTRENDMessages(messageObject & msg);     // parse the video trend messages
void    parseCALMessages(messageObject & msg);             // parse CAL messages
void    parseCPCSMessages(messageObject & msg);             // parse CPCS messages
void    parseAckResetMessages(messageObject & msg);	   // parse Alarm Ack-Reset messages
void    parseBDLMessages(messageObject & msg);             // parse BDL messaged
void    parseORLMessages(messageObject & msg);             // parse ORL messages
void    parseLinkMessages(messageObject & msg);             // parse link status display messages
void    parseRcsLeakMessages(messageObject & msg);          // parse RCS LEAK messages
void    parsePidForTestMessages(messageObject & msg);          // parse PID For Test messages
void    parseEquipmentMessages(messageObject & msg);          // parse EQUIPMENT messages
void    parseCEATrackMonitorMessages(messageObject & msg); // parse CEA Track Display messages

#endif

int	find_pid (char *pid);			// finds ipn of a point-id

// determine and set Primary/Backup statuses of the two CommLinks based on 
//their connection status and whether they are connected to the master or 
//slave host.
void	setPBstatuses();	

#endif
