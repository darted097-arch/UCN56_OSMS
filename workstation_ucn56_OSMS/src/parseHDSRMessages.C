//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseHDSRMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  This module processes the HDSR messages. These functions are 
//  Change Scan, Change Alarm, Change Limits, Insert Value, Sensor Calibration,
//  and Least Squares Fit.  This includes messages from the host to the 
//  workstation, and messages from the workstation to the host.
// **************************************************************************************
//
// **************************************************************************************
//
// System Include Files:
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"
#include "database.h"
#include "DatabaseObject.h"

//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	ofstream	ErrorFile;	// Logs all Errors
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue
extern	DatabaseObject	DBO;

//
// Local Constant Definitions:
//
//
// Function Prototypes: NONE
//
#include "parseHDSRMessages.h"
#include "prototypes.h"

//
// **************************************************************************************
//

//*****************************************************************************
//
//	Send Messages for HDSR to CCUR
// 
void parseHDSRMessage_send(messageObject &HostMsg)
{

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	HDSR_730SENDMSG * hdsr730_snddata_ptr;
	HDSR_74XSENDMSG * hdsr74x_snddata_ptr;
	GRAPH_VIEW_ACT * graph_view_act_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 GRAPH_VIEW_ACT	viewAct;
	 
	 char cDiskProtect[5], cResolut[5];
	 int   iDiskProtect, iResolut;
	 char cStartDate[15], cStartTime[15], cEndDate[15], cEndTime[15];
	 char cDiskDate_1[15], cDiskDate_2[15];
	 int  iStartDate, iStartTime;
	 char  cYear[5], cMon[5], cDay[5], cHour[5], cMin[5], cSec[5];
	 char pString[15], tmpString[20];
	 int graphSet;
	 
	 //cout << "parseHDSRMessages_send() :   " << hex << HostMsg.getOpcode() << endl;

   switch (HostMsg.getOpcode())
   {
      case HDSR_INIT:
	 break;
	 
//--------------------------------------------------------------------------------
//  	 Disk Protection
//
      case HDSR_PAGE_710:
	break;
	
      case HDSR_DISK_PROTECT:
      // make message for disk protection ( 0 - OFF, 1 - ON )
      
	 tGetCharRtm( "gPage710", "newDiskProtect", cDiskProtect);
	 iDiskProtect = atoi(cDiskProtect);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 iDiskProtect = htonl(iDiskProtect);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>

	 HostMsg.setData((void *)&iDiskProtect, sizeof(int));

	 break;
	 
//-------------------------------------------------------------------------------------
//	 HDSR Backup 
//	 
      case HDSR_PAGE_720:
	// request some data from host
	 break;
	 
      case HDSR_BACKUP:
	 tGetCharRtm( "gPage720", "resolution", cResolut);
	 if( strcmp( cResolut, "L") == 0 )
	    iResolut = 2;
	 else if( strcmp( cResolut, "H") == 0 )
	    iResolut = 1;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 iResolut = htonl(iResolut);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>

	 HostMsg.setData((void *)&iResolut, sizeof(int));
//	 
//	 tPutIntRtm( "gFooter", "hiLoBackupRequest", 1);
	 break;

//---------------------------------------------------------------------------------------
//	Logging
//
      case HDSR_PAGE_730:
      	// request init data from CCUR
	 break;
	 
      case HDSR_CANCEL_LOG:
	 // request cancel HDSR log
	 break;
	 
      case HDSR_TAPE_HEADER:
	 // request tape header information
	 break;
	 
      case HDSR_LOG:		
      	// make message to request HDSR log	
      	 
   	 HDSR_730SENDMSG send730;

	 // datasource		Disk - 1, Tape - 2
	 tGetCharRtm("gPage730", "dataSource", tmpString);
	 if( strcmp( tmpString, "D") == 0 )
		send730.dataSource = 1;
	 else if( strcmp( tmpString, "T") ==  0 )
	  	send730.dataSource = 2;
	 
	 // resolution		High - 1, Low - 2
	 tGetCharRtm("gPage730", "resolution", tmpString);
	 if( strcmp(tmpString, "H") == 0 )
		send730.resolution = 1;
	 else if( strcmp( tmpString, "L") ==  0 )
	   	send730.resolution = 2;	
	   	 
	 // time step
	 tGetCharRtm("gPage730", "timeStep", tmpString);
	 send730.timeStep = atoi( tmpString);
	 
	// startDate, startTime, endDate, and endTime	    
	 tGetCharRtm("gPage730", "startDate", cStartDate);
	 tGetCharRtm("gPage730", "startTime", cStartTime);
	 tGetCharRtm("gPage730", "endDate", cEndDate);
	 tGetCharRtm("gPage730", "endTime", cEndTime);
	 
         tConvertToCcString( send730.startDate, cStartDate, 8);
	 tConvertToCcString( send730.startTime, cStartTime, 8);
	 tConvertToCcString( send730.endDate, cEndDate, 8);
	 tConvertToCcString( send730.endTime, cEndTime, 8);	 
	       
	// fill points 
	 tGetCharRtm("gPage730", "point_1", pString);
	 tConvertToCcString( send730.point_1, pString, 8);
	 tGetCharRtm("gPage730", "point_2", pString);
	 tConvertToCcString( send730.point_2, pString, 8);
	 tGetCharRtm("gPage730", "point_3", pString);
	 tConvertToCcString( send730.point_3, pString, 8);
	 tGetCharRtm("gPage730", "point_4", pString);
	 tConvertToCcString( send730.point_4, pString, 8);
	 tGetCharRtm("gPage730", "point_5", pString);
	 tConvertToCcString( send730.point_5, pString, 8);
	 tGetCharRtm("gPage730", "point_6", pString);
	 tConvertToCcString( send730.point_6, pString, 8);
	 tGetCharRtm("gPage730", "point_7", pString);
	 tConvertToCcString( send730.point_7, pString, 8);
	 tGetCharRtm("gPage730", "point_8", pString);
	 tConvertToCcString( send730.point_8, pString, 8);
	 
	 HostMsg.setData((void *)&send730, sizeof(HDSR_730SENDMSG));

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	HDSR_730SENDMSG * hdsr730_snddata_ptr;
	hdsr730_snddata_ptr = (HDSR_730SENDMSG *)HostMsg.getDataPtr();
	hdsr730_snddata_ptr->dataSource = htonl(hdsr730_snddata_ptr->dataSource);
	hdsr730_snddata_ptr->resolution = htonl(hdsr730_snddata_ptr->resolution);
	hdsr730_snddata_ptr->timeStep = htonl(hdsr730_snddata_ptr->timeStep);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 
//------------------------------------------------------------------------------------	 
//	Graphic Trend
//
      case HDSR_GRAPH_VIEW:
      // request graphic trend view
      
         tPutIntRtm( "gView_x", "displayTrendNum", 0 );
               
   	 PfExecute( NULL, "{ DisplayAdvisoryMessage1(\"PROCESSING...  WAIT\"); }");
	 
	 graphSet = tGetIntRtm( "gView_x", "whichView");
	// cout << "------------   "  << graphSet << endl;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 graphSet = htonl(graphSet);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 HostMsg.setData((void *)&graphSet, sizeof(int));
	 break;
	 
      case HDSR_GRAPH_SETUP:
      // request graphic trend setup
      
      	// send graphic set no.
	 tGetCharRtm( "gPage740", "graphSet", tmpString);
	 graphSet = atoi(tmpString);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 graphSet = htonl(graphSet);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 HostMsg.setData((void *)&graphSet, sizeof(int));
	 
      case HDSR_PAGE_740:         
	 break;
//------------------------------------------------------------------------------------
//	Graphic Trend Setup	 
	 
      case TAPE_HEADER_74X:
	 // request Tape Header information
	 break;
	 
      case GRAPH_SETUP_74X:
      	// send setup information
      	
   	 HDSR_74XSENDMSG send74x;
   	 
   	 // graph Set ( 1 - 8 )
	 send74x.graphSet = tGetIntRtm("gPage74x", "graphSet");

	 // datasource
	 tGetCharRtm("gPage74x", "dataSource", tmpString);
	 if( strcmp( tmpString, "D") == 0 )
		send74x.dataSource = 1;
	 else if( strcmp( tmpString, "T") ==  0 )
	  	send74x.dataSource = 2;
	 
	 // resolution
	 tGetCharRtm("gPage74x", "resolution", tmpString);
	 if( strcmp(tmpString, "H") == 0 )		// High resolution
		send74x.resolution = 1;
	 else if( strcmp( tmpString, "L") ==  0 )	// Low resolution
	   	send74x.resolution = 2;	 
	 else if( strcmp( tmpString, "H1") ==  0 )	// High resolution 0-16
	   	send74x.resolution = 3;	 
	 else if( strcmp( tmpString, "H2") ==  0 )	// High resolution 8-24
	   	send74x.resolution = 4;	 
	 
	 // check and request start date/time and end date/time
	 if( send74x.resolution == 2 ) {
		 tGetCharRtm("gPage74x", "startDate", cStartDate);
		 iStartDate = tGetDate( cStartDate); 
	 
  		 tGetCharRtm("gPage74x", "startTime", cStartTime);
		 iStartTime = tGetTime( cStartTime); 
		 
		 if( iStartDate == -1 || iStartTime == -1) {
		    PfExecute(NULL, "{ DisplayAdvisoryMessage(\"INVALID FORMAT FOR START DATE\"); }");
		    return;
		 }
		 else {
		    tConvertToCcString(send74x.startDate, cStartDate, 8);	
		    tConvertToCcString(send74x.startTime, cStartTime, 8);	
		 }   	 		 
	 }
	 
	 // pack point IDs
	 tGetCharRtm("gPage74x", "point_1", pString);
	 // cout << "-----" << pString << "*****" << endl;
	 tConvertToCcString( send74x.point_1, pString, 8);
	 tGetCharRtm("gPage74x", "point_2", pString);
	 tConvertToCcString( send74x.point_2, pString, 8);
	 tGetCharRtm("gPage74x", "point_3", pString);
	 tConvertToCcString( send74x.point_3, pString, 8);
	 tGetCharRtm("gPage74x", "point_4", pString);
	 tConvertToCcString( send74x.point_4, pString, 8);
	 
	 // pack minimum values for points
	 tGetCharRtm("gPage74x", "minValue_1", pString);
	 send74x.minValue_1 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "minValue_2", pString);
	 send74x.minValue_2 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "minValue_3", pString);
	 send74x.minValue_3 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "minValue_4", pString);
	 send74x.minValue_4 = (float )atof(pString);
	 
	 // pack baseline values for points
	 tGetCharRtm("gPage74x", "baseline_1", pString);
	 send74x.baseline_1 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "baseline_2", pString);
	 send74x.baseline_2 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "baseline_3", pString);
	 send74x.baseline_3 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "baseline_4", pString);
	 send74x.baseline_4 = (float )atof(pString);
	 
	 // pack maximum values for points
	 tGetCharRtm("gPage74x", "maxValue_1", pString);
	 send74x.maxValue_1 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "maxValue_2", pString);
	 send74x.maxValue_2 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "maxValue_3", pString);
	 send74x.maxValue_3 = (float )atof(pString);
	 tGetCharRtm("gPage74x", "maxValue_4", pString);
	 send74x.maxValue_4 = (float )atof(pString);
	 
	 HostMsg.setData((void *)&send74x, sizeof(HDSR_74XSENDMSG));	 

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	hdsr74x_snddata_ptr = (HDSR_74XSENDMSG *)HostMsg.getDataPtr();
	hdsr74x_snddata_ptr->graphSet = htonl(hdsr74x_snddata_ptr->graphSet);
	hdsr74x_snddata_ptr->dataSource = htonl(hdsr74x_snddata_ptr->dataSource);
	hdsr74x_snddata_ptr->resolution = htonl(hdsr74x_snddata_ptr->resolution);
	swap32((char *)&(hdsr74x_snddata_ptr->minValue_1));
	swap32((char *)&(hdsr74x_snddata_ptr->minValue_2));
	swap32((char *)&(hdsr74x_snddata_ptr->minValue_3));
	swap32((char *)&(hdsr74x_snddata_ptr->minValue_4));
	swap32((char *)&(hdsr74x_snddata_ptr->baseline_1));
	swap32((char *)&(hdsr74x_snddata_ptr->baseline_2));
	swap32((char *)&(hdsr74x_snddata_ptr->baseline_3));
	swap32((char *)&(hdsr74x_snddata_ptr->baseline_4));
	swap32((char *)&(hdsr74x_snddata_ptr->maxValue_1));
	swap32((char *)&(hdsr74x_snddata_ptr->maxValue_2));
	swap32((char *)&(hdsr74x_snddata_ptr->maxValue_3));
	swap32((char *)&(hdsr74x_snddata_ptr->maxValue_4));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 
//------------------------------------------------------------------------------------
//	Graphic Trend View	 
	 
      case GRAPH_VIEW_74X:
      // request init graphic trens view
      
      	// set level and sector no.
      	 viewAct.level = 1;
      	 viewAct.sector = 1;
      	 	 
	 HostMsg.setData((void *)&viewAct, sizeof(GRAPH_VIEW_ACT));	 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	graph_view_act_ptr = (GRAPH_VIEW_ACT *)HostMsg.getDataPtr();
	graph_view_act_ptr->level = htonl(graph_view_act_ptr->level);
	graph_view_act_ptr->sector = htonl(graph_view_act_ptr->sector);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 
      case GRAPH_VIEW_LEFT:	 
      case GRAPH_VIEW_RIGHT:
      case GRAPH_VIEW_UP:
      case GRAPH_VIEW_DOWN:
      case HDSR_SECTOR:
      // perform to left, right, up, down, and sector down 
      
      	 viewAct.level = tGetIntRtm("gView_x", "level");
      	 viewAct.sector = tGetIntRtm("gView_x", "sector");
      	 	 
	 HostMsg.setData((void *)&viewAct, sizeof(GRAPH_VIEW_ACT));	 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	graph_view_act_ptr = (GRAPH_VIEW_ACT *)HostMsg.getDataPtr();
	graph_view_act_ptr->level = htonl(graph_view_act_ptr->level);
	graph_view_act_ptr->sector = htonl(graph_view_act_ptr->sector);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 
      case GRAPH_VIEW_DONE:
	 break;
	 
//----------------------------------------------------------------
//	Tape Information
//
      case HDSR_PAGE_750:
	 break;
      case TAPE_HEADER_750:
      // request tape information
	 break;
	 	 
   } // end of opcode switch statement
   
   return;
}

//*****************************************************************************
//
//	Process received Messages for HDSR from CCUR
// 
int parseHDSRMessage_receive( messageObject &Msg)
{
	unsigned int rawMessage[WS_MESSAGE_LENGTH/4];
	TAPE_HEADER_RECEIVEMSG 	ccTapeHeader;
	unsigned int ccOpcode;
	int numBuffer;
	char cProtect[5], resolut[5];
	char pString[10], vString[10], vdString[40], sdateString[20], edateString[20];
	char msgString_1[50], msgString_2[50], msgString_3[50], msgString[30];
	char tmpString[10], string_1[10], string_2[10];
	int useView[8], displayTrendNum;
	int iMove, iDisk, iResource, iType;
	int i, x;
//	int iTemp;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..	
#ifdef LINUX_APP
	HDSR_710RECEIVEMSG * hdsr710_rcvdata_ptr;
	GRAPH_INT_SETUP * graph_int_setup_ptr;
	GRAPH_VIEW_RECEIVEMSG * graph_view_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 cout << "parseHDSRMessages_recieve() :   " << hex << Msg.getOpcode() << endl;
	
	char iString[5];
	ccOpcode = Msg.getOpcode() + 0x1000;
	
	switch( ccOpcode) {
	   case HDSR_INIT:
	   	break;
	
//--------------------------------------------------------------------------------
//  	 Disk Protection
//	receive information for disk protection from CCUR and display it.
//
	   case HDSR_PAGE_710:			
	   case HDSR_DISK_PROTECT:
	      HDSR_710RECEIVEMSG 	ccPage710;
	      
	      numBuffer = sizeof( HDSR_710RECEIVEMSG);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	      hdsr710_rcvdata_ptr = (HDSR_710RECEIVEMSG *)Msg.getDataPtr();
	      hdsr710_rcvdata_ptr->diskProtect = ntohl(hdsr710_rcvdata_ptr->diskProtect);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	      Msg.getData(&ccPage710, numBuffer);
		
	      // disk protection		 
	      sprintf( iString,"%d", ccPage710.diskProtect);
	      tPutCharRtm( "gPage710", "currentDiskProtect", iString);
		
	      // disk protection from date/time
	      tStrncpy( tmpString, ccPage710.fromDate, 8);	// convert CC string to WS string
	      tPutCharRtm( "gPage710", "fromProtectDate", tmpString);	// display a picture
	      tStrncpy( tmpString, ccPage710.fromTime, 8);
	      tPutCharRtm( "gPage710", "fromProtectTime", tmpString);
              
              // disk protection to date/time
	      tStrncpy( tmpString, ccPage710.toDate, 8);
	      tPutCharRtm( "gPage710", "toProtectDate", tmpString);
	      tStrncpy( tmpString, ccPage710.toTime, 8);
	      tPutCharRtm( "gPage710", "toProtectTime", tmpString);
     
	      PfFlush();   
	      break;

//-------------------------------------------------------------------------------------
//	 HDSR Backup 
//	receive information for hi/low backup from CCUR and display it.
	 
	   case HDSR_PAGE_720:
	   	HDSR_720RECEIVEMSG	ccPage720;
	   	
	      numBuffer = sizeof( HDSR_720RECEIVEMSG );
	      Msg.getData(&ccPage720, numBuffer);

	      // high resolution backup date/time	      	      
	      tStrncpy( tmpString, ccPage720.hiBackupDate, 8);
	      tPutCharRtm( "gPage720", "hiBackupDate", tmpString);
	      tStrncpy( tmpString, ccPage720.hiBackupTime, 8);
	      tPutCharRtm( "gPage720", "hiBackupTime", tmpString);

	      // low resolution backup date/time	      	      	      
	      tStrncpy( tmpString, ccPage720.loBackupDate, 8);
	      tPutCharRtm( "gPage720", "lowBackupDate", tmpString);	      
	      tStrncpy( tmpString, ccPage720.loBackupTime, 8);
	      tPutCharRtm( "gPage720", "lowBackupTime", tmpString);
	      
	      PfFlush();   	   	
		break;

	   case HDSR_BACKUP:
		break;

//---------------------------------------------------------------------------------------
//	Initial Information for Logging and Graphic View Steup 
//
	   case HDSR_PAGE_730:
	   case HDSR_GRAPH_SETUP:
	   
	      HDSR_730RECEIVEMSG 	ccPage730;
	      
	      numBuffer = sizeof( HDSR_730RECEIVEMSG);
	      Msg.getData(&ccPage730, numBuffer);
	      
	      // initial information for logging
	      if( ccOpcode == HDSR_PAGE_730 ) {
	      	   
	          // start date/time for the high resolution in disk
		  tStrncpy( tmpString, ccPage730.highDiskDate_1, 8);
	          tPutCharRtm( "gPage730", "highDiskDate_1", tmpString);
   	          tStrncpy( tmpString, ccPage730.highDiskTime_1, 8);
	          tPutCharRtm( "gPage730", "highDiskTime_1", tmpString);

	          // end date/time for the high resolution in disk
	          tStrncpy( tmpString, ccPage730.highDiskDate_2, 8);
	          tPutCharRtm( "gPage730", "highDiskDate_2", tmpString);
	          tStrncpy( tmpString, ccPage730.highDiskTime_2, 8);
	          tPutCharRtm( "gPage730", "highDiskTime_2", tmpString);

	          // start date/time for the low resolution in disk
	          tStrncpy( tmpString, ccPage730.lowDiskDate_1, 8);
	          tPutCharRtm( "gPage730", "lowDiskDate_1", tmpString);
	          tStrncpy( tmpString, ccPage730.lowDiskTime_1, 8);
	          tPutCharRtm( "gPage730", "lowDiskTime_1", tmpString);

	          // end date/time for the low resolution in disk
	          tStrncpy( tmpString, ccPage730.lowDiskDate_2, 8);
	          tPutCharRtm( "gPage730", "lowDiskDate_2", tmpString);
	          tStrncpy( tmpString, ccPage730.lowDiskTime_2, 8);
	         tPutCharRtm( "gPage730", "lowDiskTime_2", tmpString);	         
	      PfFlush(); 
	         
	         // Is displyed ?
  //    		 iTemp = tGetIntRtm("gPage730", "enablePage");
  //    	 	if( iTemp == 0 ) {
  	         // display logging setup page 
//		}
	      }
	      
	      // initial information for Graphic trend setup
	      else if( ccOpcode == HDSR_GRAPH_SETUP ) {
	      
	          // start date/time for the high resolution in disk
		    tStrncpy( tmpString, ccPage730.highDiskDate_1, 8);
	 	    tPutCharRtm( "gPage74x", "highDiskDate_1", tmpString);	      
	  	    tStrncpy( tmpString, ccPage730.highDiskTime_1, 8);
	 	    tPutCharRtm( "gPage74x", "highDiskTime_1", tmpString);

	          // end date/time for the high resolution in disk
		    tStrncpy( tmpString, ccPage730.highDiskDate_2, 8);
		    tPutCharRtm( "gPage74x", "highDiskDate_2", tmpString);
		    tStrncpy( tmpString, ccPage730.highDiskTime_2, 8);
	            tPutCharRtm( "gPage74x", "highDiskTime_2", tmpString);

	          // start date/time for the low resolution in disk
	            tStrncpy( tmpString, ccPage730.lowDiskDate_1, 8);
	            tPutCharRtm( "gPage74x", "lowDiskDate_1", tmpString);
	            tStrncpy( tmpString, ccPage730.lowDiskTime_1, 8);
	            tPutCharRtm( "gPage74x", "lowDiskTime_1", tmpString);

	          // end date/time for the low resolution in disk
	            tStrncpy( tmpString, ccPage730.lowDiskDate_2, 8);
	            tPutCharRtm( "gPage74x", "lowDiskDate_2", tmpString);
	            tStrncpy( tmpString, ccPage730.lowDiskTime_2, 8);
	            tPutCharRtm( "gPage74x", "lowDiskTime_2", tmpString);
	           
	           PfFlush(); 
	      }

	      break;
	      
	   case HDSR_CANCEL_LOG:	      
	      break;
	      
	   case HDSR_LOG:
	      break;
	      
//------------------------------------------------------------------------------------	 
//	Graphic Trend
//
	   case HDSR_PAGE_740:
	   
   	      GRAPH_INT_SETUP intSetup;
   		 
	      numBuffer = sizeof( GRAPH_INT_SETUP );
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	graph_int_setup_ptr = (GRAPH_INT_SETUP *)Msg.getDataPtr();
	graph_int_setup_ptr->dataSource = ntohl(graph_int_setup_ptr->dataSource);
	graph_int_setup_ptr->resolution = ntohl(graph_int_setup_ptr->resolution);
	swap32((char *) &(graph_int_setup_ptr->minValue_1));
	swap32((char *) &(graph_int_setup_ptr->minValue_2));
	swap32((char *) &(graph_int_setup_ptr->minValue_3));
	swap32((char *) &(graph_int_setup_ptr->minValue_4));
	swap32((char *) &(graph_int_setup_ptr->baseline_1));
	swap32((char *) &(graph_int_setup_ptr->baseline_2));
	swap32((char *) &(graph_int_setup_ptr->baseline_3));
	swap32((char *) &(graph_int_setup_ptr->baseline_4));
	swap32((char *) &(graph_int_setup_ptr->maxValue_1));
	swap32((char *) &(graph_int_setup_ptr->maxValue_2));
	swap32((char *) &(graph_int_setup_ptr->maxValue_3));
	swap32((char *) &(graph_int_setup_ptr->maxValue_4));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	      Msg.getData(&intSetup, numBuffer);
	      
	      if( intSetup.dataSource == 1 )
	      	tPutCharRtm( "gPage74x", "dataSource", "D");
	      else
	      	tPutCharRtm( "gPage74x", "dataSource", "T");
	      	
	      if( intSetup.resolution == 1 )
	      	tPutCharRtm( "gPage74x", "resolution", "H");
	      else if( intSetup.resolution == 2 )
	      	tPutCharRtm( "gPage74x", "resolution", "L");
	      else if( intSetup.resolution == 3 )
	      	tPutCharRtm( "gPage74x", "resolution", "H1");
	      else if( intSetup.resolution == 4 )
	      	tPutCharRtm( "gPage74x", "resolution", "H2");

	// cout << intSetup.dataSource << "  " << intSetup.resolution << endl;
	
		tStrncpy( vString, intSetup.point_1, 8);
		tPutCharRtm( "gPage74x", "point_1", vString);
		tStrncpy( vString, intSetup.point_2, 8);
		tPutCharRtm( "gPage74x", "point_2", vString);
		tStrncpy( vString, intSetup.point_3, 8);
		tPutCharRtm( "gPage74x", "point_3", vString);
		tStrncpy( vString, intSetup.point_4, 8);
		tPutCharRtm( "gPage74x", "point_4", vString);
		      
		sprintf( vString, "%7.1f", intSetup.minValue_1);
		tPutCharRtm( "gPage74x", "minValue_1", vString);
		sprintf( vString, "%7.1f", intSetup.minValue_2);
		tPutCharRtm( "gPage74x", "minValue_2", vString);
		sprintf( vString, "%7.1f", intSetup.minValue_3);
		tPutCharRtm( "gPage74x", "minValue_3", vString);
		sprintf( vString, "%7.1f", intSetup.minValue_4);
		tPutCharRtm( "gPage74x", "minValue_4", vString);
		         
		sprintf( vString, "%7.1f", intSetup.maxValue_1);
		tPutCharRtm( "gPage74x", "maxValue_1", vString);
		sprintf( vString, "%7.1f", intSetup.maxValue_2);
		tPutCharRtm( "gPage74x", "maxValue_2", vString);
		sprintf( vString, "%7.1f", intSetup.maxValue_3);
		tPutCharRtm( "gPage74x", "maxValue_3", vString);
		sprintf( vString, "%7.1f", intSetup.maxValue_4);
		tPutCharRtm( "gPage74x", "maxValue_4", vString);
		      
		sprintf( vString, "%7.1f", intSetup.baseline_1);
		tPutCharRtm( "gPage74x", "baseline_1", vString);
		sprintf( vString, "%7.1f", intSetup.baseline_2);
		tPutCharRtm( "gPage74x", "baseline_2", vString);
		sprintf( vString, "%7.1f", intSetup.baseline_3);
		tPutCharRtm( "gPage74x", "baseline_3", vString);
		sprintf( vString, "%7.1f", intSetup.baseline_4);
		tPutCharRtm( "gPage74x", "baseline_4", vString);
	      PfFlush(); 
	      
		// display setup page after receiving information fron CCUR	        	   	
		PfExecute( NULL, "{ DisplayPage(\"PicUCN_HDSR74Xsetup.ppic\"); }");
	      break;
	      
//-------------------------------------------------------------------	      
//	      
	   case HDSR_GRAPH_VIEW:
	      GRAPH_VIEW_RECEIVEMSG 	graphView;
	      RECORD *pRecord;
	      
	      numBuffer = sizeof( GRAPH_VIEW_RECEIVEMSG);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	      graph_view_ptr = (GRAPH_VIEW_RECEIVEMSG *)Msg.getDataPtr();
      	      graph_view_ptr->graphSet = ntohl(graph_view_ptr->graphSet);
      	      graph_view_ptr->noPoint = ntohl(graph_view_ptr->noPoint);
	      graph_view_ptr->pointType = ntohl(graph_view_ptr->pointType);
	      swap32((char *) &(graph_view_ptr->minimum));
	      swap32((char *) &(graph_view_ptr->baseline));
	      swap32((char *) &(graph_view_ptr->maximum));
	      for (i = 0; i < 256; i++)
	      	 swap32((char *) &(graph_view_ptr->value.fv[i]));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	      Msg.getData(&graphView, numBuffer);
	      
	      if( graphView.pointType > 200 ) {
	      	iResource = 2;
	      	graphView.pointType -= 200;
		tPutCharRtm( "gView_x", "resource", "TAPE");
	      }
	      else {
	      	iResource = 1;
	      	graphView.pointType -= 100;
		tPutCharRtm( "gView_x", "resource", "DISK");
	      }
	      if( graphView.pointType > 20 ) {
	      	iDisk = 2;
	      	graphView.pointType -= 20;
		tPutCharRtm( "gView_x", "resolution", "LOW");
	      }
	      else {
	      	iDisk = 1;
	      	graphView.pointType -= 10;
		tPutCharRtm( "gView_x", "resolution", "HIGH");
	      }
	      iType = graphView.pointType;
	      
	      
	      tPutIntRtm( "gView_x", "whichView", graphView.graphSet);


              switch( graphView.noPoint ) {	      
                  case 1:
		      tPutFloatRtm( "gView_x", "maximum_1", graphView.maximum);
		      tPutFloatRtm( "gView_x", "minimum_1", graphView.minimum);
		      tPutFloatRtm( "gView_x", "baseline_1", graphView.baseline);
       	              tPutIntRtm( "gView_x", "pointType_1", iType);
	      	      
		      tStrncpy( vString, graphView.pointID, 8);
		      tPutCharRtm( "gView_x", "pointID_1", vString);
		      // cout << "***   Point ID  " << vString << endl;
		      
		      // delete space character of string
		      for( i=0; i < 8; i++ )
		      	if( vString[i] == ' ' )
		      		vString[i] = '\0';
		      		
		      if( (x=find_pid(vString)) != -1 ) {
		      	pRecord = (RECORD *) DBO.getDBrecptr(x);
		   	   tPutCharRtm( "gView_x", "onScale_1", pRecord->message1);
		   	   tPutCharRtm( "gView_x", "offScale_1", pRecord->message2);
		      }
		      else 
		      cout << "parseHDSRMessage(): Can't find " << vString << endl;
		      
 		      tStrncpy( vdString, graphView.description, 32);
 		      tPutCharRtm( "gView_x", "description_1", vdString);
 		      // cout << "Descrition   " << vdString << endl;	
                     break;
		  
                  case 2:
		      tPutFloatRtm( "gView_x", "maximum_2", graphView.maximum);
		      tPutFloatRtm( "gView_x", "minimum_2", graphView.minimum);
		      tPutFloatRtm( "gView_x", "baseline_2", graphView.baseline);
	              tPutIntRtm( "gView_x", "pointType_2", iType);
	      	      
		      tStrncpy( vString, graphView.pointID, 8);
		      tPutCharRtm( "gView_x", "pointID_2", vString);
		      // cout << "***   Point ID  " << vString << endl;
		      
		      // delete space character of string
		      for( i=0; i < 8; i++ )
		      	if( vString[i] == ' ' )
		      		vString[i] = '\0';
		      		
		      if( (x=find_pid(vString)) != -1 ) {
		      	pRecord = (RECORD *) DBO.getDBrecptr(x);
		   	   tPutCharRtm( "gView_x", "onScale_2", pRecord->message1);
		   	   tPutCharRtm( "gView_x", "offScale_2", pRecord->message2);
		      }
		      else 
		      cout << "parseHDSRMessage(): Can't find " << vString << endl;
		      
 		      tStrncpy( vdString, graphView.description, 32);
 		      tPutCharRtm( "gView_x", "description_2", vdString);
 		      // cout << "Descrition   " << vdString << endl;	
                     break;
		  
                  case 3:
		      tPutFloatRtm( "gView_x", "maximum_3", graphView.maximum);
		      tPutFloatRtm( "gView_x", "minimum_3", graphView.minimum);
		      tPutFloatRtm( "gView_x", "baseline_3", graphView.baseline);
	              tPutIntRtm( "gView_x", "pointType_3", iType);
	      	      
		      tStrncpy( vString, graphView.pointID, 8);
		      tPutCharRtm( "gView_x", "pointID_3", vString);
		      // cout << "***   Point ID  " << vString << endl;
		      
		      // delete space character of string
		      for( i=0; i < 8; i++ )
		      	if( vString[i] == ' ' )
		      		vString[i] = '\0';
		      		
		      if( (x=find_pid(vString)) != -1 ) {
		      	pRecord = (RECORD *) DBO.getDBrecptr(x);
		   	   tPutCharRtm( "gView_x", "onScale_3", pRecord->message1);
		   	   tPutCharRtm( "gView_x", "offScale_3", pRecord->message2);
		      }
		      else 
		      cout << "parseHDSRMessage(): Can't find " << vString << endl;
		      
 		      tStrncpy( vdString, graphView.description, 32);
 		      tPutCharRtm( "gView_x", "description_3", vdString);
 		      // cout << "Descrition   " << vdString << endl;	
                     break;
		  
                  case 4:
		      tPutFloatRtm( "gView_x", "maximum_4", graphView.maximum);
		      tPutFloatRtm( "gView_x", "minimum_4", graphView.minimum);
		      tPutFloatRtm( "gView_x", "baseline_4", graphView.baseline);
	   	      tPutIntRtm( "gView_x", "pointType_4", iType);
	      	      
		      tStrncpy( vString, graphView.pointID, 8);
		      tPutCharRtm( "gView_x", "pointID_4", vString);
		      // cout << "***   Point ID  " << vString << endl;
		      
		      // delete space character of string
		      for( i=0; i < 8; i++ )
		      	if( vString[i] == ' ' )
		      		vString[i] = '\0';
		      		
		      if( (x=find_pid(vString)) != -1 ) {
		      	pRecord = (RECORD *) DBO.getDBrecptr(x);
		   	   tPutCharRtm( "gView_x", "onScale_4", pRecord->message1);
		   	   tPutCharRtm( "gView_x", "offScale_4", pRecord->message2);
		      }
		      else 
		      cout << "parseHDSRMessage(): Can't find " << vString << endl;
		      
 		      tStrncpy( vdString, graphView.description, 32);
 		      tPutCharRtm( "gView_x", "description_4", vdString);
 		      // cout << "Descrition   " << vdString << endl;	
                     break;
              }
	      PfFlush();   	   	
              
	      if( graphView.pointType == 2 ) {	// digital point
	             sprintf( pString,"iValue_%d", graphView.noPoint);
		     tPutIArrayRtm( "gView_x", pString, 256, graphView.value.iv);		     
	      }
	      else {				// analaog point
	             sprintf( pString,"values_%d", graphView.noPoint);
		     tPutFArrayRtm( "gView_x", pString, 256, graphView.value.fv);
	      	}
  
		      tStrncpy( vString, graphView.time_1, 8);
		      tPutCharRtm( "gView_x", "time_1", vString);
		      // cout << "time_1   " << vString << endl;
		      tStrncpy( vString, graphView.time_2, 8);
		      tPutCharRtm( "gView_x", "time_2", vString);
		      tStrncpy( vString, graphView.time_3, 8);
		      tPutCharRtm( "gView_x", "time_3", vString);
		      tStrncpy( vString, graphView.time_4, 8);
		      tPutCharRtm( "gView_x", "time_4", vString);
		      tStrncpy( vString, graphView.time_5, 8);
		      tPutCharRtm( "gView_x", "time_5", vString);

		      tStrncpy( string_1, graphView.sDate, 8);
		      tStrncpy( string_2, graphView.sTime, 8);
		      sprintf( sdateString, "%s  %s", string_1, string_2);
		      tPutCharRtm( "gView_x", "startDate", sdateString);
	      
		      tStrncpy( string_1, graphView.eDate, 8);
		      tStrncpy( string_2, graphView.eTime, 8);
		      sprintf( edateString, "%s  %s", string_1, string_2);
		      tPutCharRtm( "gView_x", "endDate", edateString);
		      strcpy( edateString, "");

         	tPutIntRtm( "gView_x", "actMask", 0);		// reset act mask

	//  Display HDSR trends	 
		displayTrendNum = tGetIntRtm( "gView_x", "displayTrendNum");  
                tPutIntRtm( "gView_x", "displayTrendNum", displayTrendNum+1 );
      		   
               	iMove = tGetIntRtm( "gView_x", "actNo");
		PfExecute( NULL, "{ DisplayHDSRTrend(%d); }", graphView.noPoint);
		PfExecute( NULL, "{ DisplayAdvisoryMessage1(\"CORENET COMPLETED TRACK #%d TREND\"); }", 
		                  graphView.noPoint);	        

	      break;
	      
    	  case GRAPH_VIEW_74X:
    	  // confirm using graph set
               	tPutIntRtm( "gView_x", "useView", 1);		// use a view
         	tPutIntRtm( "gView_x", "actMask", 0);		// reset act mask
	      break;
	 
    	  case GRAPH_VIEW_LEFT:		 
  	  case GRAPH_VIEW_RIGHT:
 	  case GRAPH_VIEW_UP:
      	  case GRAPH_VIEW_DOWN:
      	  case GRAPH_VIEW_DONE:
      	  case HDSR_SECTOR:
	 	break;
	 
//--------------------------------------------------------------------------------
//	      
	   case GRAPH_SETUP_74X:
	      struct SETUP_INFO {
	      		int setNo;
	      		int done;
	      	      } setupInfo;
	      	      
	      numBuffer = sizeof(SETUP_INFO);
	      Msg.getData(&setupInfo, numBuffer);
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	      setupInfo.setNo = ntohl(setupInfo.setNo);
	      setupInfo.done = ntohl(setupInfo.done);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	      // cout << "GRAPH_SETUP_74X:   " << setupInfo.setNo << endl;
	      
	// inform to finish graph setup
	      tPutIarrayRtm( "gPage740", "setupDone", setupInfo.setNo - 1, setupInfo.done);
	      break;
	      
//--------------------------------------------------------------------------------
	   case HDSR_TAPE_HEADER:		// on page 730	      
	   case TAPE_HEADER_74X:		// on page 740
	   case HDSR_PAGE_750:
  	   case TAPE_HEADER_750:
	      
	      numBuffer = sizeof( TAPE_HEADER_RECEIVEMSG);
	      Msg.getData(&ccTapeHeader, numBuffer);
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>o
#ifdef LINUX_APP
		ccTapeHeader.resolution = ntohl(ccTapeHeader.resolution);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	      tPutIntRtm( "gPage750", "iResolution", ccTapeHeader.resolution);
	      if( ccTapeHeader.resolution == 1 ) 		// high-1, low-2
		  tPutCharRtm( "gPage750", "resolution", "HIGH");
	      else  
		  tPutCharRtm( "gPage750", "resolution", "LOW");
		 
	      tStrncpy( tmpString, ccTapeHeader.startDate, 8);
	      tPutCharRtm( "gPage750", "startDate", tmpString);
	      tStrncpy( tmpString, ccTapeHeader.startTime, 8);
	      tPutCharRtm( "gPage750", "startTime", tmpString);

	      tStrncpy( tmpString, ccTapeHeader.endDate, 8);
	      tPutCharRtm( "gPage750", "endDate", tmpString);
	      tStrncpy( tmpString, ccTapeHeader.endTime, 8);
	      tPutCharRtm( "gPage750", "endTime", tmpString);
	      
	      if( ccTapeHeader.resolution == 2 ) {
		      tStrncpy( tmpString, ccTapeHeader.weekDate[0], 8);
		      tPutCharRtm( "gPage750", "wDate_1", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[1], 8);
		      tPutCharRtm( "gPage750", "wDate_2", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[2], 8);
		      tPutCharRtm( "gPage750", "wDate_3", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[3], 8);
		      tPutCharRtm( "gPage750", "wDate_4", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[4], 8);
		      tPutCharRtm( "gPage750", "wDate_5", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[5], 8);
		      tPutCharRtm( "gPage750", "wDate_6", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.weekDate[6], 8);
		      tPutCharRtm( "gPage750", "wDate_7", tmpString);
	      }
	      
	      if( ccOpcode == HDSR_TAPE_HEADER ) {

		      tStrncpy( tmpString, ccTapeHeader.startDate, 8);
		      tPutCharRtm( "gPage730", "validStartDate", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.startTime, 8);
		      tPutCharRtm( "gPage730", "validStartTime", tmpString);

		      tStrncpy( tmpString, ccTapeHeader.endDate, 8);
		      tPutCharRtm( "gPage730", "validEndDate", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.endTime, 8);
		      tPutCharRtm( "gPage730", "validEndTime", tmpString);
	      }
	      else if( ccOpcode == TAPE_HEADER_74X ) {

		      tStrncpy( tmpString, ccTapeHeader.startDate, 8);
		      tPutCharRtm( "gPage74x", "validStartDate", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.startTime, 8);
		      tPutCharRtm( "gPage74x", "validStartTime", tmpString);

		      tStrncpy( tmpString, ccTapeHeader.endDate, 8);
		      tPutCharRtm( "gPage74x", "validEndDate", tmpString);
		      tStrncpy( tmpString, ccTapeHeader.endTime, 8);
		      tPutCharRtm( "gPage74x", "validEndTime", tmpString);
	      }
	      
	      PfFlush();   	   	
	      break;
		 
    	   case HDSR_FOOTER_MESSAGE:
    	   	struct FOOTER_MESSAGE {
    	   			int 	diskProtect;
    	   		//	int	backup;
    	   		//	int 	newAlarm;
    	   		//	int	newBadData;
    	   		//	int	newCEA;
    	   		} footerMessage;
    	   		
    	   	numBuffer = sizeof( FOOTER_MESSAGE );
	      	Msg.getData(&footerMessage, numBuffer);
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>o
#ifdef LINUX_APP
		footerMessage.diskProtect = ntohl(footerMessage.diskProtect);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	  cout <<  footerMessage.diskProtect << "   " <<   footerMessage.backup << endl; 	
	      	//PfExecute(NULL, "{ TdisplayDiskProtect(%d); }", footerMessage.diskProtect);
	      	//PfExecute(NULL, "{ TdisplayBackupRequest(%d); }", footerMessage.backup);
	      	//PfExecute(NULL, "{ TdisplayAlarm(%d); }", footerMessage.newAlarm);
	      	//PfExecute(NULL, "{ TdisplayBadData(%d); }", footerMessage.newBadData);
	      	//PfExecute(NULL, "{ TdisplayCEA(%d); }", footerMessage.newCEA);
	      	
		// if disk protection is changed, change Page 710's current disk protection	      	
	      	if( footerMessage.diskProtect == 1 ) 
	      	    tPutCharRtm( "gPage710", "currentDiskProtect", "1");
	      	else 
	      	    tPutCharRtm( "gPage710", "currentDiskProtect", "2");
	      	
 		break;
 
    	   case DYN_MESSAGE:
    	   case DYN_MESSAGEr:	// CORENET 2012.07.10
    	   	DYNAMIC_MESSAGE dynMessage;
    	   	
    	   	numBuffer = sizeof( DYNAMIC_MESSAGE );
	      	Msg.getData(&dynMessage, numBuffer);
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>o
#ifdef LINUX_APP
		dynMessage.num = ntohl(dynMessage.num);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		switch( dynMessage.num ) {
			case 1:
			    tStrncpy( msgString_1, dynMessage.message, 48);
		            PfExecute(NULL, "{ DisplayAdvisoryMessage1(\"%s\"); }", msgString_1);
		cout << "Message_1   " << msgString_1 << endl;
		            break;
		
			case 2:
		 	    tStrncpy( msgString_2, dynMessage.message, 48);
		  	    PfExecute(NULL, "{ DisplayAdvisoryMessage2(\"%s\"); }", msgString_2);
		cout << "Message_2   " << msgString_2 << endl;
				break;
			case 3:
			    tStrncpy( msgString_3, dynMessage.message, 48);
			    PfExecute(NULL, "{ DisplayAdvisoryMessage3(\"%s\"); }", msgString_3);
		cout << "Message_3   " << msgString_3 << endl;
				break;
			case 5:
			    tStrncpy( msgString, dynMessage.message, 25);
			    PfExecute(NULL, "{ DisplayAdvisoryMessage(\"%s\"); }", msgString);
		cout << "Message   " << msgString << endl;
		}
    	         break; 
    	         
	   default:
	      // cout << "parseHDSRMessages_receive():  Unknown Message" << endl;
	      return -1;
	}
	return 0;
}


int tConvertToCcString( char *ccString, char *string, int num )
{
   int strLen, restLen, i, j;
   
   // cout << "tConvertToCcString()" << endl;
   strLen = strlen(string);
   
   restLen = num - strLen;
   if( restLen == 0 ) {
	for( i=0; i<num; i++ ) 
     	    *(ccString + i) = *(string +i);
   }
   else {
      for( i=0; i<strLen; i++)
     	    *(ccString + i) = *(string +i);
      for( j=i; j<num; j++ )
     	    *(ccString + j) = ' ';
   }
   return 0;
}

int tValidateDiskDate( char *iString,  char *sString, char *eString)
{
   char  cYear[5], cMon[5], cDay[5];
   int	iYear, iMon, iDay;    
   int	sYear, sMon, sDay, eYear, eMon, eDay;    
   int lDay[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    
   strcpy( cMon, strtok( iString, "\/"));
   iMon = atoi(cMon);
   strcpy( cDay, strtok( NULL, "\/"));
   iDay = atoi(cDay);
   strcpy( cYear, strtok( NULL, "\/"));
   iYear = atoi(cYear);
             
   strcpy( cMon, strtok( sString, "\/"));
   sMon = atoi(cMon);
   strcpy( cDay, strtok( NULL, "\/"));
   sDay = atoi(cDay);
   strcpy( cYear, strtok( NULL, "\/"));
   sYear = atoi(cYear);
    
   strcpy( cMon, strtok( eString, "\/"));
   eMon = atoi(cMon);
   strcpy( cDay, strtok( NULL, "\/"));
   eDay = atoi(cDay);
   strcpy( cYear, strtok( NULL, "\/"));
   eYear = atoi(cYear);
    
    if( sYear == eYear ) { 
       if( iYear != sYear ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED YEAR NOT AVAILABLE\");}");
	    return -1;
	}
    	else if( sMon == eMon ) {
	   if( iMon != sMon ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED MONTH NOT AVAILABLE\");}");
	    return -1;
	   }
	   
	   if( iDay < sDay || iDay > eDay ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED DAY NOT AVAILABLE\");}");
	    return -1;
	   }
	}
       else if( iMon == sMon ) {
	  if( iDay < sDay || iDay > lDay[sMon] ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED DAY NOT VAILD FOR EARLIEST AVAILABLE MONTH\");}");
	    return -1;
	   }	     
       }
       else if( iMon == eMon ) {
	  if( iDay < 1 || iDay > eDay ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED DAY NOT VAILD FOR LATEST AVAILABLE MONTH\");}");
	    return -1;
	   }	     
       }
       else {
	  if( iDay < 1 ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED DAY LESS THAN 1\");}");
	    return -1;
	  }
	  else if( iDay > lDay[iMon] ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"REQUESTED DAY GREATER THAN LAST DAY OF REQUESTED MONTH\");}");
	    return -1;
	   }
       }
    }
    else if( (eYear == (sYear+1) || sYear == 99 && eYear == 0 ) && (sMon == 12 && eMon == 1) ) {
       if( !(iYear == sYear || iYear == eYear) ) {
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"START YEAR NOT AVAILABLE\");}");
	    return -1;
	}
       else if( (iMon == 12 && (iDay < sDay || iDay > lDay[sMon])) || (iMon == 1 && (iDay < 1 || iDay > eDay)) ) { 
	    PfExecute(NULL, "{DisplayAdvisoryMessage(\"START DAY NOT AVAILABLE\");}");
	    return -1;
       }
    }
    else {
	PfExecute(NULL, "{DisplayAdvisoryMessage(\"INVALID DATE RANGE\");}");
       return -1;
    }	     
    return 0;  
}


int tGetDate( char *dString )
{
   int lDay[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
   char  cYear[5], cMon[5], cDay[5], tmpString[10];
   int	iYear, iMon, iDay, iDate;
   
   strcpy( cMon, strtok( dString, "\/"));
   iMon = atoi(cMon);
   strcpy( cDay, strtok( NULL, "\/"));
   iDay = atoi(cDay);
   strcpy( cYear, strtok( NULL, "\/"));
   iYear = atoi(cYear);

   // cout << "#####  " << iMon << "    " << iDay  << "    " << iYear << "  " << iYear % 4  << endl;
   if( (iYear % 4 ) == 0 )
      lDay[1] = 29;
   
   if( (iMon < 1 || iMon > 12) || (iDay < 1 || iDay > lDay[iMon-1] ) || ( iYear < 0 || iYear > 99 ) ) 
      return -1;      
  
      //  reform MM/DD/YY
      if( strlen(cMon) == 1 ) {		// month '5' -> '05'
      	strcpy( tmpString, "0");
      	strcat( tmpString, cMon);
      }
      else if( strlen(cMon) == 2 ) 
      	strcpy( tmpString, cMon);
      
      strcat( tmpString, "\/");
            
      if( strlen(cDay) == 1 ) { 		// day '6' -> '06'
	strcat( tmpString, "0");
      	strcat( tmpString, cDay);
      }		
      else if( strlen(cDay) == 2 )
      	strcat( tmpString, cDay);
      	
      strcat( tmpString, "\/");
      strcat( tmpString, cYear);
      
      strcpy( dString, tmpString);	
      	
      iDate = iYear * 12*31 + iMon * 31 + iDay;
      return iDate;
}
   
int tGetTime( char *tString )
{
   char cHour[5], cMin[5], cSec[5], tmpString[10];
   int	iHour, iMin, iSec, time;

   strcpy( cHour, strtok( tString, ":"));
   iHour = atoi(cHour);
   strcpy( cMin, strtok( NULL, ":"));
   iMin = atoi(cMin);
   strcpy( cSec, strtok( NULL, ":"));
   iSec = atoi(cSec);

   // cout << cHour << "   " << cMin << "   " << cSec << endl;
   // cout << iHour << "   " << iMin << "   " << iSec << endl;
   
   time = iHour * 3600 + iMin * 60 + iSec;

   // cout << " " << iHour << "  " << iMin << "  " << iSec << endl;
   if( (iHour < 0 || iHour > 23) || (iMin < 0 || iMin > 59) || (iSec < 0 || iSec > 59) )
      return -1;

   //  reform HH:MM:SS
      if( strlen(cHour) == 1 ) {		// month '5' -> '05'
      	strcpy( tmpString, "0");
      	strcat( tmpString, cHour);
      }
      else if( strlen(cHour) == 2 ) 
      	strcpy( tmpString, cHour);
      
      strcat( tmpString, ":");
            
      if( strlen(cMin) == 1 ) {			// day '6' -> '06'
        strcat( tmpString, "0");
      	strcat( tmpString, cMin);
      }
      else if( strlen(cMin) == 2 )
      	strcat( tmpString, cMin);
      	
      strcat( tmpString, ":");
      strcat( tmpString, cSec);
      
      strcpy( tString, tmpString);	
  
   return time;
}


int tConvertIntToDate( int iDate, char *p_cDate )
{
   int lDay[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
   DATE_BIT  *dateBit;   
   int year, month, day;
   char string[10];
   
   dateBit = (DATE_BIT *) &iDate;
   year = dateBit->yearBit;
   month = dateBit->monthBit;
   day = dateBit->dayBit;

   if( (year % 4 ) == 0 )
      lDay[1] = 29;
// cout << "tConvertIntToDate() :  " << dec << year << ",  " << month << ",  " << day << endl;   
   if( (month < 1 || month > 12) || (day < 1 || day > lDay[month-1] ) || ( year < 0 || year > 99 ) )
      return -1;

// make a form "mm/dd/yy" 
      if( month > 9 )
	 sprintf( p_cDate, "%d\/", month);
      else
         sprintf( p_cDate, "0%d\/", month);
      
      if( day > 9 )
	 sprintf( string, "%d\/", day);
      else
	 sprintf( string, "0%d\/", day);      
      strcat( p_cDate, string);
      
      if(year > 9 )
	 sprintf( string, "%d", year);
      else
         sprintf( string, "0%d",year);
      strcat( p_cDate, string);
      
   return 0;   
}

int tConvertIntToTime( int iTime, char *p_cTime )
{
	int iHour, iMin, iSec, iRest;
	char string[10];

	iHour = iTime / 3600;
	iRest = iTime % 3600;
	
	iMin = iRest / 60;
	iSec = iRest % 60;

	if( iHour < 0 || iHour > 23 || iMin < 0 || iMin > 59 || iSec < 0 || iSec > 59 )
		return -1;

	if( iHour > 9 )
	   sprintf( p_cTime, "%d:", iHour);
	else
	   sprintf( p_cTime, "0%d:", iHour);
	
	if( iMin > 9 )
	   sprintf( string, "%d:", iMin);
	else
	   sprintf( string, "0%d:", iMin);
	strcat( p_cTime, string);
	
	if( iSec > 9 )
	   sprintf( string, "%d", iSec);
	else
	   sprintf( string, "0%d", iSec);
	strcat( p_cTime, string);
	
	return 0;	
}

int tStrncpy( char *string, char *CCstring, int num )
{
	strncpy( string, CCstring, num );
	*(string + num ) = '\0';
	
	return 0;
}

int tPutCharRtm( char *rtmStruct, char* rtmVar, char *string)
{
   int id_rtmStruct, id_rtmVar;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
   
   PfUseField(id_rtmVar);
   PfFlushUseFields();
   PfPut(id_rtmVar, string);
//   PfFlush();
   return 0;	   
}

int tPutIntRtm( char *rtmStruct, char* rtmVar, int val)
{
   int id_rtmStruct, id_rtmVar;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
   
   PfUseField(id_rtmVar);
   PfFlushUseFields();
   PfPut(id_rtmVar, &val);
//   PfFlush();
   return 0;	   
}

int tPutFloatRtm( char *rtmStruct, char* rtmVar, float val)
{
   int id_rtmStruct, id_rtmVar;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
   
   PfUseField(id_rtmVar);
   do {
   	PfFlushUseFields();
   } while( apiError != OK );
   
   PfPut(id_rtmVar, &val);
//   PfFlush();
   return 0;	   
}

int tPutFArrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, float *val)
{
   int id_rtmStruct, id_rtmVar, id_array, i;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
     
   for( i = 0; i < arrayNo; i++ ) {   
  	 id_array = PfIndex(id_rtmVar, i);
	 if( apiError != OK )	
 	     return -1;
    
   	PfUseField(id_array);
   	PfFlushUseFields();
   	// cout << " " << *(val);
   	PfPut(id_array, val++);
   }
   PfFlush();
   return 0;	   
}

int tPutFarrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, float val)
{
   int id_rtmStruct, id_rtmVar, id_array;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
     
   id_array = PfIndex(id_rtmVar, arrayNo);
   if( apiError != OK )	
 	 return -1;
    
   PfUseField(id_array);
   PfFlushUseFields();
   PfPut(id_array, &val);
   PfFlush();
   return 0;	   
}

int tPutIarrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, int val)
{
   int id_rtmStruct, id_rtmVar, id_array;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
     
   id_array = PfIndex(id_rtmVar, arrayNo);
   if( apiError != OK )	
 	 return -1;
    
   PfUseField(id_array);
   PfFlushUseFields();
   PfPut(id_array, &val);
   PfFlush();
   return 0;	   
}

int tPutIArrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, int *val)
{
   int id_rtmStruct, id_rtmVar, id_array, i;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;
     
   for( i = 0; i < arrayNo; i++ ) {
 	  id_array = PfIndex(id_rtmVar, i);
	  if( apiError != OK )	
 	     return -1;
   
 	  PfUseField(id_array);
	  PfFlushUseFields();
	  PfPut(id_array, val++);
   }
   PfFlush();
   return 0;	   
}

int tGetCharRtm( char *rtmStruct, char* rtmVar, char *string )
{
   int id_rtmStruct, id_rtmVar;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;   
   
   strcpy( string, (char*) PfGetValuePtrId(id_rtmVar) );
   
//   cout << " tGetCharRtm()::    "  << string << endl;
   
   return 0;
}

int tGetIntRtm( char *rtmStruct, char* rtmVar)
{
   int id_rtmStruct, id_rtmVar;
   int returnInt;
   
   id_rtmStruct = PfGetVarId(rtmStruct);
   if( apiError != OK )	
      return -1;
   id_rtmVar = PfId(id_rtmStruct, rtmVar);
   if( apiError != OK )	
      return -1;  
   
   returnInt = *(int*) PfData(id_rtmVar);
   // cout << "tGetIntRtm()::  " << returnInt << endl;
   return( returnInt );
}


