//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseCEATrackMonitorMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the CEA Track Monitor messages.  This includes
//  messages from the host to the workstation.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
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
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//      

//
// ***************************************************************************
//
void parseCEATrackMonitorMessages(messageObject & msg)
//
// >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
{
    switch (msg.getOpcode())
    {
     // from ccur
    case CONFIRM_CEA_TRACK_MONITOR1:
	  msg.getData(&(Interface->CEATrackingMonitor1.IndiCeaMonitor1Data), sizeof(Interface->CEATrackingMonitor1.IndiCeaMonitor1Data));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR2:
	  msg.getData(&(Interface->CEATrackingMonitor2.IndiCeaMonitor2Data), sizeof(Interface->CEATrackingMonitor2.IndiCeaMonitor2Data));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR3:
	  msg.getData(&(Interface->CEATrackingMonitor3.SubDeviMonitorData), sizeof(Interface->CEATrackingMonitor3.SubDeviMonitorData));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR4:
	  msg.getData(&(Interface->CEATrackingMonitor4.CPCTargetMonitorData), sizeof(Interface->CEATrackingMonitor4.CPCTargetMonitorData));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR5:
	  msg.getData(&(Interface->CEATrackingMonitor5.CEAPosComparisionData), sizeof(Interface->CEATrackingMonitor5.CEAPosComparisionData));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR6:
	  msg.getData(&(Interface->CEATrackingMonitor6.value), sizeof(Interface->CEATrackingMonitor6.value));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
    case CONFIRM_CEA_TRACK_MONITOR9:
	  msg.getData(&(Interface->CEATrackingMonitor9.value), sizeof(Interface->CEATrackingMonitor9.value));
	  IGUI_MsgQue.sendMessage(msg.pMsg, msg.getMsgSize(), msg.getType());
	  flush(cout);
	  break;
	  
     // to ccur
    case REQUEST_CEA_TRACK_MONITOR1:
	  msg.setData(&(Interface->CEATrackingMonitor1), sizeof(Interface->CEATrackingMonitor1));
	  cout<<"REQUEST_CEA_TRACK_MONITOR1"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR2:
	  msg.setData(&(Interface->CEATrackingMonitor2), sizeof(Interface->CEATrackingMonitor2));
	  cout<<"REQUEST_CEA_TRACK_MONITOR2"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR3:
	  msg.setData(&(Interface->CEATrackingMonitor3), sizeof(Interface->CEATrackingMonitor3));
	  cout<<"REQUEST_CEA_TRACK_MONITOR3"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR4:
	  msg.setData(&(Interface->CEATrackingMonitor4), sizeof(Interface->CEATrackingMonitor4));
	  cout<<"REQUEST_CEA_TRACK_MONITOR4"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR5:
	  msg.setData(&(Interface->CEATrackingMonitor5), sizeof(Interface->CEATrackingMonitor5));
	  cout<<"REQUEST_CEA_TRACK_MONITOR5"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR6:
          msg.setData(&(Interface->CEATrackingMonitor6), sizeof(Interface->CEATrackingMonitor6));
	  cout<<"REQUEST_CEA_TRACK_MONITOR6"<<endl;
	  break;
    case REQUEST_CEA_TRACK_MONITOR9:
          msg.setData(&(Interface->CEATrackingMonitor9), sizeof(Interface->CEATrackingMonitor9));
	  cout<<"REQUEST_CEA_TRACK_MONITOR9"<<endl;
	  break;


    };
};
