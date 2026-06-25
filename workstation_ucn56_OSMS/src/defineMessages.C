//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// defineMessages.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module performs the initialization for the Interface structure
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
//
// External Variables:
//
//
// Local Constant Definitions:
//
#define PDAT_DIGITAL_TREND      "DigitalTrendStructure.pdat"
#define PDAT_MESSAGES		"messageStructure.pdat"
#define PDAT_PEN_RECORDER	"penRecorderStructure.pdat"
#define PDAT_RPCCEA             "RPCCEAStructure.pdat"
#define PDAT_COLSS              "colssStructure.pdat"
#define PDAT_INCORE             "incoreStructure.pdat"
#define PDAT_DATABASE_DISPLAYS  "databaseDisplaysStructure.pdat"
#define PDAT_BAR_CHART          "BarChartStructure.pdat"
#define PDAT_SECURE             "secureStructure.pdat"
#define PDAT_XRBP               "XRBPStructure.pdat"
#define PDAT_XRBPA              "XRBPAUTO.pdat"
#define PDAT_BOPDMD             "BOPDMDStructure.pdat"
#define PDAT_UTDV               "UTDVStructure.pdat"
#define PDAT_GROUP_DISPLAY      "groupDisplayStructure.pdat"
#define PDAT_POINT_SUMMARY      "pointSummaryStructure.pdat"
#define PDAT_PRINT_REPORTS      "printReportsStructure.pdat"
#define PDAT_CECOR              "cecorStructure.pdat"
#define PDAT_CSLLOG             "cslStructure.pdat"
#define PDAT_VTREND_DISPLAY     "VideoTrendStructure.pdat"
#define PDAT_CAL_DISPLAY	"CALStructure.pdat"
#define PDAT_BDL_DISPLAY        "BDLStructure.pdat"
#define PDAT_ORL_DISPLAY        "ORLStructure.pdat"
#define PDAT_LINK_DISPLAY       "LinkStructure.pdat"
#define PDAT_RCS_LEAK           "RCSLeakStructure.pdat"
#define PDAT_CEATRACKMONITOR    "CEATrackMonitorStructure.pdat"
// CORENET ujson add 2017.0220
#define PDAT_EQUIP    		"EQUIPStructure.pdat"
#define PDAT_PRINTPID    	"printPIDTestStructure.pdat"

//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void defineMessages ()
{
   char	filename[100];		// used to open pdat file
	    			   
   // Begin shared memory message structure definition
	
   cout      << "Defining the Message Structures >>" << endl;
	
   //Compute the generic message structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_MESSAGES);
   
   cout      << "   The Generic Message Structures pdat File: " << filename << endl;
	
   // Read the message structure into the rtm
   PfReadScript(filename);
   
   //Compute the pen recorder structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_PEN_RECORDER);
		
   cout      << "   The Pen Recorder Messages Structure pdat File: " << filename << endl;
	
   // Read the pen recorder structure into the rtm
   PfReadScript(filename);
   
   //Compute the RPCCEA structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_RPCCEA);
		
   cout      << "   The RPCCEA Messages Structure pdat File: " << filename << endl;
	
   // Read RPCCEA structure into the rtm
   PfReadScript(filename);

   //Compute the digital trend structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_DIGITAL_TREND);

   cout      << "   The Digital Trend Messages Structure pdat File: " << filename << endl;

   // Read the digital trend structure into the rtm
   PfReadScript(filename);

   //Compute the colss structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_COLSS);

   cout      << "   The COLSS Messages Structure pdat File: " << filename << endl;

   // Read the COLSS structure into the rtm
   PfReadScript(filename);

   //Compute the Bar Chart structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_BAR_CHART);
		
   cout      << "   The Barchart Messages Structure pdat File: " << filename << endl;
	
   // Read the Barchart structure into the rtm
   PfReadScript(filename);

   //Compute the incore structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_INCORE);

   cout      << "   The Incore Messages Structure pdat File: " << filename << endl;

   // Read the Incore structure into the rtm
   PfReadScript(filename);   
   
   //Compute the secure structure pdat file.   
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_SECURE);

   // Read the Secure structure into the rtm
   PfReadScript(filename);      

   cout      << "   The Secure Messages Structure pdat File: " << filename << endl;
	
   //Compute the database displays structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_DATABASE_DISPLAYS);

   // Read the database display structure into the rtm
   PfReadScript(filename);

   cout      << "   The Database Displays Messages Structure pdat File: " << filename << endl;

   //Compute the database displays structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,"HDSRStructure.pdat");

   cout      << "   thlee HDSR pdat File: " << filename << endl;

   // Read the pen recorder structure into the rtm
   PfReadScript(filename);

   cout      << "End of Defining HDSRStructure.pdat" << endl;

   //Compute the BOP Per Calculations structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_BOPDMD);
		
   cout      << "   The BOP Per Calculations Messages Structure pdat File: " << filename << endl;
	
   // Read the BOP Per Calculations structure into the rtm
   PfReadScript(filename);
	   
   //Compute the UTDV structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_UTDV);

   // Read the UTDV structure into the rtm
   PfReadScript(filename);
   cout      << "   The UTDV Messages Structure pdat File: " << filename << endl;

   //Compute the group display structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_GROUP_DISPLAY);
		
   cout      << "   The Group Display Messages Structure pdat File: " << filename << endl;
	
   // Read the group display structure into the rtm
   PfReadScript(filename);   
   

   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_XRBP);

   // Read the XRBP structure into the rtm
   PfReadScript(filename);

   cout      << "   The XRBP Messages Structure pdat File: " << filename << endl;

   //Compute the XRBPAUTO Message structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_XRBPA);

   // Read the XRBPA structure into the rtm
   PfReadScript(filename);

   cout      << "   The XRBP AUTO Messages Structure pdat File: " << filename << endl;
   
   //Compute the point summary structure pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_POINT_SUMMARY);
		
   cout      << "   The Point Summary Messages Structure pdat File: " << filename << endl;
	
   // Read the point summary structure into the rtm
   PfReadScript(filename);   

   //Commpute the print reports pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_PRINT_REPORTS);
  
   cout      << "   The PRINT REPORTS Messages Structure pdat File: " << filename << endl;
     	    		 
   // Read the Postrip structure into the rtm
   PfReadScript(filename);

   //Commpute the cecor pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_CECOR);

   cout      << "   The CECOR Messages Structure pdat File: " << filename << endl;

   // Read the cecor structure into the rtm
   PfReadScript(filename);

   //Commpute the csllog pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_CSLLOG);

   cout      << "   The CSLLOG Messages Structure pdat File: " << filename << endl;

   // Read the CSLLOG structure into the rtm
   PfReadScript(filename);

   //Commpute the video trend pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_VTREND_DISPLAY);

   PfReadScript(filename);

   cout      << "   The Video Trend Messages Structure pdat File: " << filename << endl;
   
      //Commpute the Current Alarm List pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_CAL_DISPLAY);

   PfReadScript(filename);

   cout      << "   The Current Alarm List Message Structure pdat File: " << filename << endl;

     //Commpute the Bad Data List pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_BDL_DISPLAY);

   PfReadScript(filename);

   cout      << "   The Bad Data List Message Structure pdat File: " << filename << endl;

     //Commpute the Out of Range List pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_ORL_DISPLAY);

   PfReadScript(filename);

   cout      << "   The Out of Range List Message Structure pdat File: " << filename << endl;

     //Commpute the Out of Range List pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_LINK_DISPLAY);

   PfReadScript(filename);

   cout      << "   The Link Status Message Structure pdat File: " << filename << endl;

//Commpute the RCS Leak Structure pdat file.(by stc)--------------------------------------
   cout      << "   The RCS Leak Messages Structure pdat File: " << filename << endl;
   strcpy(filename,getenv("HOME"));
   cout      << "   The RCS Leak Messages Structure pdat File: " << filename << endl;
   strcat(filename,PICTURE_DIR);
   cout      << "   The RCS Leak Messages Structure pdat File: " << filename << endl;
   strcat(filename,PDAT_RCS_LEAK);
   cout      << "   The RCS Leak Messages Structure pdat File: " << filename << endl;
   
// Read the RCSLeak structure into the rtm.(by stc)-----------------------------------------

   PfReadScript(filename);

   cout      << "   The RCS Leak Messages Structure pdat File: " << filename << endl;
   
 
//Compute the CEATrackMonitor pdat file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_CEATRACKMONITOR);

   PfReadScript(filename);

   cout      << "   The CEA Track Monitor Message Structure pdat File: " << filename << endl;

   // CORENET ujson add 2017.0220
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_EQUIP);

   PfReadScript(filename);

   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_PRINTPID);

   PfReadScript(filename);


   cout      << "End of Defining Shared Memory Message Structure" << endl;
}
