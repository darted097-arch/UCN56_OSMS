#ifndef _MESSAGES_H
#define _MESSAGES_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// messages.h           
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  Definitions for CCUR to workstation messages and workstation to CCUR messages. 
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files: NONE
//
//
// User Defined Include Files:
//
#include "databaseParameters.h"
#include "messageParameters.h"
//
#include "messageStructure.pdat"
#include "penRecorderStructure.pdat"
#include "RPCCEAStructure.pdat"
#include "incoreStructure.pdat"
#include "colssStructure.pdat"
#include "databaseDisplaysStructure.pdat"
#include "BarChartStructure.pdat"
#include "secureStructure.pdat"
#include "CALStructure.pdat"
#include "BDLStructure.pdat"
#include "ORLStructure.pdat"
#include "LinkStructure.pdat"
#include "BOPDMDStructure.pdat"
#include "UTDVStructure.pdat"
#include "groupDisplayStructure.pdat"
#include "pointSummaryStructure.pdat"
#include "printReportsStructure.pdat"
#include "XRBPStructure.pdat"
#include "DigitalTrendStructure.pdat"
#include "XRBPAUTO.pdat"
#include "cecorStructure.pdat"
#include "cslStructure.pdat"
#include "VideoTrendStructure.pdat"
#include "RCSLeakStructure.pdat"
#include "EQUIPStructure.pdat"
#include "printPIDTestStructure.pdat"
#include "CEATrackMonitorStructure.pdat"

//
// External Variables: NONE
//
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
#ifdef LINUX_APP
void swap32(char *);	// corenet (ntohf(), htonf())
#endif
//
// ***************************************************************************
//
//
//
// These are the database load and update messages.
//
//
// Messages 0x-000 to 0x-00F:
// RESERVED For Database and system messages
//
//  >>>>>>>>>>>>>>>>>>> Digital Database Download Data Template

struct digitalDownloadData {
   short   	ipn;             	// internal point number 	( 2)
   short	pipBitmap;		// misc. pip information 	( 2)   
   char		pid[LENGTH_PID];	// ASCII point id 		( 8)
   char		ed[LENGTH_DESC];	// english description 		(32)
   char		tagid[LENGTH_TAGID];	// sensor tag from DBase file	(12)
   char		message1[LENGTH_MESS];	// message pair #1 		( 8)
   char		message2[LENGTH_MESS];	// message pair #2 		( 8)
};					//				(72)

// you must hardcode the size of the data otherwise it generates a compiler error
// in the #if statement
#define SIZEOF_digitalDownloadData 72
#define NUM_DIGITAL_DBLOAD_PTS (DATA_BYTES/SIZEOF_digitalDownloadData)
#define DIGITAL_DBLOAD_SPARE   (DATA_BYTES - (NUM_DIGITAL_DBLOAD_PTS*SIZEOF_digitalDownloadData))

//  >>>>>>>>>>>>>>>>>>>	Digital: Database Download Message Template

struct digitalDownloadMessage {
   WSmessageHeader	header;				// message information
   digitalDownloadData	data[NUM_DIGITAL_DBLOAD_PTS];	// data portion

//  Use the compiler directives to automatically word align the message and make the message
//  the correct length.
#if DIGITAL_DBLOAD_SPARE != 0
   char			spareData[DIGITAL_DBLOAD_SPARE];
#endif
   
};

//  >>>>>>>>>>>>>>>>>>> Digital Update Message Template (Data Only)

struct digitalUpdateData {
   short   	ipn;             	// internal point number	( 2)
   short   	pisaBitmap;     	// bitmap of pisa fields	( 2) 
};					//				( 4)   

// you must hardcode the size of the data otherwise it generates a compiler error
// in the #if statement
#define SIZEOF_digitalUpdateData 4
#define NUM_DIGITAL_UPDATE_PTS  (DATA_BYTES/SIZEOF_digitalUpdateData)
#define DIGITAL_UPDATE_SPARE    (DATA_BYTES - (NUM_DIGITAL_UPDATE_PTS * SIZEOF_digitalUpdateData))

//  >>>>>>>>>>>>>>>>>>>	Digital: Database Update Message Template

struct digitalUpdateMessage {
   WSmessageHeader	header;				// message information
   digitalUpdateData	data[NUM_DIGITAL_UPDATE_PTS];	// data portion

//  Use the compiler directives to automatically word align the message and make the message
//  the correct length.
#if DIGITAL_UPDATE_SPARE != 0
   char			spareData[DIGITAL_UPDATE_SPARE];
#endif
   
};
 
//  >>>>>>>>>>>>>>>>>>> Analogue Database Download Data Template

struct analogDownloadData {
   short   	ipn;             	// internal point number 	( 2)
   short	pipBitmap;		// misc. pip information 	( 2)   
   char		pid[LENGTH_PID];	// ASCII point id 		( 8)
   char		ed[LENGTH_DESC];	// english description 		(32)
   char		tagid[LENGTH_TAGID];	// sensor tag from DBase file	(12)
   char		eng_units[LENGTH_UNIT];	// english engineering units	(12)
   char		met_units[LENGTH_UNIT];	// metric engineering units 	(12)
   short	disfmt;			// display format		( 2)
   short	pad;			// to align on 4 byte boundary  ( 2)

   float	erange_lo;		// low engineering range	( 4)
   float	erange_hi;		// high engineering range	( 4)
   float	hhval;			// english: hihi alarm limit	( 4)
   float	hval;			// english: high alarm limit	( 4)
   float	lval;			// english: low alarm limit	( 4)
   float	llval;			// english: lolo alarm limit	( 4)
   float	hhmet;			// metric: hihi alarm limit	( 4)
   float	hmet;			// metric: high alarm limit	( 4)
   float	lmet;			// metric: low alarm limit	( 4)
   float	llmet;			// metric: lolo alarm limit 	( 4)
   float	mfactor;		// Met->Eng conversion factor	( 4)
   float	moffset;		// Met->Eng conversion offset	( 4)
   short	lIPN;			// non-zero = ipn of low alarm  ( 2)
   short	hIPN;			// non-zero = ipn of high alarm ( 2)
};					//			       (136)

// you must hardcode the size of the data otherwise it generates a compiler error
// in the #if statement
#define SIZEOF_analogDownloadData 136
#define NUM_ANALOG_DBLOAD_PTS (DATA_BYTES/SIZEOF_analogDownloadData)
#define ANALOG_DBLOAD_SPARE   (DATA_BYTES - (NUM_ANALOG_DBLOAD_PTS*SIZEOF_analogDownloadData))

//  >>>>>>>>>>>>>>>>>>>	Analogue: Database Download Message Template

struct analogDownloadMessage {
   WSmessageHeader	header;				// message information
   analogDownloadData	data[NUM_ANALOG_DBLOAD_PTS];	// data portion

//  Use the compiler directives to automatically word align the message and make the message
//  the correct length.
#if ANALOG_DBLOAD_SPARE != 0
   char			spareData[ANALOG_DBLOAD_SPARE];
#endif
   
};

//  >>>>>>>>>>>>>>>>>>> Database Update Data Template

struct databaseUpdateData {
   short   ipn;             	// internal point number	( 2)
   short   pisaBitmap;     	// bitmap of pisa fields 	( 2)
   float   Value;   		// value			( 4)
};				//				( 8)

// must hardcode the size of the data otherwise it generates a compiler error
// in the #if statement
#define SIZEOF_databaseUpdateData 8
#define NUM_DATABASE_UPDATE_PTS  (DATA_BYTES/SIZEOF_databaseUpdateData)
#define DATABASE_UPDATE_SPARE    (DATA_BYTES - (NUM_DATABASE_UPDATE_PTS * SIZEOF_databaseUpdateData))

//  >>>>>>>>>>>>>>>>>>>	Database Update Message Template

struct databaseUpdateMessage {
   WSmessageHeader	header;				// message information
   databaseUpdateData	data[NUM_DATABASE_UPDATE_PTS];	// data portion

//  Use the compiler directives to automatically word align the message 
//and make the message the correct length.
#if DATABASE_UPDATE_SPARE != 0
   char			spareData[DATABASE_UPDATE_SPARE];
#endif
   
};

//  >>>>>>>>>>>>>>>>>>> Analogue Database Update Data Template

struct analogUpdateData {
   short   ipn;             		// internal point number	( 2)
   short   pisaBitmap;     		// bitmap of pisa fields 	( 2)
   float   analogValue;   		// analog value			( 4)
};					//				( 8)

// must hardcode the size of the data otherwise it generates a compiler error
// in the #if statement
#define SIZEOF_analogUpdateData 8
#define NUM_ANALOG_UPDATE_PTS  (DATA_BYTES/SIZEOF_analogUpdateData)
#define ANALOG_UPDATE_SPARE    (DATA_BYTES - (NUM_ANALOG_UPDATE_PTS * SIZEOF_analogUpdateData))

//  >>>>>>>>>>>>>>>>>>>	Analogue: Database Update Message Template

struct analogUpdateMessage {
   WSmessageHeader	header;				// message information
   analogUpdateData	data[NUM_ANALOG_UPDATE_PTS];	// data portion

//  Use the compiler directives to automatically word align the message and make the message
//  the correct length.
#if ANALOG_UPDATE_SPARE != 0
   char			spareData[ANALOG_UPDATE_SPARE];
#endif
   
};

// >>>>>>>>>>>>>>>>>>>> Database Revision Data
//
struct DBmessageData {
   int			DBRevision;			// Database revision number 	(   4)
};

// >>>>>>>>>>>>>>>>>>>> Shopping List Messages
//
// The analog and digital shpping lists are identical.  Both structures contain
// up to 500 ipns.
struct shoppingData {
   short		theIPN[500];			// just the IPNs
};
// This is used to send the incore data message (only used in parseIncoreMessages.C)
typedef struct {
   WSmessageHeader      header;                         // message information          (  24)
   IncoreDataTemplate   data;                           // message information          (1000)
} IncoreDataMessage;



// HDSR by thlee

struct HDSR_710RECEIVEMSG {
    	int  diskProtect;
    	char fromDate[8];
    	char fromTime[8];
    	char toDate[8];
    	char toTime[8];
};

struct TAPE_HEADER_RECEIVEMSG {
        int resolution;                 // 1-High, 2-Low   "LORESOL"
        char startDate[8];                  // "TAPEDATE(1)"
        char startTime[8];                  // "TAPETIME(1)"
 	char endDate[8];		// "TAPEDATE(2)"
        char endTime[8];                    // "TAPETIME(2)"
        char weekDate[7][8];                // "TWDATE(7)"
};

struct HDSR_720RECEIVEMSG {
        char hiBackupDate[8];            // 0-Not start, 1-Start  "STRTLOBU"
        char hiBackupTime[8];             // 0-Not start, 1-Start  "STRTHIBU"
        char loBackupDate[8];          // 1-Backup request  "HIBUREQ"
        char loBackupTime[8];           // 1-Backup request  "LOBUREQ"
};

struct HDSR_730RECEIVEMSG {
        char highDiskDate_1[8];            // "HIDSKDAT"
        char highDiskTime_1[8];            // "HIDSKTIME"
        char highDiskDate_2[8];            // "HIDSKDAT"
        char highDiskTime_2[8];            // "HIDSKTIME"
        char lowDiskDate_1[8];             // "LODSKDAT"
        char lowDiskTime_1[8];             // "LODSKTIME"
        char lowDiskDate_2[8];             // "LODSKDAT"
        char lowDiskTime_2[8];             // "LODSKTIME"
};


struct HDSR_730SENDMSG {
        int     dataSource;             // 1-Disk, 2-Tape
        int     resolution;             // 1-High, 2-Low
        int     timeStep;
        char    startDate[8];           // mm/dd/yy
        char    startTime[8];
        char    endDate[8];             // mm/dd/yy
        char    endTime[8];
        char    point_1[8];
        char    point_2[8];
        char    point_3[8];
        char    point_4[8];
        char    point_5[8];
        char    point_6[8];
        char    point_7[8];
        char    point_8[8];
};

struct HDSR_74XSENDMSG {
        int     graphSet;
        int     dataSource;             // 1-Disk, 2-Tape
        int     resolution;             // 1-High, 2-Low
        char    startDate[8];           // mm/dd/yy
        char    startTime[8];
        char    point_1[8];
        char    point_2[8];
        char    point_3[8];
        char    point_4[8];
        float   minValue_1;
        float   minValue_2;
        float   minValue_3;
        float   minValue_4;
        float   baseline_1;
        float   baseline_2;
        float   baseline_3;
        float   baseline_4;
        float   maxValue_1;
        float   maxValue_2;
        float   maxValue_3;
        float   maxValue_4;
};

struct GRAPH_INT_SETUP {
        int     dataSource;             // 1-Disk, 2-Tape
        int     resolution;             // 1-High, 2-Low
        char    point_1[8];
        char    point_2[8];
        char    point_3[8];
        char    point_4[8];
        float   minValue_1;
        float   minValue_2;
        float   minValue_3;
        float   minValue_4;
        float   baseline_1;
        float   baseline_2;
        float   baseline_3;
        float   baseline_4;
        float   maxValue_1;
        float   maxValue_2;
        float   maxValue_3;
        float   maxValue_4;
};
struct GRAPH_VIEW_RECEIVEMSG {
	int graphSet;			// 8 sets
	int noPoint;			// 4 points
	int pointType;			// "analog-1, digital-2"
	float minimum;
	float baseline;
	float maximum;
	
	char time_1[8];
	char time_2[8];
	char time_3[8];
	char time_4[8];
	char time_5[8];
	char sDate[8];
	char eDate[8];
	char sTime[8];
	char eTime[8];
	char pointID[8];
	char description[32];

	union {
		float fv[256];
		int   iv[256];
	} value;
};

struct GRAPH_VIEW_ACT {
 	int  level;
	int  sector;
};

struct DYNAMIC_MESSAGE {
 	int  num;
	char message[48];
};

#endif
