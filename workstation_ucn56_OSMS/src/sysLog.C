//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sysLog.C             
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
//
// ***************************************************************************
//  This program writes all " cout " and errors to the circular file sysLog.log
// ***************************************************************************
//
// System Include Files:
//
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
//
// Local Constant Definitions: NONE
//
char file[40] = {'s','y','s','L','o','g','.','l','o','g'}; // by Lee.M.W
long int lineMax = 10000;	// max number of lines
char  ch = 0;
fstream fout;
//
// Function Prototypes:
//
//
// ***************************************************************************
//
int main(int argc, char *argv[])
{
   time_t       bintime;
   long int	lineCtr;
//
   lineCtr = 0;
   char timeString[12];
   char dateString[12];
   int curr_arg = 1;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> corenet
   char savebuff[1000];
   char outline[1000];
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // Set the CTRL-C interrupt handler
   signal(2, SIG_IGN);

   // check command line arguments
   while (curr_arg < argc)
   {
      // if help option display help
      if (strcmp("-h",argv[curr_arg]) == 0)
      {
         printf("Format is:\n");
         printf("   %s\n",argv[0]); 
         printf("     to write to sysLog.log or\n"); 
         printf("   -l logfile\n"); 
         printf("     to write to another log file or\n"); 
         printf("   -s size\n"); 
         printf("     to set max number of lines in the log file\n"); 
         printf("   %s -h\n",argv[0]);
         printf("     to print this help screen\n");
         return(0);
      }
      // if logfile option use specified name
      else if (strcmp("-l",argv[curr_arg]) == 0)
      {
         if ((curr_arg+1) < argc)
         {
            if ( sscanf(argv[curr_arg+1],"%s",file) == 0 )
            {
                printf("No value specified for -l option.  EXITING...\n");
                return(-1);
            }
            curr_arg += 2;
         }
         else
         {
            printf("No value specified for -l option.  EXITING...\n");
            return(-1);
         }
      }
      // if size option used, use size
      else if (strcmp("-s",argv[curr_arg]) == 0)
      {
         if ((curr_arg+1) < argc)
         {
	    cout << "sysLog: setting size of log file to " 
                 << atol(argv[curr_arg+1]) << endl;
            if ((lineMax = atol(argv[curr_arg+1]))  == 0)
            {
                cout << "Illegal value = " << argv[curr_arg+1] 
		     << " specified for -s option.  EXITING..." << endl;
                return(-1);
            }
            curr_arg += 2;
         }
         else
         {
            printf("No value specified for -s option.  EXITING...\n");
            return(-1);
         }
      }
      else
      {
          printf("Invalid option specified. type %s -h for help. EXITING...\n",
                 argv[0]);
          return(-1);
      }
   } // endwhile

   fout.open(file,ios::out | ios::ate);
   while (!cin.eof())
   {
//  start at top of the file
      fout.seekg(0,ios::beg);  // back to start of file

//  keep writing to the file while the file size is not exceeded     
      while (lineCtr < lineMax  &&  !cin.eof())
      {

//  init buffer to empty
         savebuff[0]='\0'; 

//  get the first character
	 ch = cin.get();

//  write characters of each line to buffer as received
         while (ch != 0xa  &&  !cin.eof())
	 {
	     if ((isascii(ch)  &&  
                 (isprint(ch)  ||  ch == '\t'  ||  ch == ' ')) )
	     {
	        sprintf(savebuff,"%s%c",savebuff,ch);
             }
             else
             {
                sprintf(savebuff,"%s[%x]",savebuff, ch);
             }
	     cin.get(ch);
	 } // endwhile not a carriage return and not end of file

         // count the number of lines
	 lineCtr++;

#ifdef DEBUG
         cout << "EndWhile (ch != 0xa) && !cin.eof(). ch = " << hex << (int)ch 
              << " cin.eof() = " << cin.eof() << " nlines = " << dec 
              << lineCtr << endl;
#endif
//  write date and time string before each line 
         time(&bintime);   
         strftime(timeString,12,"%X",localtime(&bintime));
	 // ujson 2016.11.15 
         //strftime(dateString,12,"%x",localtime(&bintime));
         strftime(dateString,12,"%m/%d/%Y",localtime(&bintime));
	 sprintf(outline,"%s\n",savebuff);
         cout << outline;
         sprintf(outline,"%s %s %s\n",dateString,timeString,savebuff);
         fout << outline;

//  flush output each pass
	 flush (cout);
	 flush (fout);
      } //end while within max bytes and not end of file
#ifdef DEBUG
      cout << "EndWhile (lineCtr <= lineMax) && !cin.eof(). lineCtr = " << dec 
	   << lineCtr << " lineMax = " << lineMax << " cin.eof() = " 
	   << cin.eof() << endl;
#endif
      lineCtr = 0;
   } // endwhile not end of file
#ifdef DEBUG
   cout << "EndWhile(!cin.eof()). ch = " << hex << (int)ch 
        << " cin.eof() = " << cin.eof() << endl;
#endif
   time(&bintime);   
   strftime(timeString,12,"%X",localtime(&bintime));
   // ujson 2016.11.15
   //strftime(dateString,12,"%x",localtime(&bintime));
   strftime(dateString,12,"%m/%d/%Y",localtime(&bintime));
   sprintf(savebuff,"%s\n", "EXITING sysLog...");
   sprintf(outline,"%s\n",savebuff);
   cout << outline;
   sprintf(outline,"%s %s %s\n",dateString,timeString,savebuff);
   fout << outline;
   return(0);
}//end sysLog()
