#ifndef _DB_PARAMS_H
#define _DB_PARAMS_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// databaseParameters.h 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// Description: database definitions for UCN5&6
// ***************************************************************************
// 
// ************************* IMPORTANT ***********************************
// NOTE: ANY MODIFICATIONS TO THIS FILE REQUIRE ANALOGOUS MODIFICATIONS TO
// THE PICASSO .pdat FILE.  
// ************************* IMPORTANT ***********************************
//
// System Include Files:
//
#include <sys/types.h>  
//
// User Defined Include Files: NONE
//
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
// ______________________________________________________________________
//|                                                                      |
//|          Definition  for the character field lengths                 |
//|______________________________________________________________________|

// character field length parameters
const int 	LENGTH_PID 	=    	8;
const int 	LENGTH_DESC 	=   	32;
const int 	LENGTH_UNIT 	=   	12;
const int 	LENGTH_MESS 	=   	8;
const int 	LENGTH_TAGID	=  	12;
const int	LENGTH_DISPID	=	12;

// *******************************************************************
//   bitmap fields
// *******************************************************************				
//
// pip bitmap: note 1 (CCUR bit ordering)
// 00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15
//
// bits 00:02 unused
// bit  03:   Alarm State (digital only, 0 = alarm on 0, 1 = alarm on 1)
// bit  04:   lolo alarm exits (analog only)
// bit  05:   low alarm exists (analog only)
// bit  06:   high alarm exists (analog only)
// bit  07:   hihi alarm exists (analog only)
// bit  08:   change protected
// bit  09:   alarm type (1=alarm, digital only)
// bit  10:   metunit (1=metric, analog only)
// bits 11:15 point type
						// bits 0:2 unused
const int bALSTAT	=	0x1000;		// bit 3 AlarmState (dig only)
const int bLLE 		=	0x0800;		// bit 4 lolo exists
const int bLE 		=	0x0400;		// bit 5 low exists
const int bHE 		=	0x0200;		// bit 6 high exists
const int bHHE 		=     	0x0100;		// bit 7 hihi exists
const int bCHGPROT 	= 	0x0080;		// bit 8 change protected
const int bALMTYP 	=  	0x0040;		// bit 9 alarm type
const int bMETUNIT 	= 	0x0020;		// bit 10 metric unit 
const int bTYPE 	=	0x001f;		// bits 11:15 point type
//
// pisa bitmap: note 2  (CCUR bit ordering)
// 00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15
// bit 00: unused
// bit 01: out of scan
// bit 02: out of alarm check
// bit 03: enable insert value
// bit 04: alarmed last scan
// bit 05: failed last scan
// bit 06: bad last scan
// bit 07: current digital state
// bit 08: unreliable
// bit 09: questionable
// bit 10: unused
// bits 11:15 new alarm status

const int bOOSCAN	=	0x4000;	// bit 1 out of scan 
const int bOCHEK	=	0x2000;	// bit 2 out of alarm check
const int bENVAL	=	0x1000;	// bit 3 enable inserted value 
const int bALSCAN	=	0x0800;	// bit 4 alarmed last scan 
const int bFLSCAN	=	0x0400;	// bit 5 failed last scan 
const int bBLSCAN	=	0x0200;	// bit 6 bad last scan
const int bCURSTT	=	0x0100;	// bit 7 current digital state
const int bUNRELI	=	0x0080;	// bit 8 unreliable
const int bQUES		=	0x0040;	// bit 9 questionable
					// bit 10 unused
const int bNEWSTA	=	0x001f;	// bits 11:15 for alarm state (0 - 20)

const float BAD_VALUE	=	-99999.0;
const float OORH_VALUE	=	 99998.0;  // added for vtrend by yrlee 20010913
const float OORL_VALUE	=	-99998.0;  // added for vtrend by yrlee 20010913

// *******************************************************************
//   			alarm states 
// *******************************************************************

// NAK = unacknowledged alarm condition, ACK = acknowledged 

const int NORMAL	=	0;
const int RALACK	=	1;	  	// return from alarm, out-of-range, bad
const int RORACK	=	2;
const int RBDACK	=	3;
const int RALNAK	=	4;
const int RORNAK	=	5;
const int RBDNAK	=	6;
const int ORHACK	=	7;	  	// out of range high, low, bad
const int ORLACK	=	8;
const int BDACK		=	9;
const int ORHNAK	=	10;
const int ORLNAK	=	11;
const int BDNAK		=	12;
const int HACK		=	13;	  	//setpoint alarms
const int LACK		=	14;
const int HHACK		=	15;
const int LLACK		=	16;
const int HNAK		=	17;
const int LNAK		=	18;
const int HHNAK		=	19;
const int LLNAK		=	20;
const int CACK		=	HHACK;	  	// contact alarm 
const int CNAK		=	HHNAK;		// NOTE:  contact index = HH index on ccur 
const int BADSTA	=	23;		// NOTE:  "bad state" index = -1 on ccur 
const int NALARMSTATES	=	24;


// *******************************************************************
//		   point types (signal types) 
// *******************************************************************

					// analog 
const int AO		=	1;	// analog outputs 				
const int QA		=	2;	// non-input analogs (composed, calculated, constant) 
const int XA		=	3;
const int KO		=	4;
const int AL		=	5;	// analog link inputs (non-PDAS) 	
const int AI		=	6;	// analog inputs (regular, pulse, frequency, rod up/dn)
const int PI		=	7;
const int FI		=	8;
const int ROD		=	9;
					// digital
const int MINDIGTYPE	=	10;
const int CI		=	10;	// digital inputs (regular, interrupt, SOE, bin coded)
const int DI		=	11;
const int SOE		=	12;
const int BCI		=	13;
const int DL		=	14;	// digital link inputs (non-PDAS) 		
const int QD		=	15;	// non-input digitals (composed, calculated) 	
const int XD		=	16;
const int CO		=	17;	// digital outputs (regular, binary-coded-decimal) 
const int BCO		=	18;
const int NPTYPES	=	19;

#endif
