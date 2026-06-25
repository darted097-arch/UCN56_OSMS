#ifndef _INTERFACE_H
#define _INTERFACE_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// interface.h          
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// This is the structure for the Picasso cyclic status/control variables.
// Data in this structure is used exclusivly in the Picasso Interface task.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
// #include <someSystemInclude.h>
//
// User Defined Include Files:
//
//  messages.h includes all the pdat files that define the structures used in 
// interfaceStructure.pdat.
#include "messages.h"

// This pdat file contains the structure of the system attributes which are 
//passed from the Picasso Glue task to the GUI.  This data will be in a shared 
//memory segment.
#include "interfaceStructure.pdat"
//
// Global Variables
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
#endif

