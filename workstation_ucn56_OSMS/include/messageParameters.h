#ifndef _MESSAGE_PARAMS_H
#define _MESSAGE_PARAMS_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// messageParameters.h  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  Definitions for all messageParameters
// ***************************************************************************
//
// ***************************************************************************
//
// User Defined Include Files:
#include        "colssParameters.h"             // COLSS message type definition
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
const int MESSAGE_BYTES  	= 	4096;      		// must match WSOBUFSIZ in WSCOMM.INC
const int HEADER_BYTES     	=	24;      		// number bytes reserved for header info
const int DATA_BYTES     	=	(MESSAGE_BYTES - HEADER_BYTES);

const int WS_HEADER_LENGTH	=	24;			// header for w/s message in bytes
const int WS_DATA_LENGTH	=	4072;			// data portion for a w/s message in bytes
const int WS_MESSAGE_LENGTH	=	4096;			// maximum length for w/s message in bytes
#define   WORKSTATION_MSG 		10L			// message queue identifier for w/s to ccur message
#define   CC_MSG                        15L                     // message queue identifier for w/s to ccur message



// message type numbers
// 
// All messages which go from the GUI [to CCUR] should be in the form 0x1---.
// All messages which go [from CCUR] to the GUI should be in the form 0x0---.
//
// Note: the numbering convention 0x- includes the message numbers 0x0 to 0x1.
// E.g. Messages 0x-100 to 0x-10F: includes messages 0x0100 to 0x010F and
// 0x1100 to 0x110F.  This range covers 16 messages from CCUR to the workstation and
// 16 messages from the workstation to CCUR.
//
// Messages 0x-000 to 0x-00F:
// Messages 0x-010 to 0x-01F: 
// RESERVED For Database and system messages
//
const int UPDATE_DB_DIGI  =	0x0001;	// CCUR update digital PID
const int UPDATE_DB_ANAL  =	0x0002;	// CCUR update analog PID
const int DEFINE_DIGITAL  =	0x0003;	// CCUR defines digital PIDs
const int DEFINE_ANALOG   =	0x0004;	// CCUR defines analog PIDs 
const int UPDATE_DATABASE =	0x0005;	// CCUR updates PIDs 
const int PERM_SHOP_LIST  =	0x1006;	// WS sends perm shopping list to host
const int REVISION_REQUEST =	0x0007;	// CCUR requests DB revision 
const int REVISION_DATA   =	0x1008;	// Workstation sends current DB rev
const int REVISION_DEFINED =	0x0009;	// CCUR defines DB revision
const int HEARTBEAT       =	0x000A;	// CCUR keeps link alive
const int SET_TIME        =	0x000B;	// CCUR sets current time
const int PB_STATUS_CHANGE_ON = 0x000C; // WS detects PB status change often 
					//indicating a FAILOVER occurred on 
					//the host.


// Messages for temporary shopping lists
const int WSINIT_TEMP_SHOP_LIST =               0x000F;         // W/S zeros out number of shop list items
const int INIT_TEMP_SHOP_LIST	=		0x100F;		// W/S sends temporary shopping list initialize 
const int TEMP_SHOP_LIST_ADD	=		0x1010;		// adds point-id to temp shop list message on W/S
const int TEMP_SHOP_LIST_SEND	=		0x1011;		// W/S sends temporary shopping list  
const int TEMP_SHOP_INIT_ADD	=		0x1012;		// W/S sends init, adds one point-id,
//                                                                 W/S sends temporary shop list
// Common shopping list messages
const int REQ_SHOPPING_LISTS	=		0x0013;		// CCUR requests permanent and temporary shopping lists
//
// Failover messages
const int SEND_MASTER_SLAVE_STATUS =            0x0016;         // CCUR sends its Master/Slave status to W/S
const int SEND_PRIMARY_BACKUP_STATUS=           0x1017;         // W/S sends Primary/Backup status for each CCUR connection
//
// Messages 0x-020 to 0x-02F: RESERVED
// Messages 0x-030 to 0x-03F: RESERVED
// Messages 0x-040 to 0x-04F: RESERVED
// Messages 0x-050 to 0x-05F: RESERVED
// Messages 0x-060 to 0x-06F: RESERVED
// Messages 0x-070 to 0x-07F: RESERVED
// Messages 0x-080 to 0x-08F: RESERVED
// Messages 0x-090 to 0x-09F: RESERVED
//
// All the messages you will ever need.  Use some thought...
//
// ***********************************
// Messages 0x-100 to 0x-10F: Video Trend
const int  REQUEST_VTREND_DATA          =       0x1100;         // VTREND request message
const int  RECIEVE_VTREND_DATA          =       0x0100;         // VTREND Data from CCUR
const int  PROCESS_VTREND_DATA          =       0x1101;         // Send new data to CCUR
const int  CONFIRM_VTREND_PROCESS       =       0x0101;         // Confirmation of changes from CCUR
const int  VTREND_PROCESS_CONFIRMED     =       0x0105;         // Tell picture that host confirmed processing
const int  DISPLAY_VTREND_DATA          =       0x0106;         // Tell picture to display data
const int  VTREND_RESET_REQUEST         =       0x1102;         // VT: request group reset
const int  CONFIRM_VTREND_FLAG          =       0x0102;         // VT: confirm flag data
const int  DISPLAY_VTREND_RESET         =       0x0103;         // Tell the flag no. to the picture      

// Messages 0x-110 to 0x-11F: NONE
// Messages 0x-120 to 0x-12F: NONE
// Messages 0x-130 to 0x-13F: NONE

// Messages 0x-140 to 0x-14F: Digital Trend

const int   REQUEST_DTREND_DATA         =       0x1140; // Digital Trend setup message
const int   SEND_DTREND_DATA            =       0x0140; // Digital Trend data from CCUR
const int   DISPLAY_DTREND_DATA         =       0x014C; // Tell picture to display data

// Messages 0x-150 to 0x-15F: Digital Trend messages ===jklee==

const int   PROCESS_DTREND_DATA         =       0x1150; // Send new data to CCUR
const int   CONFIRM_DTREND_PROCESSING   =       0x0150; // Send confirmation of changes to w/s
const int   DTREND_PROCESSING_CONFIRMED  =      0x015C; // Tell picture that host confirmed processing
const int   DTREND_PRT_PROCESS_CONFIRMED =      0x015F; // Tell picture that host print confirmed processing

// Messages 0x-160 to 0x-16F: NONE
const int   RESET_DTREND_REQUEST          =     0x1160; // Digital Trend reset request
const int   INUSE_DTREND_RESPONSE         =     0x0160; // send message that the DTrend is in use  
const int   INUSE_DTRD_RESPONSE_CONFIRM   =     0x016A; // Tell picture that the DTrend is in use  
const int   COMPLETED_DTREND_PRINT        =     0x0161; // send message that the print is completed
const int   COMPLETED_DTRD_PRT_CONFIRM    =     0x016B; // Tell picture that the print is completed
const int   INTERRUPTED_DTREND_PRINT      =     0x0162; // send message that the print in intrrupted
const int   INTERRUPT_DTRD_PRT_CONFIRM    =     0x016C; // Tell picture that the print in intrrupted

// Messages 0x-170 to 0x-17F: NONE
// Messages 0x-180 to 0x-18F: Group Display Messages 
const int  GROUP_INFO_REQUEST            =      0x1180;         // GROUP: request group information
const int  GROUP_INFO_DATA               =      0x0180;         // GROUP: receive group information
const int  GROUP_UPDATE                  =      0x1185;         // GROUP: update group x data
const int  GROUP_CONFIRM                 =      0x0185;         // GROUP: confirm group x data
const int  GROUP_PROCESS_CONFIRMED       =      0x0186;         // Tell the picture that host processed data
const int  DISPLAY_GROUP_DATA            =      0x0187;         // Tell the picture to display data
const int  GROUP_RESET_REQUEST           =      0x1181;         // GROUP: request group reset
const int  DISPLAY_GROUP_FLAG            =      0x0181;         // GROUP: confirm flag data
const int  DISPLAY_FLAG_RESET            =      0x0182;         // Tell the flag no. to the picture      

// Messages 0x-190 to 0x-19F: NONE
//
// ***********************************
// Messages 0x-200 to 0x-207: Single Point Display Messages
const int DISPLAY_SINGLE_POINT		=	0x1200;	// Add point to temp shopping list
const int PRINT_SINGLE_POINT		= 	0x1201; // Print single point
const int CONFIRM_PRINT_SINGLE_POINT	= 	0x0201; // Response to print single point
const int PRINT_SINGLE_POINT_CONFIRMED  =       0x020C; // Tell the picture to display data
//
// messages 0x-208 to 0x20F: secure messages
const int   DISPLAY_SECURITY_PAGE       =       0x1208; //

const int   PROCESS_SECURE_DATA         =       0x120f; // secure: messages from WS
const int   CONFIRM_SECURE_PROCESSING   =       0x020f; // secure: messages from ccur
const int   REQUEST_SECURE_SIGNOFF      =       0x020e; // secure: messages from ccur
//
// Messages 0x-210 to 0x-21F: Change Scan Status Opcodes
const int PROCESS_CHGSCN_DATA		=	0x1210;	//
const int CONFIRM_CHGSCN_PROCESSING	=	0x0210;
const int CHGSCN_PROCESSING_CONFIRMED	=	0x1211;	// From CommLink to InterfaceToGUI

// Messages 0x-220 to 0x-22F: Change Alarm Status Opcodes
const int PROCESS_CHGALM_DATA		=	0x1220;	//
const int CONFIRM_CHGALM_PROCESSING	=	0x0220;
const int CHGALM_PROCESSING_CONFIRMED	=	0x1221;	// From CommLink to InterfaceToGUI
   
// Messages 0x-230 to 0x-23F: Change Alarm Limits Opcodes
const int PROCESS_CHGLIM_DATA		=	0x1230;	//
const int CONFIRM_CHGLIM_PROCESSING	=	0x0230;
const int CHGLIM_PROCESSING_CONFIRMED	=	0x1231;	// From CommLink to InterfaceToGUI

// Messages 0x-240 to 0x-24F: Insert Value Opcodes
const int PROCESS_INSVAL_DATA		=	0x1240;	//
const int CONFIRM_INSVAL_PROCESSING	=	0x0240;
const int INSVAL_PROCESSING_CONFIRMED	=	0x1241;	// From CommLink to InterfaceToGUI

// Messages 0x-250 to 0x-25F: Sensor Calibration Opcodes
const int PROCESS_SENCAL_DATA		=	0x1250;	//
const int CONFIRM_SENCAL_PROCESSING	=	0x0250;
const int SENCAL_PROCESSING_CONFIRMED	=	0x1251;	// From CommLink to InterfaceToGUI

// Messages 0x-260 to 0x-26F: Least Squares Fit Opcodes
const int PROCESS_LSF_DATA		=	0x1260;	//
const int CONFIRM_LSF_PROCESSING	=	0x0260;
const int LSF_PROCESSING_CONFIRMED	=	0x1261;	// From CommLink to InterfaceToGUI

const int REQUEST_LSF_SCOUNT            =       0x1590;  
const int RETURN_LSF_SCOUNT             =       0x0590; 
const int LSF_SCOUNT_CONFIRMED          =       0x1591;

// Messages 0x-270 to 0x-27F: Print Report Opcodes

const int   REQUEST_PRREPT                  =       0x1270;
const int   DEMANDLOG_PRINT_CONFIRMED       =       0x0274;
const int   SEND_POSTRIP_DATA               =       0x0275;
const int   DISPLAY_POSTRIP_DATA            =       0x1271;
const int   REQUEST_POSTRIP_PRINT           =       0x1272;
const int   POSTRIP_PRINT_CONFIRMED         =       0x0276;
const int   SEND_SOE_DATA                   =       0x0278;
const int   DISPLAY_SOE_DATA                =       0x1273;
const int   REQUEST_SOE_PRINT               =       0x1274;
const int   SOE_PRINT_CONFIRMED             =       0x0279;
const int   LASTPOSTRIP_PRINT_CONFIRMED     =       0x027B;
const int   LASTSOE_PRINT_CONFIRMED         =       0x027C;

// Messages 0x-280 to 0x-28F: Print Summary Messages 
const int  PRINT_SUMMARY_MESSAGE        =       0x1280;         // Print Summary request ===ywjung===
const int  PRINT_SUMMARY_CONFIRM        =       0x0281;         // Print Summary confirm ===ywjung===

// Messages 0x-290 to 0x-29F: NONE
//
// ***********************************
// Messages 0x-300 to 0x-30F: Alarm Acknowledge Reset Messages
const int   REQUEST_ALARM_ACK           =       0x1300;
const int   CONFIRM_ACK_PROCESSING      =       0x0300;
const int   ACK_PROCESSING_CONFIRMED    =       0x1301;

const int   REQUEST_ALARM_RESET         =       0x1305;
const int   CONFIRM_RESET_PROCESSING    =       0x0305;
const int   RESET_PROCESSING_CONFIRMED  =       0x1306;

// Messages 0x-310 to 0x-31F: Pen Recorder Messages
//
const int   REQUEST_PEN_DATA		=	0x1310;	// Pen Recorder setup message
const int   SEND_PEN_DATA		=	0x0310;        // Pen Data from CCUR
const int   PROCESS_PEN_DATA		=	0x1315;	// Send new data to CCUR
const int   CONFIRM_PEN_PROCESSING	=	0x0315;	// Send confirmation of changes to w/s

// message from CommLink to InterfaceToGUI to tell the picture that the host has processed the data.
// Messages 0x-310 to 0x-31F:
const int   PEN_PROCESSING_CONFIRMED	=	0x1316;	// Tell picture that host has processed data.

// message from CommLink to InterfaceToGUI to tell the picture to display the data.
// Messages 0x-320 to 0x-32F:
const int   DISPLAY_PEN_DATA		=	0x1320;	// Display data from CCUR

// Messages 0x-330 to 0x-338: Current Alarm List
const int   REQUEST_CAL_DATA            =       0x1330; // Current Alarm List first
const int   REQUEST_CAL_NEXT_DATA       =       0x1331; // Current Alarm List next page
const int   REQUEST_CAL_PREV_DATA       =       0x1332; // Current Alarm List previous page
const int   CAL_PAGE_OFF                =       0x1333; // Leaving Current Alarm List Page
const int   SEND_CAL_DATA               =       0x0330; // Current Alarm List from CCUR

// Messages 0x-339 to 0x-33F: Current Alarm List
const int   REQUEST_LINK_STATUS		=	0x1339; // Link status page
const int   LINK_PAGE_OFF		=	0x133A; // Leaving link status page
const int   SEND_LINK_STATUS		=	0x0339; // Sending link statuses

// Messages 0x-340 to 0x-37F: Bar Chart Messages by KWANG
//
const int   REQUEST_BAR_DATA            =       0x1340; // Bar Chart setup message
const int   SEND_BAR_DATA               =       0x0340; // Bar Chart from CCUR
const int   PROCESS_BAR_DATA            =       0x1350; // Send new data to CCUR
const int   CONFIRM_BAR_PROCESSING      =       0x0350; // Send confirmation of changes to w/s

// message from CommLink to InterfaceToGUI to tell the picture that the host has processed the data.
const int   BAR_PROCESSING_CONFIRMED    =       0x1360; // Tell picture that host has processed data.

// message from CommLink to InterfaceToGUI to tell the picture to display the data.
const int   DISPLAY_BAR_DATA            =       0x1370; // Display data from CCUR
// Messages 0x-380 to 0x-38F: NONE

// Messages 0x-380 to 0x-38F: Bad Data List
const int   REQUEST_BDL_DATA            =       0x1380; // Bad Data List first
const int   REQUEST_BDL_NEXT_DATA       =       0x1381; // Bad Data List next page
const int   REQUEST_BDL_PREV_DATA       =       0x1382; // Bad Data List previous page
const int   BDL_PAGE_OFF                =       0x1383; // Leaving  Bad Data List Page
const int   SEND_BDL_DATA               =       0x0380; //  Bad Data List from CCUR

// Messages 0x-390 to 0x-39F: Current Alarm List
const int   REQUEST_ORL_DATA            =       0x1390; // Out of Range List first
const int   REQUEST_ORL_NEXT_DATA       =       0x1391; // Out of Range List next page
const int   REQUEST_ORL_PREV_DATA       =       0x1392; // Out of Range List previous page
const int   ORL_PAGE_OFF                =       0x1393; // Leaving Out of Range List Page
const int   SEND_ORL_DATA               =       0x0390; //Out of Range List from CCUR

//
// ***********************************
// Messages 0x-400 to 0x-40F: HDSR Messages
const int	GET_HDSR_TREND1_DATA		=	0x1401;	// HDSR: request the current data for trend 1
const int	GET_HDSR_TREND2_DATA		=	0x1402;	// HDSR: request the current data for trend 2
const int	GET_HDSR_TREND3_DATA		=	0x1403;	// HDSR: request the current data for trend 3
const int	GET_HDSR_TREND4_DATA		=	0x1404;	// HDSR: request the current data for trend 4

const int	HDSR_TREND1_DATA		=	0x0401;	// HDSR: current trend 1 data from host
const int	HDSR_TREND2_DATA		=	0x0402;	// HDSR: current trend 2 data from host
const int	HDSR_TREND3_DATA		=	0x0403;	// HDSR: current trend 3 data from host
const int	HDSR_TREND4_DATA		=	0x0404;	// HDSR: current trend 4 data from host

const int	PROCESS_HDSR_TREND1_DATA	=	0x140A;	// HDSR: process the current data for trend 1
const int	PROCESS_HDSR_TREND2_DATA	=	0x140B;	// HDSR: process the current data for trend 2
const int	PROCESS_HDSR_TREND3_DATA	=	0x140C;	// HDSR: process the current data for trend 3
const int	PROCESS_HDSR_TREND4_DATA	=	0x140D;	// HDSR: process the current data for trend 4
//
// Messages 0x-410 to 0x-41F: IOCPCS
const int       CONFIRM_CECOR_REQUEST           =       0x0410; // IOCPCS process CECOR queue request
const int	CONFIRM_CPCEAC_REQUEST		=	0x0411; // IOCPCS process CPCS and CEAC queue request
const int	CONFIRM_CPCS_REQUEST		=	0x0412; // IOCPCS process CPCS queue request
const int	CONFIRM_CEAC_REQUEST		=	0x0413; // IOCPCS process CEAC queue request
const int	REJECT_CPCS_REQUEST		=	0x0414; // IOCPCS reject queued request
//
// Messages 0x-420 to 0x-42F: NONE
// Messages 0x-430 to 0x-43F: NONE
// Messages 0x-440 to 0x-44F: NONE
// Messages 0x-450 to 0x-45F: NONE
// Messages 0x-460 to 0x-46F: NONE
// Messages 0x-470 to 0x-47F: NONE
// Messages 0x-480 to 0x-48F: NONE
// Messages 0x-490 to 0x-49F: NONE
//
// ***********************************
// Messages 0x-500 to 0x-50F: RCSLeak--------------------------------------------------------
const int    SEND_RCS_LEAK_DATA		=	0x0500;         //RCSLeak: send online leak data to ws
const int    SEND_LEAK_OFF_DATA		=	0x0501;         //LeakOff: send offline leak data to ws
const int    CONFIRM_LEAKPRN		=	0x0502;         //LeakPrn: send print status to ws

const int    REQUEST_RCS_LEAK_DATA	=	0x1500;         //RCSLeak: request online leak data to ccur
const int    REQUEST_LEAK_OFF_DATA	=	0x1501;         //LeakOff: request offline leak data to ccur
const int    REQUEST_LEAKPRN		=	0x1502;         //LeakPrn: request print work to ccur
//------------------------------------------------------------------------------------------
// Messages 0x-510 to 0x-51F: NONE
// Messages 0x-520 to 0x-52F: NONE
// Messages 0x-530 to 0x-53F: NONE
// Messages 0x-540 to 0x-54F: NONE
// Messages 0x-550 to 0x-55F: NONE
// Messages 0x-560 to 0x-56F: NONE
// Messages 0x-570 to 0x-57F: NONE
// Messages 0x-580 to 0x-58F: NONE
// Messages 0x-590 to 0x-59F: NONE
//
// ***********************************
// Messages 0x-600 to 0x-60F: NONE
// Messages 0x-610 to 0x-61F: NONE
// Messages 0x-620 to 0x-62F: NONE
// Messages 0x-630 to 0x-63F: NONE
// Messages 0x-640 to 0x-64F: NONE
// Messages 0x-650 to 0x-65F: NONE
// Messages 0x-660 to 0x-66F: NONE
// Messages 0x-670 to 0x-67F: NONE
// Messages 0x-680 to 0x-68F: NONE
//  Define: Message Variable Name and Number for RPCCEA Selection
//
const int       RPCCEA_MODE_REQUEST             =       0x1680; // RPCCEA: Request of Current Mode
const int       RPCCEA_PROCESS_REQUEST          =       0x1681;  // RPCCEA: Print Report or Mode Selection
const int       RPCCEA_CURRENT_MODE             =       0x0680; // RPCCEA: Current Mode
const int       RPCCEA_PROCESS_CONFIRM          =       0x0681; // RPCCEA: Confirm of Process Request
//
const int       GENERIC_PROCESSING_CONFIRMED    =       0x068C; // Generic: Confirm of Process Request

//  Define: Message Variable Name and Number for RPCCEA Selection
const int	CEA_ALM_LIMIT_REQUEST   	=	0x1690;		// CEA: Request CEA Alarms and Limits
const int       ONE_CEA_POSITION_REPT		=	0x1691; 	// CEA: One CEA Position Reports 
const int       ONE_CEA_GROUP_POSITION_REPT	=	0x1692; 	// CEA: One CEA Group Position Reports
const int	ALL_CEAGROUP_POSITION_REPT      =	0x1693;		// CEA: All CEA Group Position Reports
const int       CEA_DROPNO_REQUEST		=	0x1694; 	// CEA: CEA Drop Test Selection
const int	CEDMCS_DATAXFER_MODE		=	0x1695;		// CEA: CEDMCS Data Transfer Mode
const int	CEA_ALM_LIMIT_PROCESS   	=	0x0690;		// CEA: Request CEA Alarms and Limits
const int	CEA_MESSAGE_IN_BODY		=	0x0691;		// CEA: Confirm of Process Request
const int	CEA_GENERIC_PROCESS_CONFIRMED	=       0x069C;	        // CEA: Confirm of Process Request


//  Define: Message Variable Name and Number for CEA Tracking Monitor Selection
const int       REQUEST_CEA_TRACK_MONITOR1      =       0x1C11;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR1      =       0x0C11;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR2      =       0x1C12;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR2      =       0x0C12;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR3      =       0x1C13;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR3      =       0x0C13;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR4      =       0x1C14;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR4      =       0x0C14;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR5      =       0x1C15;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR5      =       0x0C15;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR6      =       0x1C16;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR6      =       0x0C16;         // CEA: Confirm cea track monitor display
const int       REQUEST_CEA_TRACK_MONITOR9      =       0x1C19;         // CEA: Request cea track monitor display
const int       CONFIRM_CEA_TRACK_MONITOR9      =       0x0C19;         // CEA: Confirm cea track monitor display


// Messages 0x-690 to 0x-69F: NONE
//
// ***********************************
// Messages 0x-700 to 0x-70F: INCORE
const int       INCORE_PROCESS_REQUEST          =       0x1700;  // INCORE: Request of Processing
const int       INCORE_CHARGE_REQUEST           =       0x1705;  // INCORE: Update Charging
const int       INCORE_CHARGE_UPDATE            =       0x1710;  // INCORE: Update Charging
const int       INCORE_PROCESS_CONFIRM          =       0x0700;  // INCORE: Confirm of Process Request
const int       INCORE_CHARGE_RESPONSE          =       0x0705;  // INCORE: Update Charging
// Messages 0x-710 to 0x-71F: NONE
// Messages 0x-720 to 0x-72F: NONE
// Messages 0x-730 to 0x-73F: NONE
// Messages 0x-740 to 0x-74F: NONE
// Messages 0x-750 to 0x-75F: NONE
// Messages 0x-760 to 0x-76F: NONE
// Messages 0x-770 to 0x-77F: NONE

// Messages 0x-780 to 0x-78F: COLSS
// Messages 0x-790 to 0x-79F: COLSS
// Most COLSS opcodes are found in colssParameters.h, which is included at the top of this file.

const int         REQUEST_COLSS_PROCESSING               = 0X1781 ;
const int         CONFIRM_COLSS_PROCESSING               = 0X0781 ;
//
//const int   REQUEST_COLSS_PROCESSING  =       0x1780;         // COLSS request message to CCUR
//const int   CONFIRM_COLSS_PROCESSING  =       0x0780;         // COLSS confirm message from CCUR
// 
//
// ***********************************
// Messages 0x-800 to 0x-80F: UTDV

const int   PROCESS_UTDV_SEND           =       0x1800;
const int   PROCESS_UTDV_RECEIVE        =       0x0800;
const int   PROCESS_UTDV_RECEIVE_DATA   =       0x0801;

// Messages 0x-810 to 0x-81F: NONE
// Messages 0x-820 to 0x-82F: NONE
// Messages 0x-830 to 0x-83F: NONE

// Messages 0x-840 to 0x-84F: XRBP Messages
// *********************************** Added by SMBAEK for XRBP Implementation
const int XRBP_REQUEST    =	0x1840;  // XRBP: process the input data for off-line
const int XRBP_DATA       =	0x0840;  // XRBP: receive XRBP offline result from host
const int XRBP_INIT       =	0x1841;  // XRBP: initialize for XRBP offline Calc
const int XRBP_READY      =	0x0841;  // XRBP: initialization completed
const int XRBPA_UP_START  =	0x184A;  // XRBPA: Display Start Request
const int XRBPA_UP_STOP   =	0x184B;  // XRBPA: Display Stop  Request
const int XRBPA_DISPLAY   =	0x084A;  // XRBPA: Display Request from CCUR to HP
const int XRBP_MSG        =     0x084B;  // XRBP : Message from CCUR

// Messages 0x-850 to 0x-85F: NONE
// Messages 0x-860 to 0x-86F: NONE
// Messages 0x-870 to 0x-87F: NONE
// Messages 0x-880 to 0x-88F: BOP messages

// *********************************** Added by SMBAEK for DYNMSG (temporary)
const int DYN_MSG         =	0x1888;  // DYNMSG: receive DYNMSG from host

//
const int   REQUEST_BOP_PRINT     =       0x1880;         // BOP Report Print request ===jklee===
const int   CONFIRM_BOP_PRINT     =       0x0880;         // BOP Report Print confirm ===jklee===
const int   BOP_PRINT_CONFIRMED   =       0x088C;         // picture that host confirmed processing ===jklee=

// Messages 0x-890 to 0x-89F: NONE
//
// ***********************************
// Messages 0x-900 to 0x-90F: CECOR Messages
const int   REQUEST_CECOR_SNAPSHOT         =       0x1900;
const int   CONFIRM_CECOR_DISKFILE         =       0x0900;
const int   CONFIRM_CECOR_TAPE             =       0x0901;
const int   ERROR_QUEUE_IOCPCS             =       0x0902;
const int   ERROR_CECOR_FILEOPEN           =       0x0903;
const int   ERROR_CECOR_MAG1OPEN           =       0x0904;
const int   ERROR_CECOR_DISKREAD           =       0x0905;
const int   ERROR_TAPE_NOT_MOUNT           =       0x0906;
const int   ERROR_CECOR_MAG1WRITE          =       0x0907;

// Messages 0x-910 to 0x-91F: NONE
// Messages 0x-920 to 0x-92F: NONE
// Messages 0x-930 to 0x-93F: NONE

// Messages 0x-940 to 0x-94F: DSPM Messages
const int   REQUEST_DSPM_REPORT            =       0x1940;
const int   ERROR_QUEUE_TASK               =       0x0940;
const int   CONFIRM_CPC_REPORT             =       0x0941;
const int   CONFIRM_CEAC_REPORT            =       0x0942;
const int   CONFIRM_PPS_REPORT             =       0x0943;
const int   CONFIRM_CPC_TRP_RPT            =       0x0944;
const int   CONFIRM_CEAC_TRP_RPT           =       0x0945;
const int   ERROR_OPEN_PRINTER             =       0x0946;
const int   CONFIRM_QUEUE_DSPMRPT          =       0x0947;

// Messages 0x-950 to 0x-95F: NONE
// Messages 0x-960 to 0x-96F: NONE
// Messages 0x-970 to 0x-97F: NONE
// Messages 0x-980 to 0x-98F: NONE
// Messages 0x-990 to 0x-99F: NONE
//
//************************************
//
// Messages 0x-A00 to 0x-A0F: NONE
// Messages 0x-A10 to 0x-A1F: NONE
// Messages 0x-A20 to 0x-A2F: NONE
// Messages 0x-A30 to 0x-A3F: NONE
//

// Messages 0x-A40 to 0x-A4F: CSL Messages
const int   REQUEST_CSL_DISPLAY            =       0x1A40;
const int   EXIT_CSL_DISPLAY               =       0x1A41;
const int   SEND_CSL_DISPLAY               =       0x0A40;

// Messages 0x-A50 to 0x-A5F: NONE
// Messages 0x-A60 to 0x-A6F: NONE
// Messages 0x-A70 to 0x-A7F: NONE
// Messages 0x-A80 to 0x-A8F: NONE
// Messages 0x-A90 to 0x-A9F: NONE
// Messages 0x-AA0 to 0x-AAF: NONE
// Messages 0x-AB0 to 0x-ABF: NONE
// Messages 0x-AC0 to 0x-ACF: NONE
// Messages 0x-AD0 to 0x-ADF: NONE
// Messages 0x-AE0 to 0x-AEF: NONE
// Messages 0x-AF0 to 0x-AFF: NONE

//
//*********************************
//
// Messages 0x-B00 to 0x-B0F: NONE

// Messages 0x-B10 to 0x-B1F: HDSR

const int HDSR_PAGE_710 =               0x1B10;
const int HDSR_DISK_PROTECT =           0x1B11;
const int HDSR_PAGE_720  =              0x1B12;
const int HDSR_BACKUP =                 0x1B13;
const int HDSR_PAGE_730  =              0x1B14;

const int HDSR_CANCEL_LOG =             0x1B15;
const int HDSR_TAPE_HEADER =            0x1B16;
const int HDSR_LOG =                    0x1B17;
const int HDSR_SECTOR =                 0x1B18;

const int HDSR_SYSTEM_ERROR =           0x1B1E;
const int HDSR_PAGE_750 =               0x1B1F;

// Messages 0x-B20 to 0x-B2F: HDSR

const int HDSR_PAGE_740  =              0x1B20;
const int HDSR_GRAPH_SETUP =            0x1B21;
const int HDSR_GRAPH_VIEW =             0x1B22;

const int GRAPH_SETUP_74X =             0x1B23;
const int GRAPH_VIEW_74X =              0x1B24;
const int TAPE_HEADER_74X =             0x1B25;

const int TAPE_HEADER_750 =             0x1B26;

const int GRAPH_VIEW_LEFT =             0x1B27;
const int GRAPH_VIEW_RIGHT =            0x1B28;
const int GRAPH_VIEW_UP =               0x1B29;
const int GRAPH_VIEW_DOWN  =            0x1B2A;
const int GRAPH_VIEW_DONE =             0x1B2B;

const int HDSR_INIT =                   0x1B2D;
const int HDSR_FOOTER_MESSAGE  =        0x1B2E;
const int DYN_MESSAGE =                 0x1B2F;

// -----------------------------------------
const int HDSR_PAGE_710r =               0x0B10;
const int HDSR_DISK_PROTECTr =           0x0B11;
const int HDSR_PAGE_720r  =              0x0B12;
const int HDSR_BACKUPr =                 0x0B13;
const int HDSR_PAGE_730r  =              0x0B14;

const int HDSR_CANCEL_LOGr =             0x0B15;
const int HDSR_TAPE_HEADERr =            0x0B16;
const int HDSR_LOGr =                    0x0B17;
const int HDSR_SECTORr =                 0x0B18;

const int HDSR_SYSTEM_ERRORr =           0x0B1E;
const int HDSR_PAGE_750r =               0x0B1F;

// Messages 0x-B20 to 0x-B2F: HDSR

const int HDSR_PAGE_740r  =              0x0B20;
const int HDSR_GRAPH_SETUPr =            0x0B21;
const int HDSR_GRAPH_VIEWr =             0x0B22;

const int GRAPH_SETUP_74Xr =             0x0B23;
const int GRAPH_VIEW_74Xr =              0x0B24;
const int TAPE_HEADER_74Xr =             0x0B25;

const int TAPE_HEADER_750r =             0x0B26;

const int GRAPH_VIEW_LEFTr =             0x0B27;
const int GRAPH_VIEW_RIGHTr =            0x0B28;
const int GRAPH_VIEW_UPr =               0x0B29;
const int GRAPH_VIEW_DOWNr  =            0x0B2A;
const int GRAPH_VIEW_DONEr =             0x0B2B;

const int HDSR_INITr       =             0x0B2D;
const int HDSR_FOOTER_MESSAGEr  =        0x0B2E;
const int DYN_MESSAGEr =                 0x0B2F;

// CORENET ujson add 2017.0220
const int REQUEST_MASS_CALC =             0x1503;
const int CONFIRM_MASS_CALC =             0x0503;
const int REQUEST_MASS_PRN =             0x1504;
const int CONFIRM_MASS_PRN =             0x0504;
const int CONFIRM_MASS_MSG =             0x0505;
const int REQUEST_MASS_INIT =             0x1506;
//const int CONFIRM_MASS_STOP =             0x0505;

const int REQUEST_EQUIPVALCHG =           0x1551;
const int CONFIRM_EQUIPVALCHG =           0x0551;
const int REQUEST_EQUIPSTATUS =           0x1552;
const int CONFIRM_EQUIPSTATUS =           0x0552;
const int REQUEST_EQUIPPRN =              0x1553;
const int CONFIRM_EQUIPPRN =              0x0553;
const int REQUEST_PIDTEST =               0x1130;
const int CONFIRM_PIDTEST =               0x0130;

#endif
