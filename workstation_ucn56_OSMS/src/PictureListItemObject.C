//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PictureListItemObject.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module is the implementation of the Picture List Item Object
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
//
// User Defined Include Files:
//
#include "PictureListItemObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//
// Function Prototypes:
//
//
// ***************************************************************************
//
PictureListItemObject::PictureListItemObject(const char * Pic)
{
#ifdef DEBUG
   cout << " PictureListItemObject constructor\n";
#endif
   setPicName(Pic);
   setpPidList(NULL);
   setNext(NULL);
   setnumberOfPids(0);
}

PictureListItemObject::~PictureListItemObject()
{
#ifdef DEBUG
   cout << " PictureListItemObject destructor\n";
#endif
}

