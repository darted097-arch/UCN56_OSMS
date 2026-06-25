//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// defineInterface.C    
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
#include "interface.h"			// interface variables
//
// External Variables:
//
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
// Local Constant Definitions:
//
#define PDAT_INTERFACE	"interfaceStructure.pdat"
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void defineInterface ()
{
   char	filename[100];		// used to open pdat file
	    			   
   // Begin shared memory interface structure definition
	
   // Compute the interface structure pdat filename.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_INTERFACE);
	
   cout      << " Defining the Interface Structure pdat File: " << filename << endl;
	
   // Read the message structure into the rtm
   PfReadScript(filename);
  		    	   	           
   cout      << " End of Defining the Interface Structure" << endl;

   // Now attach the interface structure to the rtm	   
   Interface->System.PicassoId = PfCreateVar("Interface", PfCRecord, "INTERFACE", TRUE, &Interface[0]);
   

}
