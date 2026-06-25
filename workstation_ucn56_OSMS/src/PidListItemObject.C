//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PidListItemObject.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module is the implementation of the PID List Item Object
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
#include "PidListItemObject.h"
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
PidListItemObject::PidListItemObject(const char * Pid, const int Ipn,
				     const int id)
{
#ifdef DEBUG
   cout << " PidListItemObject constructor\n";
#endif
   setPidName(Pid);
   setIpn(Ipn);
   setPicassoId(id);
   setNext(NULL);
}

PidListItemObject::~PidListItemObject()
{
#ifdef DEBUG
   cout << " PidListItemObject destructor\n";
#endif
}

