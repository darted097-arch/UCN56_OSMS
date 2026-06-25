//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// pager.C                
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This routine reads input from standard input and splits the input
//  into pages.  The pages are then printed individually to the remote
//  printer on the EOF station via the HP-UX remote print utilities.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
//
// User Defined Include Files:
//
// External Variables:
//
// Message queue
//
// Local Constant Definitions:
//
#define FILES_DEFAULT    10
#define LINES_DEFAULT    66
#define TIMEOUT_DEFAULT  60
#define INIT              0
#define PRINT             1
#define NOINP           -25
//
// Static Variables
//
// Function Prototypes:
//
void chg_file(int opt,int prevfile,int nextfile,FILE **fileptr);
//
// ***************************************************************************
//
void main(int argc, char *argv[])
{
   int MAXFILES = FILES_DEFAULT;
   int MAXLINES = LINES_DEFAULT;
   int MAXRETRY = TIMEOUT_DEFAULT;

   int numchar = 0;
   int state_cnt = 0;
   char c;
   ssize_t ret_status;
   int cstatus;
   int numline = 0;
   int numretry = 0;
   int nextfile = 0;
   int prevfile = 0;
   int curr_arg = 1;
   FILE *fileptr;

   while (curr_arg < argc)
   {
      if (strcmp("-h",argv[curr_arg]) == 0)
      {
         printf("Format is:\n");
         printf("   %s\n",argv[0]); 
         printf("     or any combination of:\n"); 
         printf("   %s -f files -l lines -t timeout\n",argv[0]); 
         printf("     to start execution or\n"); 
         printf("   %s -h\n",argv[0]);
         printf("     to print this help screen\n");
         printf("where:\n");
         printf("   files   = number of spool files to create\n");
         printf("             (defaults to %d files)\n",FILES_DEFAULT);
         printf("   lines   = number of lines per printed page\n");
         printf("             (defaults to %d lines)\n",LINES_DEFAULT);
         printf("   timeout = seconds to wait before printing\n");
         printf("             if page is incomplete\n");
         printf("             (defaults to %d seconds)\n",TIMEOUT_DEFAULT);
         return;
      }
      else if (strcmp("-f",argv[curr_arg]) == 0)
      {
         if ((curr_arg+1) < argc)
         {
            if ( sscanf(argv[curr_arg+1],"%d",&MAXFILES) == 0 )
            {
                printf("No value specified for -f option\n");
                return;
            }
            curr_arg += 2;
         }
         else
         {
            printf("No value specified for -f option\n");
            return;
         }
      }
      else if (strcmp("-l",argv[curr_arg]) == 0)
      {
         if ((curr_arg+1) < argc)
         {
            if ( sscanf(argv[curr_arg+1],"%d",&MAXLINES) == 0 )
            {
                printf("No value specified for -l option\n");
                return;
            }
            curr_arg += 2;
         }
         else
         {
            printf("No value specified for -l option\n");
            return;
         }
      }
      else if (strcmp("-t",argv[curr_arg]) == 0)
      {
         if ((curr_arg+1) < argc)
         {
            if ( sscanf(argv[curr_arg+1],"%d",&MAXRETRY) == 0 )
            {
                printf("No value specified for -t option\n");
                return;
            }
            curr_arg += 2;
         }
         else
         {
            printf("No value specified for -t option\n");
            return;
         }
      }
      else
      {
          printf("Invalid option specified\n");
          return;
      }
   }
      

// enable non-blocking I/O
   fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);

// open first spool file 
   chg_file(INIT,prevfile,nextfile,&fileptr);
   nextfile += 1;
   nextfile %= MAXFILES;

   while(1)
   {
//    get a character if available 
      ret_status = read(STDIN_FILENO,&c,1);
      cstatus = (int)ret_status;
      if (cstatus < 0) cstatus = NOINP;

//    char received and one page not received yet
//    so process char 
      if (cstatus != NOINP && numline < MAXLINES)
      {
         if (c == 13) // strip out carriage return
         {
         }
         else if (c == 10) // linefeed starts new line
         {
            fprintf(fileptr,"%c",c);
            numchar += 1;
            numline += 1;
         }
         else if (c == 12) // formfeed starts new page
         {
            numchar = 0;
            numline = 0;
            numretry = 0;
            chg_file(PRINT,prevfile,nextfile,&fileptr);
	    prevfile = nextfile;
            nextfile += 1;
            nextfile %= MAXFILES;
         }
         else //  print any other character to file
         {
            fprintf(fileptr,"%c",c);
            numchar += 1;
         }
      }

//    char received and one page has been received
//    so print page and then process char 
      else if (cstatus != NOINP && numline == MAXLINES)
      {
         numchar = 0;
         numline = 0;
         numretry = 0;
         chg_file(PRINT,prevfile,nextfile,&fileptr);
	 prevfile = nextfile;
         nextfile += 1;
         nextfile %= MAXFILES;

         if (c == 13) // strip out carriage return
         {
         }
         else if (c == 10) // linefeed starts new line
         {
            fprintf(fileptr,"%c",c);
            numchar += 1;
            numline += 1;
         }
         else if (c == 12) // formfeed starts new page
         {
         }
         else //  print any other character to file
         {
            fprintf(fileptr,"%c",c);
            numchar += 1;
         }
      }


//    at least one char has already been received but
//    nothing received this pass so retry next pass 
      else if (numchar > 0 && cstatus == NOINP &&
               numline < MAXLINES && numretry < MAXRETRY )
      {
	 sleep(1);
         numretry += 1;
      }


//    at least one char has already been received but
//    nothing received this pass and maximum retries
//    reached so print page 
      else if (numchar > 0 && cstatus == NOINP &&
               numline < MAXLINES && numretry == MAXRETRY )
      {
         numchar = 0;
         numline = 0;
         numretry = 0;
         chg_file(PRINT,prevfile,nextfile,&fileptr);
         prevfile = nextfile;
         nextfile += 1;
         nextfile %= MAXFILES;
      }

//    at least one char and no char was received this
//    pass but one page has been received already so
//    print the page 
      else if (numchar > 0 && cstatus == NOINP &&
               numline == MAXLINES )
      {
         numchar = 0;
         numline = 0;
         numretry = 0;
         chg_file(PRINT,prevfile,nextfile,&fileptr);
         prevfile = nextfile;
         nextfile += 1;
         nextfile %= MAXFILES;
      }

//    no chars have been received yet and none were
//    received this pass, so just wait for input 
      else if (numchar == 0 && cstatus == NOINP)
      {
         sleep(1);
      }

//    if you get here then something is wrong so print a message 
      else
      {
         printf("\ninvalid : nchar = %d, cstat = %d, nline = %d, nretry = %d",
                numchar,cstatus,numline,numretry);
         sleep(1);
      }

      fflush(stdout);
   }
}
