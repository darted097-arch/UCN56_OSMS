//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// defineDatabase.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module performs the initialization for the database structure
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
#include "database.h"			// database structure
#include "DatabaseObject.h"
//
// External Variables:
//
extern	DatabaseObject	DBO;
//
// Local Constant Definitions:
//
#define PDAT_DATABASE	"databaseStructure.pdat"
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void defineDatabase ()
{

   int	i;			// loop counter
   char	filename[100];		// used to open pdat file
	    			   
   // Begin shared memory structure definition
	
   // Compute the database pdat structure file.
   strcpy(filename,getenv("HOME"));
   strcat(filename,PICTURE_DIR);
   strcat(filename,PDAT_DATABASE);
	
   cout      << " Defining the Database Structure pdat File: " << filename << endl;
	
   // Read the database structure into the rtm
   PfReadScript(filename);
	 
   // Create each variable in the database list
   for (i=0; i<NUM_REC; i++)
   {
      // Ignore blank lines (Spare Pids)
      if (strcmp(DBO.getpid(i),"") != 0)
      {
	 DBO.setiPicassoId(i, PfCreateVar(DBO.getpid(i), 
					  PfCRecord, "RECORD", TRUE, 
					  // &((DBO.pDB)[i]) ) );
					  DBO.getDBrecptr(i) ) );
      } // end if a blank line
   } // end for each point id
		    	   	           
   cout      << " End of Defining Shared Memory Database Structure" << endl;

}
