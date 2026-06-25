
#ifndef _SYSTEM_PARAMS_H
#define _SYSTEM_PARAMS_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// systemParameters.h   
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
// Description: definitions for generic system variables
// **************************************************************************************
//
// **************************************************************************************
//
// Home directory for list files
#define	LIST_DIR		"/lst/"
//#define PICTURE_DIR		"/pictures/"
#define PICTURE_DIR		"/pictures_ucn56EOF/"	// by feel9 

// Control Variables
#undef  GENERATE_PDAT					// setting this can generate a file with all point ids
//
// All the "logical" variables
#define FAIL			0		
#define SUCCESS			(!FAIL)
#define YES    			1
#define NO     			0
#define TRUE   			1
#define FALSE  			0
#define READY			1
#define NOTREADY		0
//
// Picasso System Variables
#define UPDATE_RATE		200			// Picasso API update rate in milliseconds
//#define UPDATE_RATE		1000			// Picasso API update rate in milliseconds

// Maximum length of WS hostname
#define LEN_SYSTEMNAME		12
#define MAX_HOSTNAME_LEN	LEN_SYSTEMNAME

// Maximum length of WS portno  //210108_jhp
#define MAX_PORTNO_LEN          8 

// Max length of MODEM string
const int MODEM_OPTION_LEN = 20;
//
// Parameters

#define LAPSED_TIME_INTERVAL	2 			// output timing information if Picasso

#define	APP_NAME		"UCN5and6"		// Picasso application name
#define	RTM_NAME		"rtm"			// Picasso rtm name
#define	CACHE_SIZE		10000			// Picasso rtm cache size

// NUM_REC must be greater than or equal to NALL on the Concurrent side 
// #define NUM_REC 9500
#define NUM_REC 9999

// NUM_TEMP is number of points on temporary shopping list
#define NUM_TEMP 100

// types of data that can be put onto a data queue object
// type 0 is reserved by the object
const int DQO_WS_MESSAGE_TYPE = 1;

const int DEAD_WS_STATE			= 0xffff;
const int STARTING_WS_STATE		= 0x0000;
// Workstation States for Interface->System.wsState
const int GOOD_PROCESSES_WS_STATE 	= 0x0001;
// Primary/Backup determined
const int PB_DETERMINED_WS_STATE 	= 0x0002;	
// cyclic sending updates
const int SHOP_LISTS_BUILT_WS_STATE	= 0x0004;
const int DB_DEF_DONE_WS_STATE 		= 0x0008;
// change from backup to primary commlink occurred
const int PB_CHANGE_WS_STATE		= 0x0010; 

// composite states
const int BUILD_SL_WS_STATE	= DB_DEF_DONE_WS_STATE +
				  GOOD_PROCESSES_WS_STATE +
				  PB_DETERMINED_WS_STATE;
const int BSL_LOST_GP_WS_STATE	= BUILD_SL_WS_STATE - 
				  GOOD_PROCESSES_WS_STATE;
const int COM_LOST_WS_STATE	= DB_DEF_DONE_WS_STATE +
				  GOOD_PROCESSES_WS_STATE +
				  SHOP_LISTS_BUILT_WS_STATE;
const int PROC_LOST_WS_STATE	= PB_DETERMINED_WS_STATE +
				  SHOP_LISTS_BUILT_WS_STATE +
				  DB_DEF_DONE_WS_STATE;
const int READY_WS_STATE	= GOOD_PROCESSES_WS_STATE +
				  PB_DETERMINED_WS_STATE +
				  SHOP_LISTS_BUILT_WS_STATE +
				  DB_DEF_DONE_WS_STATE;
const int NEW_PB_WS_STATE	= GOOD_PROCESSES_WS_STATE +
				  PB_DETERMINED_WS_STATE +
				  SHOP_LISTS_BUILT_WS_STATE +
				  DB_DEF_DONE_WS_STATE +
				  PB_CHANGE_WS_STATE;
const int DBDEF_NEW_PB_WS_STATE	= NEW_PB_WS_STATE -
				  DB_DEF_DONE_WS_STATE;
const int DBDEF_READY_WS_STATE	= READY_WS_STATE -
				  DB_DEF_DONE_WS_STATE;

#endif


