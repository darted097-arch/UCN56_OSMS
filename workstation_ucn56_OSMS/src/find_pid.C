//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// find_pid.C            
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  Given a ASCII text string, PID, will search the index in shared memory and 
//  return the array index for thg given PID. returns -1 if the PID is not found.
//  The process must be connected to shared memory.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <string.h>
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
// Local Constant Definitions: NONE
//
//
// Function Prototypes: NONE
//
int find_pid (char *p) {
   int x=0;
   char pid[12] = {0,};
   
   strcpy(pid, p);    // by Lee.M.W 
   for(x=0;x<8;x++)
   {
	  if (pid[x] == '\0') break;
	  if (pid[x]==' ')
	  {
		 pid[x]='\0';
		 cout << "***** WARNING: String not NULL terminated.   find_pid(" 
                      << pid << ") inserted NULL " 
                      << "character at position of pid[" << dec << x << "]" 
		      << endl;
	         break;
	  }
   } // endfor
   pid[x] = '\0'; // 8 character Point IDs max

   // There should be NO IPN 0 that came up from the host. Start with 1.
   x=1;
   while ((strcmp(pid, DBO.getpid(x)))  &&  x <= NUM_REC) { x++; }

   if (x<NUM_REC)
      return(x);
   else 
      return(-1);
}
