//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// settime.C            
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This module sets the system time for the workstation.  The number
//  of seconds since January 1, 1970 at 00:00 UTC must be specified as
//  the second option on the command line when the task is started.
//  The task must be located in /usr/local/bin, owned by root and have
//  the set uid bit activated (chmod 4555).
//
//  Example: To set the time to January 2, 1970 00:00 UTC enter:
//  settime 86400
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
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
int main( int argc, char *argv[] )
{
   time_t epoch_secs = 0;

   if (argc != 2)
   {
      cout << "  Wrong number of arguments..." << endl;
   }
   else
   {
      epoch_secs = (time_t) atoi( argv[1] );
      if (epoch_secs < 0)
      {
          cout << "  Invalid argument entered" << endl;
      }
      else
      {
// modified to calibrate the time difference between CCUR and WS 010926
	  epoch_secs +=2 ;

          stime(&epoch_secs);
//        printf("  Received time of %d\n",(long)epoch_secs);
      }
   }
   return 1 ;
}
