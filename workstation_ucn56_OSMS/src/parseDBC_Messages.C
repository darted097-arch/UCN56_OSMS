//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseDBC_Messages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the Database Displays messages. These functions are 
//  Change Scan, Change Alarm, Change Limits, Insert Value, Sensor Calibration,
//  and Least Squares Fit.  This includes messages from the host to the 
//  workstation, and messages from the workstation to the host.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>	// CORENET
#include <netinet/in.h>	// CORENET
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
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

//
// Local Constant Definitions:
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void parseDBC_Messages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.19
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   DB_CHANGE_DATA *db_change_data_ptr;
   LSF_INF *lsf_inf_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (Msg.getOpcode())
   {
	 //  to ccur from Workstation, i.e. InterfaceToGUI processing
      case PROCESS_CHGSCN_DATA:	// Pictures sends new PID, etc. to CCUR
      case PROCESS_CHGALM_DATA:	// Pictures sends new PID, etc. to CCUR
      case PROCESS_CHGLIM_DATA:	// Pictures sends new PID, etc. to CCUR
      case PROCESS_INSVAL_DATA:	// Pictures sends new PID, etc. to CCUR
      case PROCESS_SENCAL_DATA:	// Pictures sends new PID, etc. to CCUR
      case PROCESS_LSF_DATA:	// Pictures sends new PID, etc. to CCUR
#ifdef DEBUG
	 cout << "parseDBC_Messages: processing PROCESS_xxxxxx_DATA" << endl;
	 cout << " Interface.Pid = " << Interface->DatabaseDisplaysData.PID;
	 cout << endl;	 
#endif
	 
	 // get the Interface data into the message
	 Msg.setData(&Interface->DatabaseDisplaysData,
				sizeof(Interface->DatabaseDisplaysData));
	 
#ifdef DEBUG
	 WSmessageTemplate tempBuf;
	 Msg.getMessage(&tempBuf, sizeof(tempBuf));
	 cout << "Msg.pData[0:7] = " << tempBuf.data[0];
	 cout << tempBuf.data[1] << tempBuf.data[2];
	 cout << tempBuf.data[3] << tempBuf.data[4];
	 cout << tempBuf.data[5] << tempBuf.data[6];
	 cout << tempBuf.data[7] << endl;	 
#endif
//////////////////////////////////////////////////////////////
// CORENET 2012.07.19
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 db_change_data_ptr = (DB_CHANGE_DATA *)Msg.getDataPtr();
	 db_change_data_ptr->ChgScnStatus = htonl(db_change_data_ptr->ChgScnStatus);
	 db_change_data_ptr->ChgAlmStatus = htonl(db_change_data_ptr->ChgAlmStatus);
	 swap32((char *)&(db_change_data_ptr->InsValue));
	 swap32((char *)&(db_change_data_ptr->LoLoLim));
	 swap32((char *)&(db_change_data_ptr->LoLim));
	 swap32((char *)&(db_change_data_ptr->HiLim));
	 swap32((char *)&(db_change_data_ptr->HiHiLim));
	 swap32((char *)&(db_change_data_ptr->SensorCalValue));
	 db_change_data_ptr->degree = htonl(db_change_data_ptr->degree);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
	 	 

      case REQUEST_LSF_SCOUNT:
         Msg.setData(&Interface->LSF_Info_Data, sizeof(Interface->LSF_Info_Data));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.19
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 lsf_inf_ptr = (LSF_INF *)Msg.getDataPtr();
	 swap32((char *)&(lsf_inf_ptr->HiSLim));
	 swap32((char *)&(lsf_inf_ptr->LoSLim));
	 lsf_inf_ptr->PolyOrder = htonl(lsf_inf_ptr->PolyOrder);
	 lsf_inf_ptr->Rtn_Status = htonl(lsf_inf_ptr->Rtn_Status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;			
	 
      case RETURN_LSF_SCOUNT:
//////////////////////////////////////////////////////////////
// CORENET 2012.07.19
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 lsf_inf_ptr = (LSF_INF *)Msg.getDataPtr();
	 swap32((char *)&(lsf_inf_ptr->HiSLim));
	 swap32((char *)&(lsf_inf_ptr->LoSLim));
	 lsf_inf_ptr->PolyOrder = ntohl(lsf_inf_ptr->PolyOrder);
	 lsf_inf_ptr->Rtn_Status = ntohl(lsf_inf_ptr->Rtn_Status);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

         Msg.getData(&Interface->LSF_Info_Data, sizeof(Interface->LSF_Info_Data));
         Msg.setOpcode(LSF_SCOUNT_CONFIRMED);
         // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	 break;
	 	 
	 //  from ccur, i.e. CommLink processing
      case CONFIRM_CHGSCN_PROCESSING:	// CCUR confirms processing database 
      case CONFIRM_CHGALM_PROCESSING:	
      case CONFIRM_CHGLIM_PROCESSING:	
      case CONFIRM_INSVAL_PROCESSING:	
      case CONFIRM_SENCAL_PROCESSING:	
      case CONFIRM_LSF_PROCESSING:	
	 // Change the opcode to tell InterfaceToGUI to tell the picture 
	 //that the processing was confirmed.
	 switch (Msg.getOpcode())
	 {
	    case CONFIRM_CHGSCN_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_CHGSCN_PROCESSING" << endl;
#endif
	       Msg.setOpcode(CHGSCN_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_CHGALM_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_CHGALM_PROCESSING" << endl;
#endif
	       Msg.setOpcode(CHGALM_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_CHGLIM_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_CHGLIM_PROCESSING" << endl;
#endif
	       Msg.setOpcode(CHGLIM_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_INSVAL_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_INSVAL_PROCESSING" << endl;
#endif
	       Msg.setOpcode(INSVAL_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_SENCAL_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_SENCAL_PROCESSING" << endl;
#endif
	       Msg.setOpcode(SENCAL_PROCESSING_CONFIRMED);
	       break;
	    case CONFIRM_LSF_PROCESSING:
#ifdef DEBUG
	       cout << "parseDatabaseDisplaysMessages: processing CONFIRM_LSF_PROCESSING" << endl;
#endif
	       Msg.setOpcode(LSF_PROCESSING_CONFIRMED);
	       Msg.getData(&Interface->LSF_Info_Data, sizeof(Interface->LSF_Info_Data));
	       break;
	    default:	// can't happen
	       break;
	 } // endswitch

	 // Send the message with the new opcode to InterfaceToGUI
	 // InterfaceToGUI will process that new message through parsePictureMessages.C
	 IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	 break;

      default:
	  cout << "parseDatabaseDisplaysMessages: Received Unknown Message, opcode: " 
	       << hex << Msg.getOpcode() << endl;
	 
   } // end of opcode switch statement
}
