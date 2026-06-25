//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// initInterfaceToGUI.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module performs miscellaneous initializations.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <fstream.h>
#include <stdlib.h>
#include <signal.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
//
// External Variables:
//
//
// Local Constant Definitions:
//
#define	INTERFACE_ERROR_FILE	"InterfaceToGUI.err"
//
// Function Prototypes:
//
#include "prototypes.h"
void terminateInterfaceToGUI(int);
//
// ***************************************************************************
//
void initInterfaceToGUI()
{
   char	filename[100];
   
   // ********************************************************************************
   // Open all files
   // ********************************************************************************
   // >>> Universal Error File
   strcpy(filename,getenv("HOME"));
   strcat(filename,LIST_DIR);
   strcat(filename,INTERFACE_ERROR_FILE);
   cout << "Opening the Error File " << filename << endl;
   
      // ********************************************************************************
   // Initialize Signals
   // ********************************************************************************
   // Set up to terminate on interrupt
   if(signal(SIGINT,SIG_IGN) != SIG_IGN)  
      signal(SIGINT,terminateInterfaceToGUI); 
   
}

void terminateInterfaceToGUI(int signum)
{
   cout << "InterfaceToGUI terminated with signal = " << dec << signum << endl;
   cout << "InterfaceToGUI:  Normal termination" << endl;
   exit(0);
}

