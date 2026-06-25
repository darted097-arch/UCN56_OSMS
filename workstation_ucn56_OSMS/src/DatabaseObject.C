//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// databaseObject.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This file implements generic Workstation Message object.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <fcntl.h>
#include <unistd.h>
#include <iostream.h>
#include <sys/errno.h>
#include <sys/stat.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"
#include "database.h"		// database structure
#include "DatabaseObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//#ifndef DEBUG
//#define DEBUG
//#endif
//
//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//
DatabaseObject::DatabaseObject(key_t key, int size, int mode) 
   : sharedMemoryObject(key, size, mode)
{
#ifdef DEBUG
   cout << " DatabaseObject constructor\n";
   cout << " Address of Database in shared memory " << hex << (void *)getAddr() << endl;
#endif
   pDB = (RECORD *)getAddr();
   setpDBrev(DeclarePrivateSpace(sizeof(int)));
   if( isCreatedByMe() )
   {
      readDatabase();
   }
   cout << "Database revision is: " << dec << getDBrev() << endl;
   flush(cout);
}

DatabaseObject::~DatabaseObject()
{
#ifdef DEBUG
   cout << " DatabaseObject destructor\n";
#endif
}

void DatabaseObject::setdisfmt(int ipn, int f)
{
   switch (f)
   {
      case 0:
	 strcpy(pDB[ipn].disfmt,"%6.0f");
	 break;
      case 1:
	 strcpy(pDB[ipn].disfmt,"%6.1f");
	 break;
      case 2:
	 strcpy(pDB[ipn].disfmt,"%6.2f");
	 break;
      case 3:
	 strcpy(pDB[ipn].disfmt,"%6.3f");
	 break;
      case 4:
	 strcpy(pDB[ipn].disfmt,"%6.4f");
	 break;
// Because of the implementation of the various pictures, Only a MAX of 
//8 characters can be used in the display string.  The exponant takes 4
//characters.  The sign takes 1 character.  That leaves only 3 characters 
//for 'n.n' where n is a decimal number.
      case 5:
	 strcpy(pDB[ipn].disfmt,"%8.1e");
	 break;
      default:
	 strcpy(pDB[ipn].disfmt,"?????");
	 break;
   }//endswitch
}

void DatabaseObject::readDatabase()
{
   int fdes;
   cout << "DatabaseObject:: Attempting to read database from file\n";

   if (-1 != (fdes = open(DB_FILENAME,O_RDONLY)))
   {
      int rstat;
      int tempDBrev;

      // if we cannot read database revision from file set database revision 
      //to DEFAULT_DBREV
      if ((-1 == (rstat = read(fdes,&tempDBrev,sizeof(int)))) || 
          (rstat < sizeof(int)))
      {
	 cout << "***** ERROR reading database rev number from file." << endl;
         setDBrev(DEFAULT_DBREV);
      }

      // if we read the database revision ok but didn't read the shared mem 
      //segment correctly, then set database revision to DEFAULT_DBREV;
      else if ((-1 == (rstat = read(fdes,getAddr(),getSize()))) || 
               (rstat < getSize()))
      {
	 cout << "***** ERROR reading database from file." << endl;
         setDBrev(DEFAULT_DBREV);
      }
      else
      {
	 cout << " Successfully read database from file = " << DB_FILENAME 
	      << ". Revision = " << dec << tempDBrev << endl;
         setDBrev(tempDBrev);
      }

      close(fdes);
   }
   else // error opening database file
   {
      if (ENOENT != errno)
      {
         cout << " Error opening database file = " << DB_FILENAME 
	      << ". errno = " << dec << errno << endl;
      }
      else
      {
         cout << " Database file = " << DB_FILENAME << " does not exist." 
              << endl;
      }
      setDBrev(DEFAULT_DBREV);
   }
   cout << " Done with attempting to read the database file. Database rev = " 
	<< dec << getDBrev() << endl;
}

void DatabaseObject::writeDatabase()
{
   int fdes;
   int tempDBrev = getDBrev();

   cout << "DatabaseObject: Deleting database file = " << DB_FILENAME << endl;
   if (-1 == unlink(DB_FILENAME))
   {
      if (ENOENT != errno)
      {
         cout << " Error deleting " << DB_FILENAME << ". errno = " << errno 
              << endl;
         cout << " Recommend manually deleting " << DB_FILENAME 
	      << " and restarting." << endl;
      }
      else
      {
         cout << " " << DB_FILENAME << " does not exist." << endl; 
      }
   }
   cout << " Writing: database to file = " << DB_FILENAME << " rev = " 
	<< dec << getDBrev() << endl;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   //if (-1 != (fdes = open(DB_FILENAME, O_CREAT | O_WRONLY)))
   if (-1 != (fdes = open(DB_FILENAME, O_CREAT | O_WRONLY, 666))) 	// for Linux (2012.07.05. CORENET )
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   {
      if (-1 != write(fdes,&tempDBrev,sizeof(tempDBrev)))
      {
         if (-1 != write(fdes,getAddr(),getSize()))
         {
            cout << " Done writing database file = " << DB_FILENAME 
                 << ". rev = " << dec << getDBrev() << endl;
            chmod(DB_FILENAME, DB_FILEKEYS);
         }
         else // error writing db to file
         {
            cout << "***** ERROR Writing database to database file." << endl;
         }
      }
      else // error writing db rev to file
      {
         cout << "***** ERROR Writing database revision to database file."
              << endl;
      }
      close(fdes);
   }
   else // open error
   {
      cout << "***** ERROR opening database file." << endl;
   }
}
