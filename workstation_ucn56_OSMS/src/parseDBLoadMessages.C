//
// **********************************************
// hbnpp 3&4 pms display 
// 
// parseDBLoadMessages.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module processes the database load messages from the 
//  host to the workstation
// ***************************************************************************
//
// System Include Files:
//
#include <ctype.h>
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
#include "messageObject.h"		// generic workstation message object
#include "DatabaseObject.h"
//
// External Variables:
//
extern	DatabaseObject	DBO;

// Local Constant Definitions:
//
//#ifndef DEBUG
//   #define DEBUG
//#endif
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//   
void parseDBLoadMessages(messageObject & Msg)
{
  int  i,j;			// loop counters
  int  ipn;			// internal point number index
  int  numberOfPoints;		// number of points to process
   
  union {
     WSmessageTemplate		message;
     digitalDownloadMessage   	digitalDbLoad;		// Digital Database load message
     analogDownloadMessage   	analogDbLoad;		// Analog Database load message
  } Local;
      
  // copy the message into the local union so that it can be accessed as various structures
  Msg.getMessage((void *)&Local.message, sizeof(Local.message));
  
  switch (Msg.getOpcode())              		// process based upon message type
  {
     // >>>>>>>>>>>>>>>>>>>>>>>>>> DEFINE_DIGITAL <<<<<<<<<<<<<<<<<<<<<<<<<<
     case UPDATE_DB_DIGI:
     case DEFINE_DIGITAL:
        numberOfPoints = Msg.getNumberOfSomething();
        for (i = 0; i< numberOfPoints; i++)
        {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           	Local.digitalDbLoad.data[i].ipn = ntohs(Local.digitalDbLoad.data[i].ipn);
           	Local.digitalDbLoad.data[i].pipBitmap = ntohs(Local.digitalDbLoad.data[i].pipBitmap);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

           ipn = (int)Local.digitalDbLoad.data[i].ipn;
	    
           // copy point id string and terminate.
	   // the Concurrent always sends LENGTH_PID bytes.  Keep the 
	   // alphanumeric characters and terminate with null.
	   for (j = 0; j < LENGTH_PID; j++)
	   {
	      if (isspace (Local.digitalDbLoad.data[i].pid[j]) )
	      {
		  // fix up the pid from the message to be null terminated
		  Local.digitalDbLoad.data[i].pid[j] =  '\0';
		  break;
	      }// endif
	   }// endfor
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	   if (ipn, Local.digitalDbLoad.data[i].pid[0] == '\0')
		    continue;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	   // set the pid in the database object
	   DBO.setpid(ipn, Local.digitalDbLoad.data[i].pid);
     
           // set english description in the online database
	   DBO.setdesc(ipn, Local.digitalDbLoad.data[i].ed);
 
           // set tag id in the online database
	   DBO.settagid(ipn, Local.digitalDbLoad.data[i].tagid);
 
           // set displayed tag id in the online database
	   DBO.setpipbitmap(ipn, (int)Local.digitalDbLoad.data[i].pipBitmap);
	      
	   DBO.setsig_type(ipn, (int)(Local.digitalDbLoad.data[i].pipBitmap & bTYPE));
                  
           // set message pairs
	   DBO.setmessage1(ipn, Local.digitalDbLoad.data[i].message1);
	   DBO.setmessage2(ipn, Local.digitalDbLoad.data[i].message2);

           // set the display format to 0
	   DBO.setdisfmt(ipn, 0);	// CORENET 2012.07.09
	   //DBO.setdisfmt(ipn, 0);	// CORENET 2012.07.09
	} // end for	
        break;

      // >>>>>>>>>>>>>>>>>>>>>>>>>> DEFINE_ANALOG <<<<<<<<<<<<<<<<<<<<<<<<<<
      case UPDATE_DB_ANAL:
      case DEFINE_ANALOG:
         numberOfPoints = Msg.getNumberOfSomething();
         for (i = 0; i< numberOfPoints; i++)
         {
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
           	Local.analogDbLoad.data[i].ipn = ntohs(Local.analogDbLoad.data[i].ipn);	// short
           	Local.analogDbLoad.data[i].pipBitmap = ntohs(Local.analogDbLoad.data[i].pipBitmap);	// short
           	Local.analogDbLoad.data[i].disfmt = ntohs(Local.analogDbLoad.data[i].disfmt);	// short
           	Local.analogDbLoad.data[i].pad = ntohs(Local.analogDbLoad.data[i].pad);	// short

		swap32((char *)&(Local.analogDbLoad.data[i].erange_lo));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].erange_hi));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].hhval));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].hval));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].lval));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].llval));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].hhmet));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].hmet));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].lmet));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].llmet));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].mfactor));	// float
		swap32((char *)&(Local.analogDbLoad.data[i].moffset));	// float

           	Local.analogDbLoad.data[i].lIPN = ntohs(Local.analogDbLoad.data[i].lIPN);	// short
           	Local.analogDbLoad.data[i].hIPN = ntohs(Local.analogDbLoad.data[i].hIPN);	// short
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            ipn = (int)Local.analogDbLoad.data[i].ipn;

            // copy point id string and terminate.
	    // the Concurrent always sends LENGTH_PID bytes.  Keep the alphanumeric
	    // characters and insert the null string for the rest
	    for (j = 0; j < LENGTH_PID; j++)
	    {
	       if (isspace (Local.analogDbLoad.data[i].pid[j]) )
	       {
		  // fix up the pid from the message to be a null terminated string
		  Local.analogDbLoad.data[i].pid[j] =  '\0';
		  break;
	       }// endif
	    }// endfor
	    // set the pid in the database object
	    DBO.setpid(ipn, Local.analogDbLoad.data[i].pid);
     
            // copy english description string and terminate
	    DBO.setdesc(ipn, Local.analogDbLoad.data[i].ed);
 
            // copy tag id string and terminate
 	    // Review this in final implementation for final sizes
	    DBO.settagid(ipn, Local.analogDbLoad.data[i].tagid);
 
            // copy displayed tag id string and terminate
	    // Review this in final implementation for final sizes
	    DBO.setpipbitmap(ipn, (int)Local.analogDbLoad.data[i].pipBitmap);
	    DBO.setsig_type(ipn, (int)(Local.analogDbLoad.data[i].pipBitmap & bTYPE));
               	     
	    // copy english and metric engineering units strings and terminate
	    DBO.seteng_units(ipn, Local.analogDbLoad.data[i].eng_units);
	    DBO.setmet_units(ipn, Local.analogDbLoad.data[i].met_units);
	    DBO.setdisfmt(ipn, (int)Local.analogDbLoad.data[i].disfmt);
	      
	    // engineering range
	    DBO.seterange_lo(ipn, Local.analogDbLoad.data[i].erange_lo);
	    DBO.seterange_hi(ipn, Local.analogDbLoad.data[i].erange_hi);
	    DBO.setmfactor(ipn, Local.analogDbLoad.data[i].mfactor);
	    DBO.setmoffset(ipn, Local.analogDbLoad.data[i].moffset);
	    
	    // english alarm limits
	    DBO.sethhval(ipn, Local.analogDbLoad.data[i].hhval);
	    DBO.sethval(ipn, Local.analogDbLoad.data[i].hval);
	    DBO.setlval(ipn, Local.analogDbLoad.data[i].lval);
	    DBO.setllval(ipn, Local.analogDbLoad.data[i].llval);
	    
	    // metric alarm limits
	    DBO.sethhmet(ipn, Local.analogDbLoad.data[i].hhmet);
	    DBO.sethmet(ipn, Local.analogDbLoad.data[i].hmet);
	    DBO.setlmet(ipn, Local.analogDbLoad.data[i].lmet);
	    DBO.setllmet(ipn, Local.analogDbLoad.data[i].llmet);
	      
	    DBO.setlIPN(ipn, Local.analogDbLoad.data[i].lIPN);
	    if(0 != DBO.getlIPN(ipn))
	    {
	       cout << "parseDBLoadMessages: " << dec << ipn
		    << " pid " << DBO.getpid(ipn) 
		    << " has a variable low setpoint = whose ipn = " << endl
		    << DBO.getlIPN(ipn) << " pid = " 
		    << DBO.getpid(DBO.getlIPN(ipn)) << " value = "
		    << DBO.getvalue(DBO.getlIPN(ipn))
		    << endl;
	    }
	    DBO.sethIPN(ipn, Local.analogDbLoad.data[i].hIPN);
	    if(0 != DBO.gethIPN(ipn))
	    {
	       cout << "parseDBLoadMessages: " << dec << ipn 
		    << " pid " << DBO.getpid(ipn) 
		    << " has a variable high setpoint = whose ipn = " << endl
		    << DBO.gethIPN(ipn) << " pid = " 
		    << DBO.getpid(DBO.gethIPN(ipn)) << " value = "
		    << DBO.getvalue(DBO.gethIPN(ipn))
		    << endl;
	    }
         }//endfor
         break;
      default:
	 break;
   }// end of switch statement 
}// end of parseDBLoadMessages
