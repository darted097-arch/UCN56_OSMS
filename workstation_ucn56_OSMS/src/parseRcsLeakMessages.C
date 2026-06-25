//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseRcsLeakMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the RCS Keak messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.10
#include <netinet/in.h> // CORENET 2012.07.10
#endif

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "messageObject.h"
#include <unistd.h>                     // for gethostname()
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

// generic template for the local processing of workstation messages
//
// Local Constant Definitions: NONE
//
#ifndef DEBUG
   #define DEBUG
#endif
//
// Function Prototypes: NONE
//      
//
// ***************************************************************************
//
void parseRcsLeakMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	CCUR_On_Data	*hostondata_ptr;
	WS_On_Data	*wsondata_ptr;
	CCUR_Off_Data	*hostoffdata_ptr;
	WS_Off_Data	*wsoffdata_ptr;
	WS_PRN_Data	*wsprndata_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {
// >>>>>>>>>>>>>>>>>>>>> ON-LINE <<<<<<<<<<<<<<<<<<<<<<<< 	
      // to ccur
      case REQUEST_RCS_LEAK_DATA:	// Picture requested online data from CCUR
#ifdef DEBUG
	 cout << " REQUEST_RCS_LEAK_DATA" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;
#endif
	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->WSOnData), sizeof(Interface->WSOnData));
	 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 wsondata_ptr = (WS_On_Data *)Msg.getDataPtr();
	 wsondata_ptr->TIME = htonl(wsondata_ptr->TIME);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;

	 //  from ccur
      case SEND_RCS_LEAK_DATA:	// CCUR responded to REQUEST_RCS_LEAK_DATA.
#ifdef DEBUG
	 cout << " SEND_RCS_LEAK_DATA" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 hostondata_ptr = (CCUR_On_Data *)Msg.getDataPtr();
	 hostondata_ptr->NEWTIME = ntohl(hostondata_ptr->NEWTIME);
	 hostondata_ptr->SITEVENT = ntohl(hostondata_ptr->SITEVENT);
	 hostondata_ptr->VCTEVENT = ntohl(hostondata_ptr->VCTEVENT);
	 hostondata_ptr->HPSION = ntohl(hostondata_ptr->HPSION);
	 hostondata_ptr->INPUTSTS = ntohl(hostondata_ptr->INPUTSTS);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // copy data from message to shared memory 
	 Msg.getData(&(Interface->HostOnData), sizeof(Interface->HostOnData));

	 cout << "++++++++ RCS ONLINE ++++++++++" << endl;

	 cout << " NEWTIME = " << dec << Interface->HostOnData.NEWTIME << endl;
	 cout << " SITEVENT = " << dec << Interface->HostOnData.SITEVENT << endl;
	 cout << " VCTEVENT = " << dec << Interface->HostOnData.VCTEVENT << endl;
	 cout << " HPSION = " << dec << Interface->HostOnData.HPSION << endl;
	 cout << " INPUTSTS = " << dec << Interface->HostOnData.INPUTSTS << endl;

	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break;
	 
// >>>>>>>>>>>>>>>>>>>>> OFF-LINE <<<<<<<<<<<<<<<<<<<<<<<< 
	 //  to ccur
      case REQUEST_LEAK_OFF_DATA:	// Pictures sends offline inputs to CCUR
#ifdef DEBUG
	 cout << " REQUEST_LEAK_OFF_DATA" << endl;
#endif
	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->WSOffData), sizeof(Interface->WSOffData));
	 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 wsoffdata_ptr = (WS_Off_Data *)Msg.getDataPtr();
	 wsoffdata_ptr->CURPAGE = htonl(wsoffdata_ptr->CURPAGE);
	 swap32((char *)&(wsoffdata_ptr->RXPWRI));
	 swap32((char *)&(wsoffdata_ptr->RXPWRF));
	 swap32((char *)&(wsoffdata_ptr->VCTLVLI));
	 swap32((char *)&(wsoffdata_ptr->VCTLVLF));
	 swap32((char *)&(wsoffdata_ptr->VCTTEMPI));
	 swap32((char *)&(wsoffdata_ptr->VCTTEMPF));
	 swap32((char *)&(wsoffdata_ptr->PZRLVLI));
	 swap32((char *)&(wsoffdata_ptr->PZRLVLF));
	 swap32((char *)&(wsoffdata_ptr->PZRTEMPI));
	 swap32((char *)&(wsoffdata_ptr->PZRTEMPF));
	 swap32((char *)&(wsoffdata_ptr->PZRPRI));
	 swap32((char *)&(wsoffdata_ptr->PZRPRF));
	 swap32((char *)&(wsoffdata_ptr->RCSHOT1I));
	 swap32((char *)&(wsoffdata_ptr->RCSHOT1F));
	 swap32((char *)&(wsoffdata_ptr->RCSCLD1I));
	 swap32((char *)&(wsoffdata_ptr->RCSCLD1F));
	 swap32((char *)&(wsoffdata_ptr->RDTLVLI));
	 swap32((char *)&(wsoffdata_ptr->RDTLVLF));
	 swap32((char *)&(wsoffdata_ptr->RDTTEMPI));
	 swap32((char *)&(wsoffdata_ptr->RDTTEMPF));
	 swap32((char *)&(wsoffdata_ptr->EDTLVLI));
	 swap32((char *)&(wsoffdata_ptr->EDTLVLF));
	 swap32((char *)&(wsoffdata_ptr->EDTTEMPI));
	 swap32((char *)&(wsoffdata_ptr->EDTTEMPF));
	 swap32((char *)&(wsoffdata_ptr->SI1ALVLI));
	 swap32((char *)&(wsoffdata_ptr->SI1ALVLF));
	 swap32((char *)&(wsoffdata_ptr->SI1APRI));
	 swap32((char *)&(wsoffdata_ptr->SI1APRF));
	 swap32((char *)&(wsoffdata_ptr->SI1BLVLI));
	 swap32((char *)&(wsoffdata_ptr->SI1BLVLF));
	 swap32((char *)&(wsoffdata_ptr->SI1BPRI));
	 swap32((char *)&(wsoffdata_ptr->SI1BPRF));
	 swap32((char *)&(wsoffdata_ptr->SI2ALVLI));
	 swap32((char *)&(wsoffdata_ptr->SI2ALVLF));
	 swap32((char *)&(wsoffdata_ptr->SI2APRI));
	 swap32((char *)&(wsoffdata_ptr->SI2APRF));
	 swap32((char *)&(wsoffdata_ptr->SI2BLVLI));
	 swap32((char *)&(wsoffdata_ptr->SI2BLVLF));
	 swap32((char *)&(wsoffdata_ptr->SI2BPRI));
	 swap32((char *)&(wsoffdata_ptr->SI2BPRF));
	 swap32((char *)&(wsoffdata_ptr->CMTAVGI));
	 swap32((char *)&(wsoffdata_ptr->CMTAVGF));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 	 
	 //  from ccur
      case SEND_LEAK_OFF_DATA:	// CCUR responded to REQUEST_LEAK_OFF_DATA
#ifdef DEBUG
	 cout << " SEND_LEAK_OFF_DATA" << endl;
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 hostoffdata_ptr = (CCUR_Off_Data *)Msg.getDataPtr();
	 swap32((char *)&(hostoffdata_ptr->RXPWRI));
	 swap32((char *)&(hostoffdata_ptr->RXPWRF));
	 swap32((char *)&(hostoffdata_ptr->VCTLVLI));
	 swap32((char *)&(hostoffdata_ptr->VCTLVLF));
	 swap32((char *)&(hostoffdata_ptr->VCTTEMPI));
	 swap32((char *)&(hostoffdata_ptr->VCTTEMPF));
	 swap32((char *)&(hostoffdata_ptr->PZRLVLI));
	 swap32((char *)&(hostoffdata_ptr->PZRLVLF));
	 swap32((char *)&(hostoffdata_ptr->PZRTEMPI));
	 swap32((char *)&(hostoffdata_ptr->PZRTEMPF));
	 swap32((char *)&(hostoffdata_ptr->PZRPRI));
	 swap32((char *)&(hostoffdata_ptr->PZRPRF));
	 swap32((char *)&(hostoffdata_ptr->RCSHOT1I));
	 swap32((char *)&(hostoffdata_ptr->RCSHOT1F));
	 swap32((char *)&(hostoffdata_ptr->RCSCLD1I));
	 swap32((char *)&(hostoffdata_ptr->RCSCLD1F));
	 swap32((char *)&(hostoffdata_ptr->RDTLVLI));
	 swap32((char *)&(hostoffdata_ptr->RDTLVLF));
	 swap32((char *)&(hostoffdata_ptr->RDTTEMPI));
	 swap32((char *)&(hostoffdata_ptr->RDTTEMPF));
	 swap32((char *)&(hostoffdata_ptr->EDTLVLI));
	 swap32((char *)&(hostoffdata_ptr->EDTLVLF));
	 swap32((char *)&(hostoffdata_ptr->EDTTEMPI));
	 swap32((char *)&(hostoffdata_ptr->EDTTEMPF));
	 swap32((char *)&(hostoffdata_ptr->SI1ALVLI));
	 swap32((char *)&(hostoffdata_ptr->SI1ALVLF));
	 swap32((char *)&(hostoffdata_ptr->SI1APRI));
	 swap32((char *)&(hostoffdata_ptr->SI1APRF));
	 swap32((char *)&(hostoffdata_ptr->SI1BLVLI));
	 swap32((char *)&(hostoffdata_ptr->SI1BLVLF));
	 swap32((char *)&(hostoffdata_ptr->SI1BPRI));
	 swap32((char *)&(hostoffdata_ptr->SI1BPRF));
	 swap32((char *)&(hostoffdata_ptr->SI2ALVLI));
	 swap32((char *)&(hostoffdata_ptr->SI2ALVLF));
	 swap32((char *)&(hostoffdata_ptr->SI2APRI));
	 swap32((char *)&(hostoffdata_ptr->SI2APRF));
	 swap32((char *)&(hostoffdata_ptr->SI2BLVLI));
	 swap32((char *)&(hostoffdata_ptr->SI2BLVLF));
	 swap32((char *)&(hostoffdata_ptr->SI2BPRI));
	 swap32((char *)&(hostoffdata_ptr->SI2BPRF));
	 swap32((char *)&(hostoffdata_ptr->CMTAVGI));
	 swap32((char *)&(hostoffdata_ptr->CMTAVGF));

	 swap32((char *)&(hostoffdata_ptr->VCTDIFF));
	 swap32((char *)&(hostoffdata_ptr->PZRDIFF));
	 swap32((char *)&(hostoffdata_ptr->RDTDIFF));
	 swap32((char *)&(hostoffdata_ptr->EDTDIFF));
	 swap32((char *)&(hostoffdata_ptr->DIFFSIT));
	 swap32((char *)&(hostoffdata_ptr->RCSDIFF));
	 swap32((char *)&(hostoffdata_ptr->OFDVOL));
	 swap32((char *)&(hostoffdata_ptr->OFLEAK));
	 swap32((char *)&(hostoffdata_ptr->OFIDVOL));
	 swap32((char *)&(hostoffdata_ptr->OFIDLEAK));
	 swap32((char *)&(hostoffdata_ptr->OFUNLEAK));
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->HostOffData), sizeof(Interface->HostOffData));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());
	 break;

// >>>>>>>>>>>>>>>>>>>>> ON & OFF LINE PRINT <<<<<<<<<<<<<<<<<<<<<<<< 
	 
	 //  to ccur
      case REQUEST_LEAKPRN:	// Pictures sends PRINT REQUEST to CCUR
      case REQUEST_MASS_PRN:	// Pictures sends PRINT REQUEST to CCUR
#ifdef DEBUG
	 cout << " REQUEST_LEAKPRN " << endl;
#endif
	 cout << "PAGE NUM = " << Interface->WSPrnData.PAGENUM;

	 gethostname(Interface->WSPrnData.WSNAME,LEN_SYSTEMNAME);
	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->WSPrnData), sizeof(Interface->WSPrnData));
	 
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 wsprndata_ptr = (WS_PRN_Data *)Msg.getDataPtr();
	 wsprndata_ptr->PAGENUM = htonl(wsprndata_ptr->PAGENUM);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	 break;
	 	 
	 //  from ccur
      case CONFIRM_LEAKPRN:	// CCUR responded to REQUEST_LEAKPRN
      case CONFIRM_MASS_PRN:	// CCUR responded to REQUEST_LEAKPRN
#ifdef DEBUG
	 cout << " CONFIRM_LEAKPRN " << endl;
#endif

	 break;

// CORENET ujson add 2017.0221

      case REQUEST_MASS_CALC:
	 cout << " REQUEST_MASS_CALC" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->wsMassCalcData), sizeof(Interface->wsMassCalcData));
	 break;

      case CONFIRM_MASS_CALC:
	 cout << " CONFIRM_MASS_CALC" << endl;
	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->hostMassCalcData), sizeof(Interface->hostMassCalcData));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 // DEBUG
	 cout << " minDesignMF = " << Interface->hostMassCalcData.minDesignMF << endl;
	 cout << " maxDesignMF = " << Interface->hostMassCalcData.maxDesignMF << endl;
	 cout << " minCorrMF = " << Interface->hostMassCalcData.minCorrMF << endl;
	 cout << " maxCorrMF = " << Interface->hostMassCalcData.maxCorrMF << endl;
	 cout << " colssVolF = " << Interface->hostMassCalcData.colssVolF << endl;
	 cout << " colssMassF = " << Interface->hostMassCalcData.colssMassF << endl;
	 cout << " CPC CH.A = " << Interface->hostMassCalcData.cpcCHA << endl;
	 cout << " CPC CH.B = " << Interface->hostMassCalcData.cpcCHB << endl;
	 cout << " CPC CH.C = " << Interface->hostMassCalcData.cpcCHC << endl;
	 cout << " CPC CH.D = " << Interface->hostMassCalcData.cpcCHD << endl;
	 cout << " techResult = " << Interface->hostMassCalcData.techResult << endl;
	 cout << " rcsResult = " << Interface->hostMassCalcData.rcsResult << endl;

	 break;

      case CONFIRM_MASS_MSG:
	 Msg.getData((&Interface->hostMassCalcMsg), sizeof(Interface->hostMassCalcMsg));
         IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 break;

      case REQUEST_MASS_INIT:
	 cout << " REQUEST_MASS_INIT" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->wsMassCalcData), sizeof(Interface->wsMassCalcData));
	 break;
/*
      case REQUEST_MASS_STOP:
	 cout << " REQUEST_MASS_STOP" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 // Put the Interface, shared memory data, into the message
	 break;
*/

// CORENET ujson add End

      default:
	   
	 cout << "parseRcsLeakMessages: Received Unknown Message, opcode: " 
	      << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
