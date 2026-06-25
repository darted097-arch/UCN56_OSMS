//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sendMessageToHost.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module sends messages from the GUI to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>


#include <sys/ipc.h>
#include <sys/msg.h>

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "messageQueueObject.h"
#include "messageObject.h"
#include "interface.h"
//
// External Variables:
//
extern  INTERFACE       *Interface;     // shm/CCUR to GUI interface vars

extern	messageQueueObject	WS_MsgQue;	// GUI to CommLink message queue object

//
// Local Constant Definitions:
//
#ifndef DEBUG
   #define DEBUG
#endif
//
// Function Prototypes:
//
#include "prototypes.h"
#include "parseHDSRMessages.h"
//
// send to message queue
//
// ***************************************************************************
//
int32 sendMessageToHost(int32 numArgs, void *data)
{
   int32 theSize;			// the size of the input
   int32 theType;			// the input variable type
   void *theData;			// parsed version of the data   
   int32 theOpcode;			// the submitted opcode
   int32 status;			// return status code

   struct tm   *current;		// current date and time structure
   time_t	seconds;		// seconds since 1/1/70
   
   messageObject	HostMsg(TO_HOST_MSG_ID, "HostMsg");


// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Start of executable code.
#ifdef DEBUG
   cout << "\n***** sendMessageToHost *****\n";
#endif
   
   // Extract the data, type and size   
   theData = PfGetFuncArg(&data, &theType, &theSize);
   
   // Extract the Opcode   
   theOpcode = *(int32*)theData;
   
   // get the current time for error reporting and also for the message header
   time(&seconds);
   current = localtime(&seconds);
   
   // if it's not an int, log it but continue
   if ( theType != PfCInt || theSize != 1 || numArgs != 1)
   {

      cerr      << " Bad Input to sendMessageToHost"
	        << "theType = " << theType << " theSize = " << theSize << endl;
   }

   cout      << "****************************************************" << endl;
   cout      << "sendMessageToHost: Processing Opcode [" << hex << theOpcode << "]" << endl;  
   cout      << "****************************************************" << endl;
   
   // set the Header portion of the message with the opcode, date & time
   HostMsg.setHeader(theOpcode, TO_HOST_MSG_LEN);

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<
  
//	 GRAPH_VIEW_ACT	viewAct;
	 
//	 char cDiskProtect[5], cResolut[5];
//	 char cStartDate[15], cStartTime[15], cEndDate[15], cEndTime[15];
//	 char cDiskDate_1[15], cDiskDate_2[15];
//	 char  cYear[5], cMon[5], cDay[5], cHour[5], cMin[5], cSec[5];
//	 char pString[15], tmpString[20];
  
   switch (theOpcode)
   {
// Thlee still does not use his own parse function ...... 
// instead of that, directly set message data
//--------------------------------------------------------------------------------
 
      case HDSR_INIT:
      case HDSR_PAGE_710:
      case HDSR_DISK_PROTECT:
      case HDSR_PAGE_720:
      case HDSR_BACKUP:
      case HDSR_PAGE_730:
      case HDSR_CANCEL_LOG:
      case HDSR_TAPE_HEADER:
      case HDSR_LOG:		// check validate inputs and request HDSR log	 
      case HDSR_PAGE_740:
      case HDSR_GRAPH_VIEW:
      case HDSR_GRAPH_SETUP:
      case TAPE_HEADER_74X:
      case GRAPH_SETUP_74X:
      case GRAPH_VIEW_74X:
      case GRAPH_VIEW_LEFT:	 
      case GRAPH_VIEW_RIGHT:
      case GRAPH_VIEW_UP:
      case GRAPH_VIEW_DOWN:
      case HDSR_SECTOR:
      case GRAPH_VIEW_DONE:
      case HDSR_PAGE_750:
      case TAPE_HEADER_750:
         parseHDSRMessage_send(HostMsg);
	 break;
	 
//--------------------------------------------------------------------------------
      case PROCESS_CHGSCN_DATA:
      case PROCESS_CHGALM_DATA:
      case PROCESS_CHGLIM_DATA:
      case PROCESS_INSVAL_DATA:
      case PROCESS_SENCAL_DATA:
      case PROCESS_LSF_DATA:
      case REQUEST_LSF_SCOUNT:
#ifdef DEBUG
	 cout << " Received DATABASE CHANGE MESSAGE\n";
#endif
	 parseDBC_Messages(HostMsg);
	 break;

      case REQUEST_PEN_DATA: 
      case PROCESS_PEN_DATA:
#ifdef DEBUG
	 cout << " Received PEN_RECORDER_MESSAGE" << endl;
#endif
	 parsePenMessages(HostMsg);
 	 break;

      case REQUEST_CAL_DATA:
      case REQUEST_CAL_NEXT_DATA:
      case REQUEST_CAL_PREV_DATA:
      case CAL_PAGE_OFF:
#ifdef DEBUG
         cout << " Received request to send CAL_MESSAGE" << endl;
#endif
         parseCALMessages(HostMsg);
         break;

      case REQUEST_BDL_DATA:
      case REQUEST_BDL_NEXT_DATA:
      case REQUEST_BDL_PREV_DATA:
      case BDL_PAGE_OFF:
#ifdef DEBUG
         cout << " Received request to send BDL_MESSAGE" << endl;
#endif
         parseBDLMessages(HostMsg);
         break;

      case REQUEST_ORL_DATA:
      case REQUEST_ORL_NEXT_DATA:
      case REQUEST_ORL_PREV_DATA:
      case ORL_PAGE_OFF:
#ifdef DEBUG
         cout << " Received request to send ORL_MESSAGE" << endl;
#endif
         parseORLMessages(HostMsg);
         break;

      case REQUEST_LINK_STATUS:
      case LINK_PAGE_OFF:
#ifdef DEBUG
         cout << " Received request to send Link Status" << endl;
#endif
         parseLinkMessages(HostMsg);
         break;
	 
      case REQUEST_ALARM_RESET:
      case REQUEST_ALARM_ACK:
#ifdef DEBUG
         cout << " sendMessageToHost : Alarm Ack-Reset Message, opcode= " << hex << theOpcode  << endl;
#endif
         // no Interface->AckResetData data now, just Opcode
         // parseAckResetMessages(HostMsg);
         break;

      case REQUEST_BAR_DATA:
      case PROCESS_BAR_DATA:
#ifdef DEBUG
         cout << " Received BAR_CHART_MESSAGE ... "<< endl;
#endif
         parseBarMessages(HostMsg);
         break;

      case REQUEST_DTREND_DATA:
#ifdef DEBUG
         cout << " sendMessageToHost : REQUEST_DTREND_DATA Message, opcode= " << hex << theOpcode << endl;
#endif
         parseDTrendMessages(HostMsg);
         break;

      case PROCESS_DTREND_DATA:
#ifdef DEBUG
         cout << " sendMessageToHost : PROCESS_DTREND_DATA Message, opcode= " << hex << theOpcode << endl;
#endif
         parseDTrendMessages(HostMsg);
         break;

      case RESET_DTREND_REQUEST:
#ifdef DEBUG
         cout << " sendMessageToHost : RESET_DTREND_REQUEST Message, opcode= " << hex << theOpcode << endl;
#endif
         parseDTrendMessages(HostMsg);
         break;

      case REQUEST_COLSS_PROCESSING:
#ifdef DEBUG
         cout << " Received COLSS_MESSAGE" << endl;
#endif
         parseColssMessages(HostMsg);
         break;

      case XRBP_REQUEST:
      case XRBP_INIT:
#ifdef DEBUG
         cout << " Received XRBP_MESSAGES " << theOpcode << endl;
#endif
         parseXRBPMessages(HostMsg);
         break;

      case XRBPA_UP_START:
      case XRBPA_UP_STOP:
#ifdef DEBUG
         cout << " Received XRBP_AUTO Messgaes: " << theOpcode <<endl;
#endif
         parseXRBPAMessages(HostMsg);
         break;

// ******** Single Point Display *******
      case DISPLAY_SINGLE_POINT: 
      case PRINT_SINGLE_POINT:
#ifdef DEBUG
	 cout << " Received Single Point Message" << endl;
#endif
   	 parseSinglePointMessages(HostMsg);
 	 break;

      case PRINT_SUMMARY_MESSAGE:
#ifdef DEBUG
         cout << " data option ==" << Interface->PRTSUM_REQUEST_MESSAGE.data.Option << endl;
#endif
         parsePRTSUMMessages(HostMsg);
         break;

      case GROUP_INFO_REQUEST:
      case GROUP_UPDATE:
      case GROUP_RESET_REQUEST:
         parseGroupMessages(HostMsg);
         break;

// this portion still valid ???? ... kysohn 
      case GET_HDSR_TREND1_DATA:
      case GET_HDSR_TREND2_DATA:
      case GET_HDSR_TREND3_DATA:
      case GET_HDSR_TREND4_DATA:
      case PROCESS_HDSR_TREND1_DATA:
      case PROCESS_HDSR_TREND2_DATA:
      case PROCESS_HDSR_TREND3_DATA:
      case PROCESS_HDSR_TREND4_DATA:
#ifdef DEBUG	 
	 cout << " Received HDSR MESSAGE" << endl;
#endif
	 // Process all HDSR messages here	    
	 // parseHDSRMessages(HostMsg);	
 	 break;

      case CEA_ALM_LIMIT_REQUEST:
      case ONE_CEA_POSITION_REPT:
      case ONE_CEA_GROUP_POSITION_REPT:
      case ALL_CEAGROUP_POSITION_REPT:
      case CEA_DROPNO_REQUEST:
      case CEDMCS_DATAXFER_MODE:
      case RPCCEA_MODE_REQUEST:
      case RPCCEA_PROCESS_REQUEST:
         // Process RPCCEA messages
         parseRPCCEAMessages(HostMsg);
         break;

      case INCORE_PROCESS_REQUEST:
      case INCORE_CHARGE_REQUEST:
      case INCORE_CHARGE_UPDATE:
#ifdef DEBUG	 
	 cout << " Received INCORE_.. MESSAGE" << endl;
#endif
         //Process Incore messages
         parseIncoreMessages(HostMsg);
         break;

      case PROCESS_SECURE_DATA:
         parseSecureMessages(HostMsg);
         break;

      case REQUEST_BOP_PRINT:
         parseBOPDMDMessages(HostMsg);
         break;

      case PROCESS_UTDV_SEND:
#ifdef DEBUG
         cout << "****SEND THE UTDV MESSAGES TO CCUR****" << endl;
#endif
         // Process all UTDV messages here
         parseUTDVMessages(HostMsg);
         break;

      case REQUEST_PRREPT:
#ifdef DEBUG	 
         cout <<" I received REQUEST_PRREPT message, opcode= " << hex << theOpcode << endl;
#endif
         parsePrreptMessages(HostMsg);
         break;
           
      case REQUEST_POSTRIP_PRINT:
#ifdef DEBUG	 
         cout <<" I received REQUEST_POSTRIP_PRINT message, opcode= " << hex << theOpcode << endl;
#endif
         parsePrreptMessages(HostMsg);
         break;

      case REQUEST_SOE_PRINT:
#ifdef DEBUG	 
         cout <<" I received REQUEST_SOE_PRINT message, opcode= " << hex << theOpcode << endl;
#endif
         parsePrreptMessages(HostMsg);
         break;

      case REQUEST_CECOR_SNAPSHOT:
#ifdef DEBUG	 
         cout <<" I received REQUEST_CECOR_SNAPSHOT message, opcode= " << hex << theOpcode << endl;
#endif
         parseCecorMessages(HostMsg);
         break;

      case REQUEST_DSPM_REPORT:
#ifdef DEBUG	 
         cout <<" I received REQUEST_DSPM_REPORT message, opcode= " << hex << theOpcode << endl;
#endif
         parseDspmMessages(HostMsg);
         break;

      case REQUEST_CSL_DISPLAY:
         cout << " REQUEST_CSL_DISPLAY message, opcode= " << hex << theOpcode << endl;
         parseCslMessages(HostMsg);
         break;

      case EXIT_CSL_DISPLAY:
         cout <<" EXIT_CSL_DISPLAY message, opcode= " << hex << theOpcode << endl;
         parseCslMessages(HostMsg);
         break;
         
      case REQUEST_VTREND_DATA:
      case PROCESS_VTREND_DATA:
      case VTREND_RESET_REQUEST:
#ifdef DEBUG	 
         cout << " Received Video Trend Messages " << endl;
#endif
         parseVTRENDMessages(HostMsg);
         break;
         
         // Process all Rcs Leak messages here.(by stc)
// CORENET ujson add 2017.0221 : case REQUEST_MASS_CALC:
      case REQUEST_RCS_LEAK_DATA:
      case REQUEST_LEAK_OFF_DATA:
      case REQUEST_LEAKPRN:  
      case REQUEST_MASS_CALC:
      case REQUEST_MASS_PRN:
      case REQUEST_MASS_INIT:
#ifdef DEBUG	 
         cout << " Received Rcs Leak Messages " << endl;
#endif   
         parseRcsLeakMessages(HostMsg);
         break;   

// CORENET ujson add 2017.0223
      case REQUEST_PIDTEST:
	 parsePidForTestMessages(HostMsg);
	 break;

      case REQUEST_EQUIPVALCHG:
      case REQUEST_EQUIPSTATUS:
      case REQUEST_EQUIPPRN:
	 parseEquipmentMessages(HostMsg);
	 break;
// CORENET ujson add end


        //CEA TRACK MONITOR message // 20061116

      case REQUEST_CEA_TRACK_MONITOR1:
      case REQUEST_CEA_TRACK_MONITOR2:
      case REQUEST_CEA_TRACK_MONITOR3:
      case REQUEST_CEA_TRACK_MONITOR4:
      case REQUEST_CEA_TRACK_MONITOR5:
      case REQUEST_CEA_TRACK_MONITOR6:
      case REQUEST_CEA_TRACK_MONITOR9:
           parseCEATrackMonitorMessages(HostMsg);
#ifdef DEBUG	 
           cout<<"send:REQUEST_CEA_TRACK_MONITOR" << endl;
#endif
           break;

      default:
	 cout << " Received Unknown Message: " << hex << theOpcode << endl;
	 // Do not send invalid messages
	 flush(cout);
	 return (!OK);
	 
   } // end of opcode switch statement

   cout << "sendMessageToHost theOpcode = " << hex << theOpcode << endl;
      
   // send all messages to CommLink.  WS_MsgQue is the message queue 
   //read by CommLink.
   status = WS_MsgQue.sendMessage(HostMsg.pMsg, HostMsg.getMsgSize(),
			          HostMsg.getType());
   if (0 != status)
      cout << " sendMessageToHost ERROR: Unsucessful at putting message on "
	   << "CommLink message queue\n";
//#ifdef DEBUG
   else
      cout << " Sucessfully put message on CommLink message queue\n";
//#endif

   // set flag for data sent
   Interface->System.sentData = TRUE;
   Interface->System.receivedData = FALSE;
    
   flush(cout); //ensure all messages are written to console.     
   return (OK);
}//end sendMessageToHost
