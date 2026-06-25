//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseUTDVMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  This module processes messages the UTDV messages
// **************************************************************************************
//
// **************************************************************************************
//
// System Include Files:
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.11
#include <netinet/in.h> // CORENET 2012.07.11
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
#include "messageObject.h"
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject      IGUI_MsgQue;
//
// Local Constant Definitions: NONE
//
// Function Prototypes:
//
#include "prototypes.h"
//   
//
// **************************************************************************************
//
void parseUTDVMessages(messageObject & msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   UTDV_DIALOGUE_DATA	*utdvdlgdata_ptr;	
   UTDV_CEA_SUBGROUP	*ceasubgrp_ptr;
   UTDV_DATA		*utdvdata_ptr;	
   int i, j;	
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef DEBUG   
   cout << "****in ParseUTDV msg****  opcode =" << hex << msg.getOpcode() << endl;
#endif   
   switch (msg.getOpcode())
   {
        // Send the same info for both messages; it won't hurt
        
	case PROCESS_UTDV_SEND:
	   // copy the Interface structure into the message
	   msg.setData(&(Interface->UtdvDialogue), sizeof(Interface->UtdvDialogue));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   utdvdlgdata_ptr = (UTDV_DIALOGUE_DATA *)msg.getDataPtr();

	   utdvdlgdata_ptr->current_page = htonl(utdvdlgdata_ptr->current_page);
	   utdvdlgdata_ptr->option = htonl(utdvdlgdata_ptr->option);
	   utdvdlgdata_ptr->new_page_request = htonl(utdvdlgdata_ptr->new_page_request);
	   swap32((char *)&(utdvdlgdata_ptr->entered_EFPH));
	   swap32((char *)&(utdvdlgdata_ptr->cea_average));

	   for (i = 0; i < 6; i++) {
		if (i == 0) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col1);
		if (i == 1) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col2);
		if (i == 2) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col3);
		if (i == 3) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col4);
		if (i == 4) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col5);
		if (i == 5) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col6);
		else break;

	        ceasubgrp_ptr->SubGrpNumber = htonl(ceasubgrp_ptr->SubGrpNumber);
	        ceasubgrp_ptr->row1CeaNumber = htonl(ceasubgrp_ptr->row1CeaNumber);
	        ceasubgrp_ptr->row2CeaNumber = htonl(ceasubgrp_ptr->row2CeaNumber);
	        ceasubgrp_ptr->row3CeaNumber = htonl(ceasubgrp_ptr->row3CeaNumber);
	        ceasubgrp_ptr->row4CeaNumber = htonl(ceasubgrp_ptr->row4CeaNumber);
	        swap32((char *)&(ceasubgrp_ptr->row1CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row2CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row3CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row4CeaValue));
	        ceasubgrp_ptr->row1CeaQuality = htonl(ceasubgrp_ptr->row1CeaQuality);
	        ceasubgrp_ptr->row2CeaQuality = htonl(ceasubgrp_ptr->row2CeaQuality);
	        ceasubgrp_ptr->row3CeaQuality = htonl(ceasubgrp_ptr->row3CeaQuality);
	        ceasubgrp_ptr->row4CeaQuality = htonl(ceasubgrp_ptr->row4CeaQuality);
	   }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	   break;

	case PROCESS_UTDV_RECEIVE:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   utdvdlgdata_ptr = (UTDV_DIALOGUE_DATA *)msg.getDataPtr();

	   utdvdlgdata_ptr->current_page = ntohl(utdvdlgdata_ptr->current_page);
	   utdvdlgdata_ptr->option = ntohl(utdvdlgdata_ptr->option);
	   utdvdlgdata_ptr->new_page_request = ntohl(utdvdlgdata_ptr->new_page_request);
	   swap32((char *)&(utdvdlgdata_ptr->entered_EFPH));
	   swap32((char *)&(utdvdlgdata_ptr->cea_average));

	   for (i = 0; i < 6; i++) {
		if (i == 0) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col1);
		if (i == 1) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col2);
		if (i == 2) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col3);
		if (i == 3) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col4);
		if (i == 4) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col5);
		if (i == 5) ceasubgrp_ptr =  &(utdvdlgdata_ptr->col6);
		else break;

	        ceasubgrp_ptr->SubGrpNumber = ntohl(ceasubgrp_ptr->SubGrpNumber);
	        ceasubgrp_ptr->row1CeaNumber = ntohl(ceasubgrp_ptr->row1CeaNumber);
	        ceasubgrp_ptr->row2CeaNumber = ntohl(ceasubgrp_ptr->row2CeaNumber);
	        ceasubgrp_ptr->row3CeaNumber = ntohl(ceasubgrp_ptr->row3CeaNumber);
	        ceasubgrp_ptr->row4CeaNumber = ntohl(ceasubgrp_ptr->row4CeaNumber);
	        swap32((char *)&(ceasubgrp_ptr->row1CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row2CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row3CeaValue));
	        swap32((char *)&(ceasubgrp_ptr->row4CeaValue));
	        ceasubgrp_ptr->row1CeaQuality = ntohl(ceasubgrp_ptr->row1CeaQuality);
	        ceasubgrp_ptr->row2CeaQuality = ntohl(ceasubgrp_ptr->row2CeaQuality);
	        ceasubgrp_ptr->row3CeaQuality = ntohl(ceasubgrp_ptr->row3CeaQuality);
	        ceasubgrp_ptr->row4CeaQuality = ntohl(ceasubgrp_ptr->row4CeaQuality);
	   }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

           // copy data from the message to shared memory
	   msg.getData(&(Interface->UtdvDialogue), sizeof(Interface->UtdvDialogue));
	   IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	   break;

	case PROCESS_UTDV_RECEIVE_DATA:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.11
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	   utdvdata_ptr = (UTDV_DATA *)msg.getDataPtr();

	   utdvdata_ptr->nextToLastMidnight.data_available 
		= ntohl(utdvdata_ptr->nextToLastMidnight.data_available);
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.efphFile));
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.efphRpcb));
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.efphColss));
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.efphIncore));
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.efphCeaExposure));
	   swap32((char *)&(utdvdata_ptr->nextToLastMidnight.rpcbBurnup));

	   utdvdata_ptr->lastMidnight.data_available 
		= ntohl(utdvdata_ptr->lastMidnight.data_available);
	   swap32((char *)&(utdvdata_ptr->lastMidnight.efphFile));
	   swap32((char *)&(utdvdata_ptr->lastMidnight.efphRpcb));
	   swap32((char *)&(utdvdata_ptr->lastMidnight.efphColss));
	   swap32((char *)&(utdvdata_ptr->lastMidnight.efphIncore));
	   swap32((char *)&(utdvdata_ptr->lastMidnight.efphCeaExposure));
	   swap32((char *)&(utdvdata_ptr->lastMidnight.rpcbBurnup));

	   utdvdata_ptr->latestHourly.data_available 
		= ntohl(utdvdata_ptr->latestHourly.data_available);
	   swap32((char *)&(utdvdata_ptr->latestHourly.efphFile));
	   swap32((char *)&(utdvdata_ptr->latestHourly.efphRpcb));
	   swap32((char *)&(utdvdata_ptr->latestHourly.efphCeaExposure));
	   swap32((char *)&(utdvdata_ptr->latestHourly.rpcbBurnup));

	   swap32((char *)&(utdvdata_ptr->present.mostRecentEfphFile));
	   swap32((char *)&(utdvdata_ptr->present.oldPresentEfph));
	   swap32((char *)&(utdvdata_ptr->present.pendingPresentEfph));

	   
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaFile12));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaColss12));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaIncore12));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaFile2));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaRpcb2));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaColss2));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaIncore2));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaCeaExposure2));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaFile3));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaRpcb3));
	   swap32((char *)&(utdvdata_ptr->calc.efphDeltaCeaExposure3));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

           msg.getData(&(Interface->UtdvData), sizeof(Interface->UtdvData));
           IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	   break;

	default:
	   
	   cout << "parseUTDVMessages: Received Unknown Message, opcode: " 
	        << hex << msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
   
}
