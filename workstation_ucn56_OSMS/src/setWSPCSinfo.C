//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// setWSPCSinfo.C       
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module gets the WSPCS parameters from the WSPCS configuration
//  file.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
//
// User Defined Include Files: NONE
//
#include "WSPCSObject.h"
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
const int MAX_LINE_LEN = 180;
const int MAX_WORD_LEN = 120;
const int MAX_STRINGS = 110;
const int MAX_ARGS = MAX_STRINGS + 1;

static void exitMsg(char * args[], char strings[MAX_STRINGS][MAX_WORD_LEN+1], 
		    int strCtr);

void WSPCSObject::setWSPCSinfo(char * fn)
{
   ifstream	cfgFile;
   char		lineBuf[MAX_LINE_LEN+1];	// add 1 for the NULL char
   char		wordBuf[MAX_WORD_LEN+1];
   char         strings[MAX_STRINGS][MAX_WORD_LEN+1];// string entries
   char	*	args[MAX_ARGS];			// pointers to string entries
   int		i, argCtr, strCtr;

   argCtr = 0;
   strCtr = 0;

   // arg[0] should be the name of the program, or in this case method
   args[argCtr++] = "WSPCSObject::setWSPCSinfo";

   cout << "\nsetWSPCSinfo(" << fn << ")" << endl;
   cfgFile.open(fn);
   if(!(cfgFile.good()))
   {
      cout << "***** Could not open configuration file = " << fn << endl;
      return;
   }
   cout << "***** WSPCS Configuration file = " << fn 
        << " opened successfully" << endl;
   for(i = 0; i < MAX_LINE_LEN; i++)
   {
      lineBuf[i] = '\0';
   } 
   while(cfgFile.getline(lineBuf,MAX_LINE_LEN,'\n'))
   {
#ifdef DEBUG
      cout << "\nAfter cfgFile.getline() len = " << dec << strlen(lineBuf) 
           << endl << "(" << lineBuf << ")" << endl;
#endif
      if(cfgFile.good()  &&  '!' != lineBuf[0])
      {
         istrstream	lineStr(lineBuf, strlen(lineBuf));
	 while(!((lineStr >> wordBuf).fail())) // something left in the line
	 {
            if (MAX_WORD_LEN < strlen(wordBuf))
            {
               cout << "  The word " << wordBuf << " is greater than "
                    << dec << MAX_WORD_LEN << " characters. Exiting..." 
                    << endl;
               exitMsg(args, strings, strCtr);
            }

            if('-' == wordBuf[0])
            {
               cout << " Found the KEY WORD " << wordBuf << endl;
#ifdef DEBUG
               cout << "     MAX_STRINGS = " << MAX_STRINGS 
                    << " strCtr = " << strCtr << endl;
               cout << "     MAX_ARGS = " << MAX_ARGS 
                    << " argCtr = " << argCtr << endl;
               cout << " length of word = " << strlen(wordBuf) << endl;
#endif
	       // add the keyword to the string array
	       if(MAX_STRINGS > strCtr) strcpy(strings[strCtr],wordBuf);
	       else
	       {
#ifdef DEBUG
	          cout << "   About to exit after processing " << wordBuf 
	               << endl;
                  cout << "     MAX_STRINGS = " << MAX_STRINGS 
                       << " strCtr = " << strCtr << endl;
#endif
                  // exit!!!
	          exitMsg(args, strings, strCtr);
	       }

	       // add the keyword to the argument pointer array
	       if(MAX_ARGS > argCtr) args[argCtr++] = strings[strCtr++];
	       else
	       {
                  // exit!!!
	          exitMsg(args, strings, strCtr);
	       }

	       if((lineStr >> wordBuf).fail())
               {
                  // if no parameters for the keyword
                  if(MAX_STRINGS > strCtr) strcpy(strings[strCtr],"");
	          else
	          {
                     // exit!!!
	             exitMsg(args, strings, strCtr);
	          }
               }
               else
               {
                  // for now, only one parameter per keyword
                  if(MAX_STRINGS > strCtr) strcpy(strings[strCtr], wordBuf);
	          else
	          {
                     // exit!!!
	             exitMsg(args, strings, strCtr);
	          }
               }

               if(MAX_ARGS > argCtr) args[argCtr++] = strings[strCtr++];
	       else
	       {
                  // exit!!!
		  exitMsg(args, strings, strCtr);
	       }

	       cout << "  " << strings[strCtr-2] << " " << strings[strCtr-1] 
		    << " is being added to the option list." << endl;
            } // endif found an option
#ifdef DEBUG
            else
            {
               cout << wordBuf << " is not a valid option" << endl;
            }
#endif
         } // endwhile get a word from the line
#ifdef DEBUG
         cout << "endwhile: !(lineStr >> wordBuf.fail()) returned " 
              << lineStr.fail() << " wordBuf = " << wordBuf << endl;
#endif
      } // endif got a line from the file
#ifdef DEBUG
      cout << "endif cfgFile.good() returned " << cfgFile.good() << endl;
      cout << "lineBuf = " << lineBuf << endl;
#endif
   } // endwhile getline
   if(cfgFile.eof())
   {
#ifdef DEBUG
      cout << "End of File: cfgFile.eof() returned " << dec 
           << cfgFile.eof() << endl;
#endif
   }
   else
   {
      cout << "***** ERROR End of File not reached, but can not get "
           << "any more lines from" << fn << endl;
   }
   cfgFile.close();
#ifdef DEBUG
   cout << "  arguments found in " << fn << " are:" << endl;
   for(i = 0; i < argCtr; i++)
   {
      cout << "  arg[" << dec << i << "] is " << args[i] << endl;
   }
#endif
   parseOptions(argCtr, args);
}



static void exitMsg(char * args[], char strings[MAX_STRINGS][MAX_WORD_LEN+1], 
		    int strCtr)
{
   if(0 < strCtr)
   {
      if(MAX_STRINGS <= strCtr)
      {
         cout << "  " << args[0] << " FATAL ERROR: "
              << "Maximum number of strings reached! Exiting..."
              << endl;
      }
   }
   else
   {
      strCtr = 1;
   }

   cout << "    Last string processed = " 
        << strings[strCtr-1] << endl;
   exit(-1);
}
