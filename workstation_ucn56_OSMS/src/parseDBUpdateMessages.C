//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseDBUpdateMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the database update messages from the 
//  host to the workstation
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <fstream.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "database.h"			// database variables
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "messageObject.h"
#include "DatabaseObject.h"

// added for OORH,OORL, BAD // yrlee 010926
#include "interface.h"	
#include "InterProcSMObject.h"

//
// Global Variables:
//
extern INTERFACE * Interface;

//
// External Variables:
//
extern	DatabaseObject	DBO;

//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void parseDBUpdateMessages(messageObject & Msg)
// Although the message object is not changed, input is as a reference to a 
//messageObject for consistency with other similar parse routines, and to 
//allow the message to be changed if desired.
{
   int  i;			// loop counter
   int	ipn;			// internal point number index
   int  numberOfPoints;		// number of points to process
   
   union {
      WSmessageTemplate		message; // big enough to hold biggest message
      digitalUpdateMessage	digitalUpdate; // Digital DB update message
      analogUpdateMessage	analogUpdate; // Analog Database update message
      databaseUpdateMessage	databaseUpdate; // database update message
   } Local;
   
   //  copy the message into the local version 
   // so that it can be accessed in as various structures.
   Msg.getMessage(&(Local.message), sizeof(Local.message));

  switch (Msg.getOpcode())	// decode based upon message type
  {
      
     // >>>>>>>>>>>>>>>>>>>>>>>>>>> UPDATE_DIGITAL <<<<<<<<<<<<<<<<<<<<<<<<<<< 
     case UPDATE_DATABASE:
	numberOfPoints = Msg.getNumberOfSomething();

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	 for (i = 0; i < numberOfPoints; i++) {
            Local.databaseUpdate.data[i].ipn = ntohs(Local.databaseUpdate.data[i].ipn);
            Local.databaseUpdate.data[i].pisaBitmap = ntohs(Local.databaseUpdate.data[i].pisaBitmap);
	    swap32((char *)&(Local.databaseUpdate.data[i].Value));
            //cout << " ^^^^^^^^^^^^^^^^^^ " << dec << i << endl;
            //cout << " ^ " << dec << Local.databaseUpdate.data[i].ipn << endl;
            //cout << " ^ " << dec << Local.databaseUpdate.data[i].pisaBitmap << endl;
            //cout << " ^ " << dec << Local.databaseUpdate.data[i].Value << endl;
	 }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//	cout << "============================================" << endl;

	for(i = 0; i <numberOfPoints; i++)
	{
	   ipn = (int)Local.databaseUpdate.data[i].ipn;
	   DBO.setpisabitmap(ipn, (int)Local.databaseUpdate.data[i].pisaBitmap);
	   //201028_jhp
	   //DBO.setnewsta(ipn, (int)(Local.databaseUpdate.data[i].pisaBitmap & bNEWSTA));
	   if(DBO.isanalog(ipn))
	   {
//            if(Local.databaseUpdate.data[i].pisaBitmap & bUNRELI)

// FOR BAD_VALUE for Network disconnect... yrlee 010926 
// This routine can be activated 
//	      if(Interface->System.connected==0)   // BAD when Network disconnected...
//            {
//		 DBO.setvalue(ipn, BAD_VALUE);
//                cout << "Network Disconnected. "
//	               << "Setting Value for analog ipn " << dec << ipn 
//                     << " to " << DBO.getvalue(ipn) << endl;
//	      }
//            else 
//       
	      if( (!((int)Local.databaseUpdate.data[i].pisaBitmap&bOOSCAN) > 0) && 
                  ((((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == BDACK)||
                   (((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == BDNAK)  ) )
              {
		 DBO.setvalue(ipn, BAD_VALUE);
//                 cout << "     setting Value for analog ipn " << dec << ipn 
//                      << " to " << DBO.getvalue(ipn) << endl;
	      }
// FOR OORH PROCESSING... yrlee010915 
              else if( (!((int)Local.databaseUpdate.data[i].pisaBitmap&bOOSCAN) > 0) && 
                  ((((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == ORHACK)||
                   (((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == ORHNAK)  ) )
              {
		 DBO.setvalue(ipn, OORH_VALUE);
//                 cout << "     setting Value for analog ipn " << dec << ipn 
//                      << " to " << DBO.getvalue(ipn) << endl;
	      }
// FOR OORL PROCESSING... yrlee010915 
              else if( (!((int)Local.databaseUpdate.data[i].pisaBitmap&bOOSCAN) > 0) && 
                  ((((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == ORLACK)||
                   (((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == ORLNAK)  ) )
              {
		 DBO.setvalue(ipn, OORL_VALUE);
//                 cout << "     setting Value for analog ipn " << dec << ipn 
//                      << " to " << DBO.getvalue(ipn) << endl;
	      }
              else
	      {
                 DBO.setvalue(ipn, Local.databaseUpdate.data[i].Value);
              }

//	      if (!strncmp("RCYX", DBO.getpid(ipn), 4)) {
//	        cout << "*** A PID = " << DBO.getpid(ipn) << endl;
//	        cout << "*** A PID VALUE = " << DBO.getvalue(ipn) << endl;
//	      }
	   } // endif analog
	   else
	   {
//            if(Local.databaseUpdate.data[i].pisaBitmap & bUNRELI)

              if( (!((int)Local.databaseUpdate.data[i].pisaBitmap&bOOSCAN) > 0) && 
                  ((((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == BDACK)||
                   (((int)Local.databaseUpdate.data[i].pisaBitmap&bNEWSTA) == BDNAK)  ) )
              {
		 DBO.setvalue(ipn, BAD_VALUE);
//                 cout << "     setting Value for digital ipn " << dec << ipn 
//                      << " to " << DBO.getvalue(ipn) << endl;
              }
              else
              {
	         DBO.setvalue(ipn, 
                              (float)((Local.databaseUpdate.data[i].pisaBitmap 
                              & bCURSTT) > 0));
              }
//	      cout << "*** D PID = " << DBO.getpid(ipn) << endl;
//	      cout << "*** D PID VALUE = " << DBO.getvalue(ipn) << endl;
	   } // endif digital
	} //endfor number of points
	break;

     //This should never be reached. It should be caught in the calling program
     default:	
	cout << "parseDBUpdateMessages:Received Unknown Message, opcode: " 
	     << hex << Msg.getOpcode() << endl;
	break;
	 
  } // end of opcode switch statement
     
}//end parseDBUpdateMessages
