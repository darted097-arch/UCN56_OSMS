//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// createPicturePidLists.C
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module creates the Point Id shopping list for individual pictures.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>		// the ins and the outs
#include <fstream.h>		// for file I/O
#include <stdlib.h>		// for the system call
#include <string.h>		// for string operations
#include <unistd.h>		// for usleep
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system parameters
#include "interface.h"			// interface variables
#include "databaseParameters.h"
#include "DatabaseObject.h"
#include "PicPidListObject.h"	// Picture PID list object
//
// External Variables:
//
extern	DatabaseObject	DBO;		// Global database object
extern	INTERFACE	*Interface;	// Structure of all shm to GUI variables
//
// Local Constant Definitions:
//
#define	SORTED_SHOPPING_LIST	"ShoppingList"
#define LOG_FILENAME		"shoppingList.log"
#define FILE_LIST_NAME		"FileList"
#define MAXLENGTH		150
#define MAXFILENAMELEN		99
//
// static data
//
// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//
void PicPidListObject::createPicturePidLists()
{
   char command[100];
   char inputPath[100];
   char outputPath[100];
   char FLSTfilename[100];
   char inputLine[100];
#ifdef DEBUG
   struct mallinfo MemInfo;
#endif
   ifstream	FileList;

   if (TRUE != listIsBuilt)
   {
      cout << "\n\n***** Creating Picture Point ID Lists *****" << endl;
      // create the path to the directory where the Tdoc files are
      strcpy(inputPath, getenv("HOME"));
      strcat(inputPath, PICTURE_DIR);

      // create the path to the directory where the file list will be kept
      strcpy(outputPath, getenv("HOME"));
      strcat(outputPath, LIST_DIR);

      // create the full path of the file that will hold the file list
      strcpy(FLSTfilename, outputPath);
      strcat(FLSTfilename, FILE_LIST_NAME);
#ifdef DEBUG
      cout << "   Opening the file list\n";
#endif

      FileList.open(FLSTfilename);

#ifdef DEBUG
      // print out the current memory map before building the lists
      MemInfo = mallinfo();
      cout << " ***** Memory Information *****\n";
      cout << "  Total Space in Arena = " << dec << MemInfo.arena << endl
           << "  Number of ordinary blocks = " << MemInfo.ordblks << endl
           << "  Number of small blocks = " << MemInfo.smblks << endl
           << "  Space in holding block headers = " << MemInfo.hblkhd << endl
           << "  Number of holding blocks = " << MemInfo.hblks << endl
           << "  Space in sml blocks in use = " << MemInfo.usmblks << endl
           << "  Space in free small blocks = " << MemInfo.fsmblks << endl
           << "  Space in ord blocks in use = " << MemInfo.uordblks << endl
           << "  Space in free ordinary blcks = " << MemInfo.fordblks << endl
           << "  Penalty if keep option used = " << MemInfo.keepcost << endl;
      flush(cout);
#endif
      // For each line, i.e. filename, build a list of PIDs
      while (FileList.getline(inputLine, MAXFILENAMELEN))
      {
	 cout << "  Building PID list for " << inputLine << endl;
	 buildPicPidList(inputLine);

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
         usleep(50000);   
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      }
#ifdef DEBUG
      printPicList();
#endif
      listIsBuilt = TRUE;

#ifdef DEBUG
      // print out the current memory map before building the lists
      MemInfo = mallinfo();
      cout << " ***** Memory Information *****\n";
      cout << "  Total Space in Arena = " << dec << MemInfo.arena << endl
           << "  Number of ordinary blocks = " << MemInfo.ordblks << endl
           << "  Number of small blocks = " << MemInfo.smblks << endl
           << "  Space in holding block headers = " << MemInfo.hblkhd << endl
           << "  Number of holding blocks = " << MemInfo.hblks << endl
           << "  Space in sml blocks in use = " << MemInfo.usmblks << endl
           << "  Space in free small blocks = " << MemInfo.fsmblks << endl
           << "  Space in ord blocks in use = " << MemInfo.uordblks << endl
           << "  Space in free ordinary blcks = " << MemInfo.fordblks << endl
           << "  Penalty if keep option used = " << MemInfo.keepcost << endl;
      flush(cout);
#endif
   } // endif TRUE != listIsBuilt
}

void PicPidListObject::buildPicPidList(const char * picFile)
{
//
// Stack Variables:
//		
   int	PidCtr;      	   // keeps track of number of Pids
   int  numberOfAnalogs;
   int  numberOfDigitals;
   int	ipn;		   // pointer in shared memory
   
   ifstream SortedShoppingList;	// The sorted and unique list of point ids

   ofstream LogFile;		// logs all actions
	    		 
   char	command[500];		// command to gather raw point id list
   char	theInputLine[MAXLENGTH];	// used to read each line of input file
   char myPid[LENGTH_PID+sizeof(int)];	// parsed point id   
   char	SRTfilename[100];	// used to open Sorted Shopping List file
   char	LOGfilename[100];	// used to open LOG file
   char theInputPath[100];	// defines the input path
   char theOutputPath[100];	// defines the output path
   
   // >>>>>>>>>>>>>>>>>>>>>>>> Start <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // add the picture file to the list of pictures
   addPicToList(picFile);

   // Open all output files to be used; 

   // Create the path to the input directory 
   strcpy(theInputPath,getenv("HOME"));
   strcat(theInputPath,PICTURE_DIR);

   // Create the path to the output directory 
   strcpy(theOutputPath,getenv("HOME"));
   strcat(theOutputPath,LIST_DIR);

   // Open the shopping list log file
   strcpy(LOGfilename,theOutputPath);
   strcat(LOGfilename,picFile);
   strcat(LOGfilename,".");
   strcat(LOGfilename,LOG_FILENAME);
#ifdef DEBUG
   cout << "  Opening the Shopping List Log File " << LOGfilename << endl;
#endif
   LogFile.open(LOGfilename);
   
   // >>>>>>>>>>>>>>>>>>>>>>> Files open, Start <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   // Create the Picture Pid List File output path/filename
   strcpy(SRTfilename,theOutputPath);
   strcat(SRTfilename,picFile);
   strcat(SRTfilename,".");
   strcat(SRTfilename,SORTED_SHOPPING_LIST);
   

// >>>>>>>>>> THIS IS WHERE TO BUILD THE MEMORY RESIDENT LISTS <<<<<<<<<<
// >>>>>>>>>> Need a list of pictures, each of which has a list of PIDs <


      
   // Open the shopping list file which was generated from the raw 
   //shopping list.
#ifdef DEBUG
   cout << "  Opening the Sorted Shopping List File " << SRTfilename << endl;
#endif
   SortedShoppingList.open(SRTfilename);
   
   // initialize the number of Picasso points to update, the number 
   //of analog, and the number of digitals
   PidCtr = 0;
   numberOfAnalogs = 0;
   numberOfDigitals = 0;
  
   // Create the permanent GUI update array of UNIQUE pids
   //NOTE the SortedShoppingList had better not have any lines greater than
   //LENGTH_PID or the getline will overflow the myPid array.
   while (SortedShoppingList.getline(myPid,MAXLENGTH))
   {
      // The pid should have already been verified above;  Need the ipn now.
      // Note, you CANNOT do this in the previous while loop.  The raw 
      //list must eliminate all duplicates.  Otherwise, the 
      //permanent update array could contain many instances of the same pid.
      if ( (ipn = find_pid(myPid)) > 0)
      {
         PidCtr++;	// count the Pids for this picture
         // Put into update list and log action to log file
	 LogFile << "Inserting " << myPid << " into the memory resident array "
	         << "as IPN " << dec << ipn << " PicassoId "  
	         << DBO.getiPicassoId(ipn) << endl;

#ifdef DEBUG
	 cout << " Inserting " << myPid << " into the memory resident array "
	      << "as IPN " << dec << ipn << " PicassoId " 
	      << DBO.getiPicassoId(ipn) << endl;
#endif

	 // add the PID to the memory resident list
	 addPidToPicture(myPid, ipn, DBO.getiPicassoId(ipn));

	 if (DBO.isanalog(ipn)) numberOfAnalogs++; // count the analogs
	 else        	       numberOfDigitals++; // count the digitals

      } //endif Pid found in database
      else
      {
         // if point id is not found, then BIG TROUBLE; 
	 // output to error device, log file, and error file
	 cerr      << "FATAL ERROR: " << myPid << " Not Found in database\n";
	 LogFile   << "FATAL ERROR: " << myPid << " Not Found in database\n";

      }  
   } // end while loop

// end of Picture shopping list file
#ifdef DEBUG
   cout    << "***** Completed Processing the Picture Pid List for "
           << picFile << endl
           << " The total number of points to update is " << dec
      	   << PidCtr << endl
           << " Analog Points: " << dec << numberOfAnalogs
	   << ", " << "Digital Points: " << numberOfDigitals << endl << endl;
   LogFile << "***** Completed Processing the Picture Pid List for "
           << picFile << endl
           << " The total number of points to update is " << dec
      	   << PidCtr << endl
           << " Analog Points: " << dec << numberOfAnalogs
	   << ", " << "Digital Points: " << numberOfDigitals << endl << endl;
#endif
   
}// end createPicturePidLists()
