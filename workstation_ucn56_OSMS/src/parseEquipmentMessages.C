//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
//
// parseEquipmentMessages.C
// ver 1.0 -- 2017.02.21.
// **********************************************
//
// ***************************************************************************
//  This module processes the Equipment messages.  This includes
//  messages from the host to the workstation, and messages from
//  the workstation to the host.
// ***************************************************************************

// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

// Local Include Files:
#include "systemParameters.h"           // generic system parameters
#include "databaseParameters.h"         // database parameters
#include "messageParameters.h"          // message parameters
#include "messages.h"                   // message variables
#include "interface.h"                  // interface variables
#include "messageQueueObject.h"
#include "messageObject.h"

// External Variables:

extern  INTERFACE               *Interface;     // shm/CCUR to GUI interface variables
extern  messageQueueObject      IGUI_MsgQue;    // Commlink to InterfaceToGUI message queue

// Prototype Definitions:

//
// ********************************************************************************************
//

void parseEquipmentMessages(messageObject & Msg)
{

   switch (Msg.getOpcode())
   {

      // ***** EQUIPMENT CALCULATION VALUE CHANGE ******
      case REQUEST_EQUIPVALCHG:
	 cout << " REQUEST_EQUIPVALCHG" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->wsEquipValChgData), sizeof(Interface->wsEquipValChgData));

	 cout << "Day = " << Interface->wsEquipValChgData.numDay << endl;
	 cout << "Month = " << Interface->wsEquipValChgData.numMonth << endl;
	 cout << "Year = " << Interface->wsEquipValChgData.numYear << endl;
	 cout << "Run Day = " << Interface->wsEquipValChgData.runDay << endl;
	 cout << "Run Month = " << Interface->wsEquipValChgData.runMonth << endl;
	 cout << "Run Year = " << Interface->wsEquipValChgData.runYear << endl;

	 break;

      case CONFIRM_EQUIPVALCHG:
	 cout << " CONFIRM_EQUIPVALCHG" << endl;
	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->hostEquipMsg), sizeof(Interface->hostEquipMsg));

	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 break;

      // ***** EQUIPMENT RUNNING STATUS TO CRT ******
      case REQUEST_EQUIPSTATUS:
	 cout << " REQUEST_EQUIPSTATUS" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 break;

      case CONFIRM_EQUIPSTATUS:
	 cout << " CONFIRM_EQUIPSTATUS" << endl;
	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->hostEquipStatusData), sizeof(Interface->hostEquipStatusData));

	 // BUFF TRSNSFER
	 // CCUR_CurStatus_Data_BUF
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.reactorCurStatus, Interface->hostEquipStatusData.hostCurStatusData.reactorCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.reactorCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.turGenCurStatus, Interface->hostEquipStatusData.hostCurStatusData.turGenCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.turGenCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp1aCurStatus, Interface->hostEquipStatusData.hostCurStatusData.rcp1aCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp1aCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp1bCurStatus, Interface->hostEquipStatusData.hostCurStatusData.rcp1bCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp1bCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2aCurStatus, Interface->hostEquipStatusData.hostCurStatusData.rcp2aCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2aCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2bCurStatus, Interface->hostEquipStatusData.hostCurStatusData.rcp2bCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2bCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp01pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwp01pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp01pCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp02pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwp02pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp02pCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp03pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwp03pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp03pCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp04pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwbp04pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp04pCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp05pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwbp05pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp05pCurStatus[12]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp06pCurStatus, Interface->hostEquipStatusData.hostCurStatusData.mfwbp06pCurStatus, 12);
	 Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwbp06pCurStatus[12]='\0';

	 // CCUR_StartDate_Data_BUF
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.reactorStartDate, Interface->hostEquipStatusData.hostStartDateData.reactorStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.reactorStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.turGenStartDate, Interface->hostEquipStatusData.hostStartDateData.turGenStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.turGenStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1aStartDate, Interface->hostEquipStatusData.hostStartDateData.rcp1aStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1aStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1bStartDate, Interface->hostEquipStatusData.hostStartDateData.rcp1bStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1bStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.rcp2aStartDate, Interface->hostEquipStatusData.hostStartDateData.rcp2aStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.rcp2aStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.rcp2bStartDate, Interface->hostEquipStatusData.hostStartDateData.rcp2bStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.rcp2bStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp01pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwp01pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp01pStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp02pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwp02pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp02pStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp03pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwp03pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp03pStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp04pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwbp04pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp04pStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp05pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwbp05pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp05pStartDate[10]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp06pStartDate, Interface->hostEquipStatusData.hostStartDateData.mfwbp06pStartDate, 10);
	 Interface->hostEquipStatusDataBuf.hostStartDateData.mfwbp06pStartDate[10]='\0';

	 // CCUR_StartTime_Data_BUF
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.reactorStartTime, Interface->hostEquipStatusData.hostStartTimeData.reactorStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.reactorStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.turGenStartTime, Interface->hostEquipStatusData.hostStartTimeData.turGenStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.turGenStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1aStartTime, Interface->hostEquipStatusData.hostStartTimeData.rcp1aStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1aStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1bStartTime, Interface->hostEquipStatusData.hostStartTimeData.rcp1bStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1bStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp2aStartTime, Interface->hostEquipStatusData.hostStartTimeData.rcp2aStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp2aStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp2bStartTime, Interface->hostEquipStatusData.hostStartTimeData.rcp2bStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp2bStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp01pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwp01pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp01pStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp02pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwp02pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp02pStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp03pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwp03pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwp03pStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp04pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwbp04pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp04pStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp05pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwbp05pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp05pStartTime[8]='\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp06pStartTime, Interface->hostEquipStatusData.hostStartTimeData.mfwbp06pStartTime, 8);
	 Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp06pStartTime[8]='\0';

	 // CCUR_RunningTime_Data_BUF
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.reactorRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.reactorRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.turGenRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.turGenRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.rcp1aRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.rcp1aRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.rcp1bRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.rcp1bRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.rcp2aRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.rcp2aRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.rcp2bRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.rcp2bRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwp01pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwp01pRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwp02pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwp02pRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwp03pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwp03pRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwbp04pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwbp04pRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwbp05pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwbp05pRunningTime;
	 Interface->hostEquipStatusDataBuf.hostRunningTimeData.mfwbp06pRunningTime = Interface->hostEquipStatusData.hostRunningTimeData.mfwbp06pRunningTime;

	 // CCUR_Equip_Msg_BUF
	 strncpy(Interface->hostEquipStatusDataBuf.hostEquipMsg.message1, Interface->hostEquipStatusData.hostEquipMsg.message1, 40);
	 Interface->hostEquipStatusDataBuf.hostEquipMsg.message1[39] = '\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostEquipMsg.message2, Interface->hostEquipStatusData.hostEquipMsg.message2, 40);
	 Interface->hostEquipStatusDataBuf.hostEquipMsg.message1[39] = '\0';
	 strncpy(Interface->hostEquipStatusDataBuf.hostEquipMsg.message3, Interface->hostEquipStatusData.hostEquipMsg.message3, 40);
	 Interface->hostEquipStatusDataBuf.hostEquipMsg.message1[39] = '\0';


	 Msg.setData(&(Interface->hostEquipStatusDataBuf), sizeof(Interface->hostEquipStatusDataBuf));

	 Msg.getData((&Interface->hostEquipStatusDataBuf), sizeof(Interface->hostEquipStatusDataBuf));


	 // TEST
	 cout << "Interface->hostEquipStatusDataBuf.hostCurStatusData.reactorCurStatus = " << Interface->hostEquipStatusDataBuf.hostCurStatusData.reactorCurStatus << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2aCurStatus = " << Interface->hostEquipStatusDataBuf.hostCurStatusData.rcp2aCurStatus << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp03pCurStatus = " << Interface->hostEquipStatusDataBuf.hostCurStatusData.mfwp03pCurStatus << endl;

	 cout << "Interface->hostEquipStatusDataBuf.hostStartDateData.turGenStartDate = " << Interface->hostEquipStatusDataBuf.hostStartDateData.turGenStartDate << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1bStartDate = " << Interface->hostEquipStatusDataBuf.hostStartDateData.rcp1bStartDate << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp03pStartDate = " << Interface->hostEquipStatusDataBuf.hostStartDateData.mfwp03pStartDate << endl;

	 cout << "Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1aStartTime = " << Interface->hostEquipStatusDataBuf.hostStartTimeData.rcp1aStartTime << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp05pStartTime = " << Interface->hostEquipStatusDataBuf.hostStartTimeData.mfwbp05pStartTime << endl;

	 cout << "Interface->hostEquipStatusDataBuf.hostEquipMsg.message1 = " << Interface->hostEquipStatusDataBuf.hostEquipMsg.message1 << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostEquipMsg.message2 = " << Interface->hostEquipStatusDataBuf.hostEquipMsg.message2 << endl;
	 cout << "Interface->hostEquipStatusDataBuf.hostEquipMsg.message3 = " << Interface->hostEquipStatusDataBuf.hostEquipMsg.message3 << endl;

	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 Interface->System.receivedData = TRUE;

	 break;

      // ***** EQUIPMENT RUNNING STATUS TO PRINTER ******
      case REQUEST_EQUIPPRN:
	 cout << " REQUEST_EQUIPPRN" << endl;
	 cout << "opcode= " << hex << Msg.getOpcode() << endl;

	 gethostname(Interface->wsEquipPrnData.WSNAME,LEN_SYSTEMNAME);
	 // Put the Interface, shared memory data, into the message
	 Msg.setData(&(Interface->wsEquipPrnData), sizeof(Interface->wsEquipPrnData));

	 break;

      case CONFIRM_EQUIPPRN:
	 cout << " CONFIRM_EQUIPPRN" << endl;
	 // Get the data from the message into shared memory
	 Msg.getData((&Interface->hostEquipMsg), sizeof(Interface->hostEquipMsg));
	 IGUI_MsgQue.sendMessage(Msg.pMsg,Msg.getMsgSize(),Msg.getType());

	 break;

      default:
         cout << "parsePidForTestMessages: Received Unknown Message, opcode: " << hex << Msg.getOpcode() << endl;
         break;
   }

}
