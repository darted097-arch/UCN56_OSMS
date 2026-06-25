//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parsePictureMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// This file parses messages for pictures.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <sys/msg.h>

//
// User Defined Include Files:
//
#include "messageParameters.h"
#include "databaseParameters.h"
#include "DatabaseObject.h"
#include "messages.h"
#include "systemParameters.h"
#include "messageObject.h"
#include "interface.h"
#include "TempShoppingListObject.h"
#include "PermShoppingListObject.h"
#include "PicPidListObject.h"
#include "DataQueueObject.h"
//
// External Variables:
//
extern	INTERFACE		*Interface;
extern	PermShoppingListObject	PermShoppingList; // Global Perm Shopping List
extern	TempShoppingListObject	TempShoppingList; // Global Temp Shopping List
extern  PicPidListObject        PicPidList;
extern  DatabaseObject          DBO;
extern  char                    curPicture[]; // name of current picture
//
// Local Constant Definitions: NONE
//
static messageObject	PPmsg(FROM_HOST_MSG_ID, "parsePictureMessagesMsg");
//
// Function Prototypes: NONE
//
#include "prototypes.h"
#include "parseHDSRMessages.h"

struct CAL_DATA1{
        int             CalPage;
        CAL_MEMBER      CALine[15];
       };

struct BDL_DATA1{
        int             BdlPage;
        BDL_MEMBER      BDLine[15];
        };

struct ORL_DATA1{
        int             OrlPage;
        ORL_MEMBER      ORLine[15];
        };
//
// ***************************************************************************
//

void parsePictureMessages(DataQueueObject & DQ)
{
    CAL_DATA1 *pCALData;
    BDL_DATA1 *pBDLData;
    ORL_DATA1 *pORLData;

    size_t	len;
    int 	line;
    int        j;
    int        local_ipn;
    
    // Buffer for processing the message as a WS_Colss_Message structure
    WS_Colss_Message    cmsg;
  
    while(!(DQ.isEmpty()))
    {
        len = DQ.removeItem(DQO_WS_MESSAGE_TYPE, (void *)PPmsg.pMsg, 
					 (size_t)PPmsg.getmaxMsgSize());
	PPmsg.setMsgSize(len);  
//#ifdef DEBUG
	cout << "CORENET parsePictureMessages(): Received message.  Opcode = ";
	cout << hex << PPmsg.getOpcode() << endl;
//#endif
	switch(PPmsg.getOpcode())
	{
            case DISPLAY_DTREND_DATA:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture to display the data from the Host\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                        "DTREND DATA RECEIVED FROM HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case DTREND_PROCESSING_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PROCESSED BY HOST");  
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case DTREND_PRT_PROCESS_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture that the host has processed the data and print\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PROCESSED BY HOST"); // yrlee 010925
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case INUSE_DTRD_RESPONSE_CONFIRM:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}", " ");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case COMPLETED_DTRD_PRT_CONFIRM:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PRINT COMPLETED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case INTERRUPT_DTRD_PRT_CONFIRM:
#ifdef DEBUG
                cout << "Tell the Digital Trend picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PRINT INTERRUPTED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case PRINT_SINGLE_POINT_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Single Point Display picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message1);
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

	    case ACK_PROCESSING_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the picture to display Alarm Acknowledge Confirmation from the Host\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
			"ALARM ACKNOWLEDGE PROCESSED BY HOST");
		PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
			" \0");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
			" \0");
		//    PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;
		
	    case RESET_PROCESSING_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the picture to display Alarm Reset Confirmation from the Host\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
			"ALARM RESET PROCESSED BY HOST");
		PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
		        " \0");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
		        " \0");
		//    PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;
		
	    case DISPLAY_PEN_DATA:
#ifdef DEBUG
		cout << "Tell the Pen Recorder picture to display the data from the Host\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
			"PEN RECORDER DATA RECEIVED FROM HOST");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				" ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				" ");
        	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;
		  
	    case SEND_CAL_DATA:
	       
	       if (0 != strcmp (curPicture,"PicUCN_CAL"))
	       {
        	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
	       }
		else  
	       {
//		cout << " parsePictureMessages pic name is " << curPicture;
#ifdef DEBUG
		cout << "Add CAL points to temp shopping list\n";
#endif
	         pCALData = (CAL_DATA1 *)&(Interface->CAL_Data);
	         TempShoppingList.wsinit();
	         for (line=1; line<=15; line++)
	         {
	            if ( (0 != strcmp (pCALData->CALine[line-1].Time,"        ") ) && 
                       (8 == strlen(pCALData->CALine[line-1].Time) ))
			{
			   TempShoppingList.add(pCALData->CALine[line-1].AlarmPid);
			   local_ipn = find_pid(pCALData->CALine[line-1].AlarmPid);
// if ipn is -1, blank out time for Tdoc file
      	             	   if (local_ipn == -1)
			   {
			      cout << "parsePictureMessages::SEND_CAL: PID not found = " 
				   << pCALData->CALine[line-1].AlarmPid << endl;
			      for (j = 0; j < 8; j++)
			      {
				 pCALData->CALine[line-1].Time[j] = ' ';
			      }
			      pCALData->CALine[line-1].Time[9] = '\0';
			   }
			   else
			   {
         	            if ( DBO.isanalog(local_ipn) )
                            {
			       if (0 != DBO.gethIPN(local_ipn))
			       {
	                          TempShoppingList.add(DBO.getpid(DBO.gethIPN(local_ipn)));
			       }
			       if   (0 !=  DBO.getlIPN(local_ipn))
			       {
	                         
          		          TempShoppingList.add(DBO.getpid( DBO.getlIPN(local_ipn)));
         			}
			    }
			 }
			}
		 }
	       }
		break;

            case SEND_BDL_DATA:
	       
	       if (0 != strcmp (curPicture,"PicUCN_BDL"))
	       {
        	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
	       }
		else  
	       {
//		cout << " parsePictureMessages pic name is " << curPicture;
#ifdef DEBUG
                cout << "Tell the Bad Data List picture to display the data from the Host\n";
#endif
	         pBDLData = (BDL_DATA1 *)&(Interface->BDL_Data);
	         TempShoppingList.wsinit();
	         for (line=1; line<=15; line++)
	         {
	            if ( (0 != strcmp (pBDLData->BDLine[line-1].AlarmProb,"    ") ) && 
                       (4 == strlen(pBDLData->BDLine[line-1].AlarmProb) ))
			{
	                    TempShoppingList.add(pBDLData->BDLine[line-1].AlarmPid);
			}
		 }
	       }
                break; 
                 
            case SEND_ORL_DATA:
	       
	       if (0 != strcmp (curPicture,"PicUCN_ORL"))
	       {
        	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
	       }
		else  
	       {
//		cout << " parsePictureMessages pic name is " << curPicture;
#ifdef DEBUG
                cout << "Tell the Out of Range List picture to display the data from the Host\n";
#endif
	         pORLData = (ORL_DATA1 *)&(Interface->ORL_Data);
	         TempShoppingList.wsinit();
	         for (line=1; line<=15; line++)
	         {
	            if ( (0 != strcmp (pORLData->ORLine[line-1].AlarmProb,"    ") ) && 
                       (4 == strlen(pORLData->ORLine[line-1].AlarmProb) ))
			{
	                    TempShoppingList.add(pORLData->ORLine[line-1].AlarmPid);
			}
		 }
	       }
                break;
		  
            case SEND_LINK_STATUS:
#ifdef DEBUG
        	cout << "Tell the Link Status Page to display the data from the Host\n";
#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;
		
	    case PEN_PROCESSING_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the Pen Recorder picture that the host has processed the data\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"CHANGE(S) ACCEPTED");
			//	"REQUEST PROCESSED BY HOST");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				" ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				" ");
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;

            case DISPLAY_BAR_DATA:
                cout << "Tell the Bar Chart picture to display the data from the Host\n";
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                        "BAR DATA RECEIVED FROM HOST");
                PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
                break;
                 
            case BAR_PROCESSING_CONFIRMED:
                cout << "Tell the Bar Chart picture that the host has processed the data\n";
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PROCESSED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
                break;
  
	    case REQ_SHOPPING_LISTS:
	    {
#ifdef DEBUG
		cout << " IGUI received shopping list request " << endl;
#endif
		// Create Picture Point ID list from .Tdoc files
                PicPidList.createPicturePidLists();

		// create the shopping list from user requests and send a
		//message to CommLink to send the list to the host.
		TempShoppingList.create();

                PermShoppingList.buildPermanentShoppingList(curPicture);

		break;
	    }



            case XRBP_READY:
#ifdef DEBUG
                cout << "parsePicture: Tell the display initialization done" << endl;
#endif
//                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
//                        "INITIALIZATION COMPLETED");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;
	
            case XRBP_DATA:
#ifdef DEBUG
                cout << "Tell the XRBP Offline that host has processed the data\n";
                PfExecute(NULL,"{displayPicture(\"Main\",\"./PicUNN_XRBP_Off2.ppic\");}");
#endif
//                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
//                        "REQUEST PROCESSED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case XRBPA_DISPLAY:
#ifdef DEBUG
                cout << "Tell the XRBPAUTO that host has processed the data\n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;
                
            case XRBP_MSG:
#ifdef DEBUG
                cout << "Tell the footer that the host has processed the request\n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
//              PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
//                              Interface->GenericConfirmMessage.data.message1);
//              PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
//                              Interface->GenericConfirmMessage.data.message2);
//              PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
//                              Interface->GenericConfirmMessage.data.message3);
//              PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());                            
                break;
            

	    case CHGSCN_PROCESSING_CONFIRMED:
	    case CHGALM_PROCESSING_CONFIRMED:
	    case CHGLIM_PROCESSING_CONFIRMED:
	    case INSVAL_PROCESSING_CONFIRMED:
	    case SENCAL_PROCESSING_CONFIRMED:
	    case LSF_PROCESSING_CONFIRMED:
	    case LSF_SCOUNT_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the DBC Picture that the host has processed the request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"REQUEST PROCESSED BY HOST");
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;

	    case GENERIC_PROCESSING_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the request\n";
#endif
		//CORENET 0706
		//PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
		//		Interface->GenericConfirmMessage.data.message1);
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}", Interface->GenericConfirmMessage.data.message1);
		PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}", Interface->GenericConfirmMessage.data.message2);
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}", Interface->GenericConfirmMessage.data.message3);
		break;

	    case CONFIRM_SECURE_PROCESSING:
#ifdef DEBUG
		cout << "Tell the Signon Picture that the host has processed the request\n";
#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;

	    case REQUEST_SECURE_SIGNOFF:
//#ifdef DEBUG
		cout << "Tell the Signon Picture that the host has processed the signoff request\n";
//#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;

	    case DISPLAY_SECURITY_PAGE:
#ifdef DEBUG
		cout << "Tell the Warm Start Picture to display the Signon Picture\n";
#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;

	    case BOP_PRINT_CONFIRMED:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the request\n";
#endif
		//CORENET 20170623
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

		//PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
		//		Interface->BOPDMD_CONFIRM_MESSAGE.data.message1);
		break;
		
	    case CONFIRM_CECOR_REQUEST:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the CECOR request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"CPC AND CEAC DATA       ");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				"HAVE BEEN REQUESTED FOR CECOR");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				"WAITING FOR I/O TO COMPLETE  ");
		break;
	    case CONFIRM_CPCEAC_REQUEST:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the CPC/CEAC request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"CPC AND CEAC DATA            ");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				"HAVE BEEN REQUESTED           ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				"WAITING FOR I/O TO COMPLETE   ");
		break;
	    case CONFIRM_CPCS_REQUEST:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the CPCS request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"CPC DATA HAS BEEN REQUESTED     ");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				"                                ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				"WAITING FOR I/O TO COMPLETE     ");
		break;
	    case CONFIRM_CEAC_REQUEST:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the CPCS request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"CEAC DATA HAS BEEN REQUESTED   ");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				"                                ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				"WAITING FOR I/O TO COMPLETE     ");
		break;
	    case REJECT_CPCS_REQUEST:
#ifdef DEBUG
		cout << "Tell the footer that the host has processed the CPCS request\n";
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
				"REQUEST HAS BEEN REJECTED      ");
		PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
				"PREVIOUS I/O HAS NOT COMPLETED! ");
		PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
				"TRY AGAIN IN 30 SECONDS         ");
		break;
	    case DEMANDLOG_PRINT_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Print reports  picture that the host has processed demand log print \n";		
#endif
		PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
		                "DEMAND LOG SENT TO PRINTER");
		break;                
	
	    case DISPLAY_POSTRIP_DATA:
#ifdef DEBUG
	        cout << "Tell the selected Postrip picture that the host has sent the postrip data \n";
#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;
		
            case PROCESS_UTDV_RECEIVE:
#ifdef DEBUG
                cout << "Tell the UTDV Picture that the host has processed the request\n";
#endif
                PfExecute(NULL,"{doUTDVAction();}");
                break;

	    case DISPLAY_SOE_DATA:
#ifdef DEBUG
	        cout << "Tell the selected soe picture that the host has sent the soe data \n";
#endif
		PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
		break;	
		
            case POSTRIP_PRINT_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the selected Postrip picture that the host has processed print \n";	
#endif
            	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());	        
        	break;
            
            case SOE_PRINT_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the selected SOE picture that the host has processed print \n";	
#endif
            	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());		        
               	break;
            
            case LASTPOSTRIP_PRINT_CONFIRMED:
#ifdef DEBUG
                cout << "last postrip printing is confirmed\n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());  		        
               	break;	
            		        
            case LASTSOE_PRINT_CONFIRMED:
#ifdef DEBUG
                cout << "last soe printing is confirmed\n";
#endif
            	PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode()); 	        
            	break;

//cecor begin by jky

           case CONFIRM_CECOR_DISKFILE:
#ifdef DEBUG
               cout <<"tell cecor pictures cecor snapshot disk file is completed!";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
               PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "CECOR SNAPSHOT DISK FILE COMPLETED");
               break;

           case CONFIRM_CECOR_TAPE:
#ifdef DEBUG
               cout <<"tell cecor pictures cecor snapshot tape is completed!";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}",PPmsg.getOpcode());
               PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "CECOR SNAPSHOT TAPE COMPLETED. PLEASE REMOVE TAPE");
               break;

           case ERROR_QUEUE_IOCPCS:
#ifdef DEBUG
               cout << "Tell quing S task failed \n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

           case ERROR_CECOR_FILEOPEN:
#ifdef DEBUG
               cout << "Tell that cecor file can not be open\n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

           case ERROR_CECOR_MAG1OPEN:
#ifdef DEBUG
               cout << "Tell that mag tape can not be open\n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

           case ERROR_CECOR_DISKREAD:
#ifdef DEBUG
               cout << "Tell that disk reading is failed \n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

           case ERROR_TAPE_NOT_MOUNT:
#ifdef DEBUG
               cout << "Tell that tape not mounted \n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

           case ERROR_CECOR_MAG1WRITE:
#ifdef DEBUG
               cout << "Tell that writing to mag tape is failed \n";
#endif
               PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
               break;

// CECOR END 
//
//
// DSPM Begin 

        case    ERROR_QUEUE_TASK:
#ifdef DEBUG
                cout << "Tell that quing task has failed \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

        case    CONFIRM_CPC_REPORT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that the host has processed CPC report print \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                "CPC CROSS CHANNEL COMPARISON SENT TO PRINTER");
                break;

        case    CONFIRM_CEAC_REPORT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that the host has processed CEAC report print \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                "CEAC CROSS CHANNEL COMPARISON SENT TO PRINTER");
                break;

        case    CONFIRM_PPS_REPORT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that the host has processed PPS report print \n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                "PPS DEV AND SETPNT MONITORING SENT TO PRINTER");
                break;

        case    CONFIRM_CPC_TRP_RPT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that the host has processed CPC TRIP report print \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                "DSPM REPORT FOR CPC TRIP BUFFER SENT TO PRINTER");
                break;

        case    CONFIRM_CEAC_TRP_RPT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that the host has processed CEAC TRIP report print \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                "DSPM REPORT FOR CEAC TRIP BUFFER SENT TO PRINTER");
                break;

        case    ERROR_OPEN_PRINTER:
#ifdef DEBUG
                cout << "Tell that cannot open printer \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

        case    CONFIRM_QUEUE_DSPMRPT:
#ifdef DEBUG
                cout << "Tell the DSPM  picture that selection 3 queued DSPMRPT task \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "ACCEPTED REQUEST=3");
                break;

// DSPM End 


// CSLLOG Begin 
        case    SEND_CSL_DISPLAY:
#ifdef DEBUG
                cout << "new computer status list mesagese has come to ws \n";
#endif
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

// CSLLOG End By JKY

            case CONFIRM_COLSS_PROCESSING:

                PPmsg.getMessage(&cmsg, sizeof(cmsg));

                char *msg2; msg2=cmsg.ColssData.MsgData.message2;
                if((strcmp(msg2,"COLSS MODE OF THIS CPU MUST BE SCHED."))!=0)
                {
                   PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                  cmsg.ColssData.MsgData.message1);
                }

                PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
                               cmsg.ColssData.MsgData.message2);
                PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
                               cmsg.ColssData.MsgData.message3);
                cout << "parsePictureMessages(): Received message.  COLSS Opcode = ";
                cout << hex << cmsg.header.opcode << endl;

                cout << "parsePictureMessages(): Received message.  message_type = ";
                cout <<  cmsg.ColssData.message_type << endl;

                cout << "parsePictureMessages(): Received message.  ws_to_host_mode = ";
                cout <<  cmsg.ColssData.ModeData.ws_to_host_mode << endl;

                cout << "parsePictureMessages(): Received message.  colss_current_mode = ";
                cout <<  cmsg.ColssData.ModeData.colss_current_mode << endl;

                cout << "parsePictureMessages(): Received message.  test_case = ";
                cout <<  cmsg.ColssData.ModeData.test_case << endl;

                cout << "parsePictureMessages(): Received message.  use_burnup_file = ";
                cout <<  cmsg.ColssData.ModeData.use_burnup_file << endl;

                cout << "parsePictureMessages(): Received message.  print_option = ";
                cout << hex << cmsg.ColssData.ModeData.print_option << endl;

                cout << "parsePictureMessages(): Received message.  msg1 = ";
                cout << hex << cmsg.ColssData.MsgData.message1 << endl;

                cout << "parsePictureMessages(): Received message.  msg2 = ";
                cout <<  cmsg.ColssData.MsgData.message2  << endl;

                cout << "parsePictureMessages(): Received message.  msg3 = ";
                cout <<  cmsg.ColssData.MsgData.message3  << endl;
                cout << "parsePictureMessages(): Received message.  plot_status =";
                cout <<  cmsg.ColssData.plot_status << endl;

                PfExecute(NULL,"{doPictureAction(%d);}",cmsg.ColssData.message_type );
                break;

            case DISPLAY_VTREND_DATA:
#ifdef DEBUG
                cout << "Tell the Video Trend picture to display the data from the Host\n";
#endif

// commented out by yrlee 981112
//              PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
//                        "VIDEO TREND DATA RECEIVED FROM HOST");

                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case VTREND_PROCESS_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Video Trend picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PROCESSED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case DISPLAY_VTREND_RESET:
#ifdef DEBUG
                cout << "Tell the VTREND picture that the host can not process the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "CAN NOT MODIFY IT NOW(BEING USED)");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case DISPLAY_GROUP_DATA:
#ifdef DEBUG
                cout << "Tell the Group Display picture to display the data from the Host\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                        "GROUP DISPLAY DATA RECEIVED FROM HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case GROUP_PROCESS_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the Group Display picture that the host has processed the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "REQUEST PROCESSED BY HOST");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;

            case DISPLAY_FLAG_RESET:
#ifdef DEBUG
                cout << "Tell the Group Display picture that the host can not process the data\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage(\"%s\");}",
                                "CAN NOT MODIFY IT NOW(BEING USED)");
                PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
                break;


//    For CEA application --- Red Roof 7/22/97

            case CEA_GENERIC_PROCESS_CONFIRMED:
#ifdef DEBUG
                cout << "Tell the footer that the host has processed the request\n";
#endif
                PfExecute(NULL,"{DisplayAdvisoryMessage1(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message1);

                PfExecute(NULL,"{DisplayAdvisoryMessage2(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message2);

                PfExecute(NULL,"{DisplayAdvisoryMessage3(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message3);
                break;

            case CEA_MESSAGE_IN_BODY:
#ifdef DEBUG
                cout << "Tell the footer that the host has processed the request\n";
#endif
                PfExecute(NULL,"{DisplayCEAMessage1(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message1);

                PfExecute(NULL,"{DisplayCEAMessage2(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message2);

                PfExecute(NULL,"{DisplayCEAMessage3(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message3);

                PfExecute(NULL,"{DisplayCEAMessage4(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message4);

                PfExecute(NULL,"{DisplayCEAMessage5(\"%s\");}",
                                Interface->GenericConfirmMessage.data.message5);
                break;

//    End of CEA general application message processing

      case HDSR_INITr:
      case HDSR_PAGE_710r:
      case HDSR_PAGE_720r:
      case HDSR_PAGE_730r:
      case HDSR_PAGE_740r:
      case HDSR_PAGE_750r:

      case GRAPH_SETUP_74Xr:
      case GRAPH_VIEW_74Xr:
      case GRAPH_VIEW_LEFTr:
      case GRAPH_VIEW_RIGHTr:
      case GRAPH_VIEW_UPr:
      case GRAPH_VIEW_DOWNr:
      case GRAPH_VIEW_DONE:

      case TAPE_HEADER_74Xr:
      case TAPE_HEADER_750r:

      case HDSR_DISK_PROTECTr:
      case HDSR_BACKUPr:
      case HDSR_CANCEL_LOGr:
      case HDSR_TAPE_HEADERr:
      case HDSR_LOGr:
      case HDSR_SECTORr:
      case HDSR_GRAPH_VIEWr:
      case HDSR_GRAPH_SETUPr:
      case DYN_MESSAGEr:
      case HDSR_FOOTER_MESSAGEr:

         parseHDSRMessage_receive( PPmsg );

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;

      case PB_STATUS_CHANGE_ON:
         cout << "parsePictureMessages() received PB_STATUS_CHANGE_ON "
              << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;
         
// RCS LEAK--------------------------------------------------------------------------------
      case SEND_RCS_LEAK_DATA:
         cout << "parsePictureMessages() received SEND_RCS_LEAK_DATA " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;         

       case SEND_LEAK_OFF_DATA:
         cout << "parsePictureMessages() received SEND_LEAK_OFF_DATA " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   
	
	// CORENET ujson add 2017.0316
       case CONFIRM_MASS_CALC:
         cout << "parsePictureMessages() received CONFIRM_MASS_CALC " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   

       case CONFIRM_MASS_MSG:
         cout << "parsePictureMessages() received CONFIRM_MASS_MSG " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   
// PID TEST -------------------------------------------------------------------------------------
// CORENET ujson add 2017.0316
       case CONFIRM_PIDTEST:
         cout << "parsePictureMessages() received CONFIRM_PIDTEST " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   
// EQUIPMENT-------------------------------------------------------------------------------------
// CORENET ujson add 2017.0316
       case CONFIRM_EQUIPVALCHG:
         cout << "parsePictureMessages() received CONFIRM_EQUIPVALCHG " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   
       case CONFIRM_EQUIPSTATUS:
         cout << "parsePictureMessages() received CONFIRM_EQUIPSTATUS " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   
       case CONFIRM_EQUIPPRN:
         cout << "parsePictureMessages() received CONFIRM_EQUIPPRN " << "message" << endl;

         PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
         break;   


//   20061116 For CEATRACK 
      case CONFIRM_CEA_TRACK_MONITOR1:
      case CONFIRM_CEA_TRACK_MONITOR2:
      case CONFIRM_CEA_TRACK_MONITOR3:
      case CONFIRM_CEA_TRACK_MONITOR4:
      case CONFIRM_CEA_TRACK_MONITOR5:
      case CONFIRM_CEA_TRACK_MONITOR6:
      case CONFIRM_CEA_TRACK_MONITOR9:
	   PfExecute(NULL,"{doPictureAction(%d);}", PPmsg.getOpcode());
	   break;

      default:
           cout << "parsePictureMessages: Invalid opcode in message\n" 
		<< endl;
	    cout << " opcode = " << PPmsg.getOpcode() << endl;
            break;
	} //endswitch
    } //endwhile
}

