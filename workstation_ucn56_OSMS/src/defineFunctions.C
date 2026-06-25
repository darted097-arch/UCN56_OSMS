//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// defineFunctions.C    
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  	This module performs the initialization for the functions used in
//	the Interface to GUI task
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
//
// User Defined Include Files: NONE
//
//
// External Variables:
//
//
// Local Constant Definitions: NONE
//
//
// Function Prototypes:
//
int32 sendMessageToHost(int32 argc, void* argv);	// message routine
int32 sendPictureNameToInterfaceToGUI(int32 argc, void* argv);
int32 findPidForGUI(int32 argc, void* argv);		// find point id function
int32 manageTempShopList(int32 argc, void* argv);	// manage temp shopping list
//
void defineFunctions()
{

   static PfTArg sendMessageToHost_args[] 	= { {PfCInt,1} };
   static PfTArg sendPictureName_args[] = { {PfCUnsignedChar,0} };
   static PfTArg findPidForGUI_args[]
              = { {PfCUnsignedChar,0},{PfCUnsignedChar,0},
                  {PfCUnsignedChar,0},{PfCUnsignedChar,0}};
   static PfTArg manageTempShopList_args[] 	= { {PfCInt,1},{PfCUnsignedChar,0}};
   
   
   PfRegisterFunction ("sendMessageToHost",(PfTFunction)sendMessageToHost,1,sendMessageToHost_args);
   if (apiError != OK)
   {
      cerr      << "[sendMessageToHost]: PfRegisterFunction failed (" 
	        << PfGetSystemError(apiError) << ")" << endl;
   }
   PfRegisterFunction ("findPidForGUI",(PfTFunction)findPidForGUI,4,findPidForGUI_args);
   if (apiError != OK)
   {
      cerr      << "[findPidForGUI]: PfRegisterFunction failed (" 
	        << PfGetSystemError(apiError) << ")" << endl;
   }
   PfRegisterFunction ("manageTempShopList",(PfTFunction)manageTempShopList,2,manageTempShopList_args);
   if (apiError != OK)
   {
      cerr      << "[manageTempShopList]: PfRegisterFunction failed (" 
	        << PfGetSystemError(apiError) << ")" << endl;

   }
   
   PfRegisterFunction ("sendPictureNameToInterfaceToGUI",(PfTFunction)sendPictureNameToInterfaceToGUI,1,sendPictureName_args);
   if (apiError != OK)
   {
      cerr      << "[sendPictureNameToInterfaceToGUI]: PfRegisterFunction failed (" 
	        << PfGetSystemError(apiError) << ")" << endl;
   }

}
