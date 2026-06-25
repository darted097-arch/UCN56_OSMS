//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sendPictureNameToInterfaceToGUI.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module sends a picture name from Picasso to InterfaceToGUI
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <api/api.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>

// External Variables:
extern	char	curPicture[];		// name of current picture

// Local Constant Definitions:
//
//
// Function Prototypes:
//
#include "prototypes.h"
//
//
// ***************************************************************************
//
int32 sendPictureNameToInterfaceToGUI(int32 numArgs, void *data)
{
   int32 theSize;			// the size of the input
   int32 theType;			// the input variable type
   void *theData;			// parsed version of the data   



// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Start of executable code.
   // Extract the data, type and size   
   theData = PfGetFuncArg(&data, &theType, &theSize);
   
#ifdef DEBUG
   cout << "  sendPictureNameToInterfaceToGUI(): theSize = " << dec << theSize 
	<< " theType = " << theType << " theData = " << (char *)theData
	<< endl;
#endif
   // Check the length of the picture name
   if(40 < strlen((char *)theData)) 
   {
      cout << "\n***** ERROR sendPictureNameToInterfaceToGUI: "
	   << endl << "Picture name length = " << dec << strlen((char*)theData)
	   << " is longer than the destination length of 40." << endl;
      flush(cout); //ensure all messages are written to console.     
      exit(-1);
   }

   if (theType != PfCUnsignedChar || numArgs != 1)
   {
      cerr << " Bad Input to sendPictureNameToInterfaceToGUI"
	   << "theType = " << theType << " theSize = " << theSize << endl;
      flush(cout); //ensure all messages are written to console.     
      exit(-1);
   }

   // copy the picture name to the Interface structure.
   strcpy(curPicture, (char *)theData);

   return (OK);
}//end sendPictureNameToInterfaceToGUI
