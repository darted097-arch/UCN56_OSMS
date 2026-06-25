//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// disconnectFromGUI.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module is called whenever communication is lost with the rtm
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>		// Picasso api calls
#include <iostream.h>		// i/o stuff
//
// User Defined Include Files: NONE
//
//
// External Variables: NONE
//
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
void disconnectFromGUI (int status, const char *msg)
{
	cout << "Lost connection with Picasso3" << endl; 
	cout << "Status: " << status << " Reason: " << msg << endl;
}
