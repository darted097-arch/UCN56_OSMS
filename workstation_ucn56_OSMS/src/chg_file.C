//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// chg_file.C           
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This subroutine opens a spool file if INIT is passed as the
//  first argument, otherwise if PRINT is passed as the first
//  argument then it switches to the next spool file and submits
//  the previous spool file for printing.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <stdio.h>
#include <stdlib.h>
//
// User Defined Include Files:
//
// External Variables:
//
// Message queue
//
// Local Constant Definitions:
//
#define INIT              0
#define PRINT             1
//
// Static Variables
//
// Function Prototypes:
//
//
// ***************************************************************************
//
void chg_file(int opt,int prevfile,int nextfile,FILE **fileptr)
{
   char *basefile="/tmp/eofspool";
   char *printer="eof1";
   char filename[20];
   char cmdstring[40];
   int retcode;

   switch(opt)
   {
      case PRINT:
	 fclose(*fileptr);
	 sprintf(filename,"%s.%d",basefile,prevfile);
         sprintf(cmdstring,"lp -d %s -o nb %s",printer,filename);
//       printf("%s\n",cmdstring); 
         retcode = system(cmdstring); 
	 sprintf(filename,"%s.%d",basefile,nextfile);
	 *fileptr = fopen(filename,"w");
         break;
      case INIT:
      default:
	 sprintf(filename,"%s.%d",basefile,nextfile);
	 *fileptr = fopen(filename,"w");
   }
   return;
}
