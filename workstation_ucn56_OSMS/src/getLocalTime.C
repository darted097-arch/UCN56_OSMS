//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// getLocalTime.C       
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  Routine to convert time to the local time in seconds since the EPOCH
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <time.h>
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
time_t getLocalTime()
{
   
   struct tm   *timeStructure;		// to read local time into
   struct tm	timeFields;		// to convert to seconds
   time_t	myTime;			// seconds since EPOCH
   
   // Get the seconds since the EPOCH
   time (&myTime);
   
   // Convert this to the local time
   timeStructure = localtime(&myTime);
   
   // Now convert it back to seconds since the EPOCH
   timeFields.tm_mday = timeStructure->tm_mday;
   timeFields.tm_mon  = timeStructure->tm_mon;
   timeFields.tm_year = timeStructure->tm_year;
   timeFields.tm_hour = timeStructure->tm_hour;
   timeFields.tm_min  = timeStructure->tm_min;
   timeFields.tm_sec  = timeStructure->tm_sec;
   
   timeFields.tm_isdst = timeStructure->tm_isdst;
   
   return (mktime(&timeFields) );
}
