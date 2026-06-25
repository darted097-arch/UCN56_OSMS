//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseGroupMessages.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//	Purpose:	This module processes the Group Display messages.  This includes
//			both messages from the host to the workstation and messages from
//			the workstation to the host.
// ********************************************************************************************	
//
// ********************************************************************************************	
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

// Local Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"
#include "messageObject.h"

// External Variables:

extern	INTERFACE		*Interface;	// shm/CCUR to GUI interface variables
extern  messageQueueObject	IGUI_MsgQue;	// Commlink to InterfaceToGUI message queue

// Prototype Definitions:
   
//
// ********************************************************************************************	
//
void parseGroupMessages(messageObject & Msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   int * req_option_ptr; // 
   GROUP_DISPLAY_TEMPLATE * grpdisp_data_ptr; // 
   WSflagMessageData * grfm_data_ptr; // 
   GROUP_MEMBER_TEMPLATE * grpupdate_data_ptr;
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   
   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   switch (Msg.getOpcode())
   {
      
      // >>>>>>>>>>>>>>>>>>>>  Messages sent from the GUI to the Host <<<<<<<<<<<<<<<<<<<<
      
      case GROUP_INFO_REQUEST:  // to ccur, x-group title, PID, rate....
	{
         Msg.setData(&(Interface->GenericRequestMessage.data.requestOption),
		  sizeof(Interface->GenericRequestMessage.data.requestOption)); 

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 req_option_ptr = (int *)Msg.getDataPtr();
	 *req_option_ptr = htonl(*req_option_ptr);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
      	}
      	
      case GROUP_RESET_REQUEST:  // to ccur, set/reset group no.
	{
         Msg.setData(&(Interface->FlagHandleMessage.data),
		       sizeof(Interface->FlagHandleMessage.data)); 

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 grfm_data_ptr = (WSflagMessageData *)Msg.getDataPtr();
	 grfm_data_ptr->requestOption = htonl(grfm_data_ptr->requestOption);
	 grfm_data_ptr->flipflop1 = htonl(grfm_data_ptr->flipflop1);
	 grfm_data_ptr->flipflop2 = htonl(grfm_data_ptr->flipflop2);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;
      	}

      case GROUP_INFO_DATA:     // from ccur all titles, x-group PID
	 {

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grpdisp_data_ptr = (GROUP_DISPLAY_TEMPLATE *)Msg.getDataPtr(); // 
            grpdisp_data_ptr->group.groupNumber = ntohl(grpdisp_data_ptr->group.groupNumber); // 
            grpdisp_data_ptr->group.printRate = ntohl(grpdisp_data_ptr->group.printRate); // 
            grpdisp_data_ptr->group.timesToPrint = ntohl(grpdisp_data_ptr->group.timesToPrint); // 
            grpdisp_data_ptr->group.displayRate = ntohl(grpdisp_data_ptr->group.displayRate); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    // Change the opcode to tell the InterfaceToGUI to tell the picture to display the data.
	    Msg.setOpcode(DISPLAY_GROUP_DATA);
	    // copy data from message to shared memory.
            Msg.getData(&(Interface->GroupDisplay.data),sizeof(Interface->GroupDisplay.data)); 

	    // terminates point with null character
	    Interface->GroupDisplay.data.group.Point1[8]='\0'; 
	    Interface->GroupDisplay.data.group.Point2[8]='\0';
	    Interface->GroupDisplay.data.group.Point3[8]='\0';
	    Interface->GroupDisplay.data.group.Point4[8]='\0';
	    Interface->GroupDisplay.data.group.Point5[8]='\0';
	    Interface->GroupDisplay.data.group.Point6[8]='\0';
	    Interface->GroupDisplay.data.group.Point7[8]='\0';
	    Interface->GroupDisplay.data.group.Point8[8]='\0';
	    Interface->GroupDisplay.data.group.Point9[8]='\0';
	    Interface->GroupDisplay.data.group.Point10[8]='\0';
	    Interface->GroupDisplay.data.group.Point11[8]='\0';
	    Interface->GroupDisplay.data.group.Point12[8]='\0';
	    Interface->GroupDisplay.data.group.Point13[8]='\0';
	    Interface->GroupDisplay.data.group.Point14[8]='\0';
	    Interface->GroupDisplay.data.group.Point15[8]='\0';
	    Interface->GroupDisplay.data.group.Point16[8]='\0';
	    Interface->GroupDisplay.data.group.Point17[8]='\0';
	    Interface->GroupDisplay.data.group.Point18[8]='\0';
	    Interface->GroupDisplay.data.group.Point19[8]='\0';
	    Interface->GroupDisplay.data.group.Point20[8]='\0';
	    Interface->GroupDisplay.data.group.Point21[8]='\0';
	    Interface->GroupDisplay.data.group.Point22[8]='\0';
	    Interface->GroupDisplay.data.group.Point23[8]='\0';
	    Interface->GroupDisplay.data.group.Point24[8]='\0';
	    Interface->GroupDisplay.data.group.Title[20]='\0';

	    // terminates title with null character
	    Interface->GroupDisplay.data.Titles.Title1[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title2[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title3[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title4[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title5[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title6[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title7[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title8[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title9[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title10[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title11[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title12[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title13[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title14[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title15[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title16[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title17[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title18[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title19[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title20[20]='\0'; 
	    
	    Interface->FlagHandleMessage.data.flipflop2 = 0; 
	    Interface->System.receivedData = TRUE;
	    // send the message with the new opcode to InterfaceToGUI.
	    // InterfaceToGUI will process that the new message through parsePictureMessages.C
	    IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	   
	    //Interface->FlagHandleMessage.data.flipflop2 = 0; 
	    //Interface->System.receivedData = TRUE;
	    //CORENET 20170421 by Philkyu Kwon Debug;
	    cout << "TIMES TO PRINT = " << Interface->GroupDisplay.data.group.timesToPrint <<endl;
	    break;
	 }
	case GROUP_UPDATE: // setup for x-group data to ccur
         {
            gethostname(Interface->GroupMember.data.theWorkstation,LEN_SYSTEMNAME);
            Msg.setData(&(Interface->GroupMember.data),sizeof(Interface->GroupMember.data)); 

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    grpupdate_data_ptr = (GROUP_MEMBER_TEMPLATE *)Msg.getDataPtr(); // 
            grpupdate_data_ptr->groupNumber = htonl(grpupdate_data_ptr->groupNumber); // 
            grpupdate_data_ptr->printRate = htonl(grpupdate_data_ptr->printRate); // 
            grpupdate_data_ptr->displayRate = htonl(grpupdate_data_ptr->displayRate); // 
            grpupdate_data_ptr->timesToPrint = htonl(grpupdate_data_ptr->timesToPrint); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    break;
         }
      case GROUP_CONFIRM: // ccur confirms receipt of new PIDs, etc..from Picture.
	 {
	    // Change the opcode to tell the InterfaceToGUI to tell the picture that the
	    // processing was confirmed.
	    Msg.setOpcode(GROUP_PROCESS_CONFIRMED);

	    Interface->System.receivedData = TRUE;

	    IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	 
	    //Interface->System.receivedData = TRUE;
	    break;
         }
         
      case DISPLAY_GROUP_FLAG: // ccur confirms receipt of new PIDs, etc.. from Picture.
	 {


//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
            grpdisp_data_ptr = (GROUP_DISPLAY_TEMPLATE *)Msg.getDataPtr(); // 
            grpdisp_data_ptr->group.groupNumber = ntohl(grpdisp_data_ptr->group.groupNumber); // 
            grpdisp_data_ptr->group.printRate = ntohl(grpdisp_data_ptr->group.printRate); // 
            grpdisp_data_ptr->group.timesToPrint = ntohl(grpdisp_data_ptr->group.timesToPrint); // 
            grpdisp_data_ptr->group.displayRate = ntohl(grpdisp_data_ptr->group.displayRate); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	    // Change the opcode to tell the InterfaceToGUI to tell the picture that the
	    // processing was confirmed.
	    Msg.setOpcode(DISPLAY_FLAG_RESET);
	    Msg.getData(&(Interface->GroupDisplay.data),sizeof(Interface->GroupDisplay.data));
	    
	    // terminates point with null character
	    Interface->GroupDisplay.data.group.Point1[8]='\0'; 
	    Interface->GroupDisplay.data.group.Point2[8]='\0';
	    Interface->GroupDisplay.data.group.Point3[8]='\0';
	    Interface->GroupDisplay.data.group.Point4[8]='\0';
	    Interface->GroupDisplay.data.group.Point5[8]='\0';
	    Interface->GroupDisplay.data.group.Point6[8]='\0';
	    Interface->GroupDisplay.data.group.Point7[8]='\0';
	    Interface->GroupDisplay.data.group.Point8[8]='\0';
	    Interface->GroupDisplay.data.group.Point9[8]='\0';
	    Interface->GroupDisplay.data.group.Point10[8]='\0';
	    Interface->GroupDisplay.data.group.Point11[8]='\0';
	    Interface->GroupDisplay.data.group.Point12[8]='\0';
	    Interface->GroupDisplay.data.group.Point13[8]='\0';
	    Interface->GroupDisplay.data.group.Point14[8]='\0';
	    Interface->GroupDisplay.data.group.Point15[8]='\0';
	    Interface->GroupDisplay.data.group.Point16[8]='\0';
	    Interface->GroupDisplay.data.group.Point17[8]='\0';
	    Interface->GroupDisplay.data.group.Point18[8]='\0';
	    Interface->GroupDisplay.data.group.Point19[8]='\0';
	    Interface->GroupDisplay.data.group.Point20[8]='\0';
	    Interface->GroupDisplay.data.group.Point21[8]='\0';
	    Interface->GroupDisplay.data.group.Point22[8]='\0';
	    Interface->GroupDisplay.data.group.Point23[8]='\0';
	    Interface->GroupDisplay.data.group.Point24[8]='\0';
	    Interface->GroupDisplay.data.group.Title[20]='\0';

	    // terminates title with null character
	    Interface->GroupDisplay.data.Titles.Title1[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title2[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title3[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title4[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title5[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title6[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title7[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title8[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title9[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title10[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title11[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title12[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title13[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title14[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title15[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title16[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title17[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title18[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title19[20]='\0'; 
	    Interface->GroupDisplay.data.Titles.Title20[20]='\0'; 
	    
	    
	    //IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());
	    
//   Msg.getData(&(Interface->GRConfirmMessage.data),sizeof(Interface->GRConfirmMessage.data)); //org.
//   Interface->GRConfirmMessage.data.confirmMessage[39] = '\0' ;

            Interface->FlagHandleMessage.data.flipflop2 = 1;
	    Interface->System.receivedData = TRUE;

	    IGUI_MsgQue.sendMessage(Msg.pMsg, Msg.getMsgSize(), Msg.getType());

	    break;

	 }
	 
	 
   } // end of opcode switch statement

}
