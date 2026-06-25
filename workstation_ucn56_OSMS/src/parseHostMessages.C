//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseHostMessages.C  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This routine processes the special messages (set time, database revision
//  messages, etc. received from the Host.  It passes off the other message 
//  to the appropriate parse routines.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <fcntl.h>
#include <fstream.h>
#include <iostream.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/errno.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
//
// User Defined Include Files:
//
#include "systemParameters.h"		// generic system variable definitions
#include "databaseParameters.h"		// database parameters
#include "database.h"			// database structure
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message structures
#include "interface.h"			// Picasso to shm structure
#include "messageQueueObject.h"
#include "CommLinkObject.h"
#include "messageObject.h"
#include "InterProcSMObject.h"
#include "DatabaseObject.h"
//
// External Variables:
//
extern	INTERFACE       *Interface;	// shared memory to Picasso variables
extern	InterProcSMObject       IPO;	// Inter Process Shared Memory
extern	DatabaseObject		DBO;	// Database object shared memory object
extern  CommLinkObject          CommLink;	// Commlink object

// Message queue
extern	messageQueueObject	WS_MsgQue;	// GUI to CommLink message queue
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue

extern  CommLinkObject          CommLink;	// Commlink object
//
// Local Constant Definitions:
//
//#ifndef DEBUG
//   #define DEBUG
//#endif
//
// Static Variables
//
// Create a variable of WSmessageObject type.
static messageObject PHmsg(FROM_HOST_MSG_ID,"PHmsg");	// Create message object

static int needToSignalDatabaseLoaded = TRUE;

#ifdef DEBUG
   static unsigned long DBupdateCounter = 1;
   static unsigned long heartbeatCounter = 1;
   const int DEBUG_DISPLAY_INTERVAL = 30;
#endif

static int 	newDBupdatesStarted = FALSE;
static int 	db_buffer = 0;			// database download counter
static short	last_pb_status = UNKNOWN_COMMLINK;
static short	pb_status = UNKNOWN_COMMLINK;
static struct timeval	timeout;	// for select "sleep" function
static int	needToWriteDB = FALSE;

// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//
void CommLinkObject::parseHostMessages()
{   
   // number of ticks since WSPCS start for this message
   int		   numTicksThisMsg;	

   int		   status;		// returned status
   time_t	   thisMessageTime;	// time of the current message

   time_t          wsLocalTime;
   time_t	   hostTime;		// time from the host
   
   int		   j;			// used for database download message  
   int             wait_cnt = 0;            // read wait loop count
   
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   int		   * db_revision_ptr;	// CORENET 2012.07.10
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // initialize variables
   total_bytes_read = 0;
   bytes_read = 0;
   bytesToRead = sizeof(Local.header);

   msgSize = sizeof(Local.header);
   gotMsgSize = FALSE;

   char cmdstring[30];

   // read from the Concurrent
   while (total_bytes_read < msgSize) // loop until buffer full
   {
      // read host message into local buffer
      if(( bytes_read = u_read(ReadSocket, 
			       (char *)&(Local.char_buf[total_bytes_read]),
                               bytesToRead)) > 0 ) 
      {
#ifdef DEBUG
         cout << " bytes_read = " << dec << bytes_read << endl;
         flush(cout);
#endif
         total_bytes_read += bytes_read;
	 if ((total_bytes_read >= sizeof(Local.header)) &&
	     (FALSE == gotMsgSize) )
	 { // if we've read at least a header number of bytes
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
	    Local.header.opcode = ntohs(Local.header.opcode); // 
	    Local.header.numberOfSomething = ntohs(Local.header.numberOfSomething); // 
            Local.header.mspbStatus = ntohs(Local.header.mspbStatus); // 
	    Local.header.disabilityIndex = ntohl(Local.header.disabilityIndex); // 
	    Local.header.messageSize = ntohs(Local.header.messageSize); // 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//cout << "mspbStatus : [0x" <<  hex << Local.header.mspbStatus << "]" 
//	<< "disabilityIndex : [0x" <<  hex << Local.header.disabilityIndex << "]" <<  endl; // 


#ifdef DEBUG
	    switch(Local.header.opcode)
	    {
               case DEFINE_DIGITAL:
               case DEFINE_ANALOG:
               case UPDATE_DATABASE:
               case HEARTBEAT:
                  break;
               default:
                  cout << getname() << "::ParseHostMessages: receiving " << dec
                       << Local.header.messageSize << " byte message for "
                       << "opcode " << hex << Local.header.opcode << endl;
                  break;
            }
#endif
	    if (0 == Local.header.messageSize)
	    {
	       // no size in header, use default
	       msgSize = sizeof(Local.char_buf);
	    }
	    else
	    {
	       msgSize = Local.header.messageSize;
	    } // endelse
	    gotMsgSize = TRUE;
	    flush(cout);
	 } // endif
	 if (total_bytes_read > msgSize)
	 {
	    cout << "   ***** ERROR: parseHostMessages(): read more than one "
                 << "message. Header of next message is" << endl << hex;
	    cout << Local.totalMessage[msgSize];
	    cout << Local.totalMessage[msgSize+1];
	    cout << Local.totalMessage[msgSize+2];
	    cout << Local.totalMessage[msgSize+3];
	    cout << Local.totalMessage[msgSize+4];
	    cout << Local.totalMessage[msgSize+5];
	    cout << endl;
	    flush(cout);
	    // try to realign the the messages, but this may not be
	    //possible. This error may cause problems that can not
	    //be repaired.
	    bytesToRead = (total_bytes_read - msgSize) + 
			  ((Local.totalMessage[msgSize+5] & 0xff00) >> 16);
	 }
	 else
	 {
            bytesToRead = msgSize - total_bytes_read;
         }
	 wait_cnt = 0;
      }
      else   
      { 
	 // set up the data structure for the microsecond sleep time
	 timeout.tv_sec  = 0;
	 timeout.tv_usec = SLEEPTIME;
	 // use a null select for a more precise sleep time
	 if (select(0, 0, 0, 0, &timeout) < 0)
	    cout << getname() << "CommLinkObject::parseHostMessages() "
		 << "Error from select() call" << endl;

         //  when bytes_read is zero, ws2ccurlink checks if a message is on the
         //  message queue, and if present, sends it to the host.      
         wait_cnt++;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
         //if(wait_cnt > 4)
         //   ws2ccurlink();
         if(wait_cnt > 4) {
            ws2ccurlink();
	    wait_cnt = 0;
	 }
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//#ifdef DEBUG
//       if(0 == (wait_cnt % 20))
//       {
//          cout << "  wait_cnt = " << dec << wait_cnt << endl;
//          flush(cout);
//       }
//#endif        
      }
      
      // Check for timeout from the host
      numTicksThisMsg = IPO.getNumTicks();
      thisMessageTime = getLocalTime();
      lastMessageTime = IPO.gettimeOfLastRead(getnum());
      if ( (numTicksThisMsg - IPO.getNumTicksOfLastRead(getnum())) > PACKET_TIMEOUT)	
      {

         cout << getname() << " CommLinkObject::parseHostMessages() "
              << "Timeout reading from host at: " 
	      << ctime(&thisMessageTime) << endl;
         cout << " Last Message Received at: " << ctime(&lastMessageTime) 
	      << endl;
         cout << "  Number of ticks since last read = " << dec
              << numTicksThisMsg - IPO.getNumTicksOfLastRead(getnum())
              << endl;
         closeAndReconnect();
      } // endif packet timeout
   }	// end while loop

   // Set the last message time in ticks for this message
   IPO.settimeOfLastRead(getnum(), thisMessageTime);
   IPO.setNumTicksOfLastRead(getnum(), numTicksThisMsg);
   
#ifdef DEBUG
   cout << "   ParseHostMessages: read message of size = " << dec 
	<< Local.header.messageSize << " " << hex << Local.header.messageSize 
        << endl;
   cout << "   ParseHostMessages: Putting Host message of size = " << dec 
	<< Local.header.messageSize << " " << " into a WSmessage of size " 
        << dec << msgSize << endl;
#endif
   // get the message into the message object.
   PHmsg.setMessage(&Local.char_buf, msgSize);
   // And process the message based upon the opcode...

   // set the master/slave and failover statuses in shared memory 
   //from the message
   IPO.setmasterStatus(getnum(), (int)PHmsg.getmspbStatus());
   IPO.setfailoverState(getnum(), PHmsg.getfailoverState());
   IPO.setcpuNum(getnum(), PHmsg.getcpuNumber());

#ifdef DEBUG
   cout << "***** " << getname() << "::parseHostMessages() wsState = " 
        << hex << Interface->System.wsState << endl;
   cout << "***** masterStatus, failoverState, cpuNum = " << dec 
	<< IPO.getmasterStatus(getnum()) << " " 
	<< IPO.getfailoverState(getnum()) << " " << IPO.getcpuNum(getnum()) 
	<< endl;
#endif
      
   // save the last primary/backup status and get the new one
   last_pb_status = pb_status;
   pb_status = IPO.getprimaryStatus(getnum());
   
   if (PRIMARY_COMMLINK == pb_status)
   {
      Interface->System.cpuNum = PHmsg.getcpuNumber(); 
#ifdef DEBUG
      cout << "***** " << getname() 
	   << "::parseHostMessages() setting CPU number to " 
	   << PHmsg.getcpuNumber() << endl;
      cout << "***** " << getname() 
	   << "::parseHostMessages() about to look at pbstatus from " 
	   << "message, SLAVE_CPU = " << hex << (int)PHmsg.getmspbStatus() 
	   << " " << SLAVE_CPU << endl;
#endif
      if (SLAVE_CPU == (int)PHmsg.getmspbStatus())
      {
	 Interface->System.slaveUpdating = TRUE;
      }
      else
      {
	 Interface->System.slaveUpdating = FALSE;
      }
   }

   // corenet ujson 2017.0104
   if (PHmsg.getOpcode() != 5) {
   cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RCV DATA(from CCUR).... Opcode [" << hex <<  PHmsg.getOpcode() << "]" << endl;
   }
   flush(cout);

   switch (PHmsg.getOpcode()) // process based upon opcode
   {

     // >>>>>>>>>>>>>>>>>>>> Database Revision Request <<<<<<<<<<<<<<<<<<<<<<
     
      case REVISION_REQUEST:
	 {
	    cout << getname() << "::parseHostMessages: "
		 << "Received revision request message. " << endl;
	    cout << "	 Database revision " << "on the server is " << dec 
#ifdef LINUX_APP
		 << ntohl(Local.response.data.returnStatus) << endl;
#else
     << Local.response.data.returnStatus << endl;
#endif
	    cout << "    wsState is " << hex << Interface->System.wsState 
                 << endl; 

            // if we are change from Backup to Primary CommLink,
            //then if the data base rev of the host does not match the 
            //database rev of the client, restart PCS processes.
            if (Interface->System.wsState & PB_CHANGE_WS_STATE)
	    {
               cout << "***** " << getname() 
                    << "::parseHostMessages() In pb status change: wsState = " 
                    << hex << Interface->System.wsState << endl;
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef LINUX_APP
               if (Local.response.data.returnStatus != 
#else
               if (ntohl(Local.response.data.returnStatus) != 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                   Interface->System.databaseRevision)
               {
                  cout << endl << endl << endl;
                  cout << "***** " << getname() 
                       << "parseHostMessages() DB rev on "
                       << "server = " << dec 
#ifndef LINUX_APP
                       << Local.response.data.returnStatus                     
#else
                       << ntohl(Local.response.data.returnStatus) 
#endif
                       << " does not match rev on client = "
                       << Interface->System.databaseRevision << endl;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef LINUX_APP
                  if (Local.response.data.returnStatus > 
#else
                  if (ntohl(Local.response.data.returnStatus) > 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                      Interface->System.databaseRevision)
                  {
                     cout << "   DB rev on server is newer than DB rev on "
                          << "client." << endl;
                     cout << "   Attempting to reload the database."
                          << endl;
                     // delete the database image file
                     if (-1 == unlink(DB_FILENAME))
                     {
                        if (ENOENT != errno)
                        {
                           cout << " Error deleting " << DB_FILENAME 
                                << ". errno = " << errno
                                << endl;
                           cout << " Recommend manually deleting " 
                                << DB_FILENAME << " and restarting." << endl;
                        }
                        else
                        {
                           cout << " " << DB_FILENAME << " does not exist." 
                                << endl;
                        }
                     }
                     // kill CommLink, so that WSPCS will restart processes. 
		     Interface->System.databaseRevision = NOT_A_DBREV;
                  } //endif host DB rev greater than WS DB rev
                  else // host DB rev less than WS db rev
                  {
                     cout << "   DATABASE ON SERVER OLDER THAN DATABASE ON "
                          << "CLIENT." << endl;
                     cout << "   LOAD NEWER DATABASE ON THE HOST AND "
		          << "RESTART THE WORKSTATION." << endl;
                     cout << "   KILLING ALL PCS PROCESSES ON CLIENT!!!" 
                          << endl;

                     // delete the database image file
                     if (-1 == unlink(DB_FILENAME))
                     {
                        if (ENOENT != errno)
                        {
                           cout << " Error deleting " << DB_FILENAME 
                                << ". errno = " << errno
                                << endl;
                           cout << " Recommend manually deleting " 
                                << DB_FILENAME
                                << endl;
                        }
                        else
                        {
                           cout << " " << DB_FILENAME << " does not exist." 
                                << endl;
                        }
                     }
                     // kill all the PCS processes on the workstation
                     kill(0, SIGINT);
                  } // endesle host DB rev less than WS DB rev
               } // endif host DB rev does not equal WS DB rev
               else
               {
                  cout << "   DB rev on server matches DB rev on client." 
                       << endl;
               }
	    } // endif we are in the middle of a pb status change to primary

	    cout << getname() << "CommLinkObject::parseHostMessages: "
		 << "Received revision request message, returning " << dec
	         <<  Interface->System.databaseRevision << endl;
	    
	    // create the message object
	    messageObject DB_revData(TO_HOST_MSG_ID, "DB_revData");
	    
	    // Set the opcode in the header
	    DB_revData.setHeader(REVISION_DATA, 1);
	    
	    // Set the data portion of the message
	    DB_revData.setData(&(Interface->System.databaseRevision),
			       sizeof(Interface->System.databaseRevision));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
		db_revision_ptr = (int *)DB_revData.getDataPtr();	
		*db_revision_ptr = htonl(*db_revision_ptr);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            // flush the message queue of any messages that are on it
	    //We do this because after a the host sends a REVISION_REQUEST,
	    //it expects a revision data on its next read from the WS.
	    WS_MsgQue.flush();

  	    // send the message directly to the host
            if (writeMessageToHost(DB_revData) >= 0)
	       cout << getname() << " CommLinkObject::parseHostMessages: "
		    << "Sent DB revision " << dec
		    << Interface->System.databaseRevision << " back to host\n";
	    else
	       cout << getname() << " CommLinkObject::parseHostMessages: "
		    << "Error sending DB revision back "
		    << "to host" << endl;

	    db_buffer=0;
            break;
	 }

     // >>>>>>>>>>>>>>>>>>>>>>>> Master Slave Status <<<<<<<<<<<<<<<<<<<<<<<<<<
      // Initialization after connection phase.
      // message from Host to workstation 
      case SEND_MASTER_SLAVE_STATUS:
      {	    
        // flush the message queue of any messages that are on it
        //We do this because after a the host sends a SEND_MASTER_SLAVE_STATUS,
        //it expects primary/backup data on its next read from the WS.
        WS_MsgQue.flush();

//#ifdef DEBUG	       
         cout << getname() << "parseHostMessages: CommLink Number = " 
	      << dec << getnum() << " Received SEND_MASTER_SLAVE_STATUS "
	      << "message from CPU: " << dec << PHmsg.getcpuNumber() << endl;
	 cout << "     Received Master Slave Status: " 
              << dec << PHmsg.getmspbStatus() << endl;
	 cout << "     Received Cpu State: " <<  PHmsg.getfailoverState() 
	      << endl;
//#endif
	 // Calculate and set the Primary and Backup statuses of the 
	 //two CommLinks in shared memroy based on the data from the host.
	 setPBstatuses();

	 // get the newly calculated primary/backup status for this CommLink
         pb_status = IPO.getprimaryStatus(getnum());

//#ifdef DEBUG	    
 	 cout << getname() << "CommLinkObject::parseHostMessages() "
	      << "Returning Primary Backup status: " 
	      <<  pb_status << endl;
         cout << "         Primary/Backup status for " << getname() 
	      << " is " << pb_status << endl;
         cout << "         Host1 CPU M/S, CT, DI, P/B = " 
	      << IPO.getcpuNum(1) << " " << IPO.getmasterStatus(1) 
              << " " << IPO.getconnected(1) << " " 
	      << IPO.getfailoverState(1) << " " << IPO.getprimaryStatus(1) 
	      << endl;
         cout << "         Host2 CPU M/S, CT, DI, P/B = " 
	      << IPO.getcpuNum(2) << " " << IPO.getmasterStatus(2) 
              << " " << IPO.getconnected(2) << " " 
	      << IPO.getfailoverState(2) << " " << IPO.getprimaryStatus(2) 
	      << endl;
//#endif
         // create the message object
         messageObject PB_Data(TO_HOST_MSG_ID, "PB_Data");

         // Set the opcode in the header
         //to send Primary/Backup status to host
         PB_Data.setHeader(SEND_PRIMARY_BACKUP_STATUS, 1);

         // Set the master/slave primary/backup status of the
         //workstation in the message.
         PB_Data.setmspbStatus(pb_status);

         if (writeMessageToHost(PB_Data) < 0)
         {
            cout << getname()
                 << "parseHostMessages: On Host request: Error sending "
                 << "Primary/Backup status back to host\n";
         }
         else
         {
            cout << getname() << " CommLinkObject::parseHostMessages: "
                 << " As requested from host: Sent Primary/Backup "
                 << "status = " << dec << pb_status << " back to host\n";
         }
         break;
      } // endcase

      // >>>>>>>>>>>>>>>>>>>>>>>>> Database Revision Define Request <<<<<<<<<<<<<<<<<<
     
      case REVISION_DEFINED:
          // Set Workstation to the Database revision Number received from Host
          PHmsg.getData(&(Interface->System.databaseRevision),
                   sizeof(Interface->System.databaseRevision));
//////////////////////////////////////////////////////////////
// CORENET 2012.07.10
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
          Interface->System.databaseRevision = ntohl(Interface->System.databaseRevision);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

          DBO.setDBrev(Interface->System.databaseRevision);
          DBO.writeDatabase();

	  cout << endl << getname() << " CommLinkObject::parseHostMessages: "
	       << "REVISE: Received database revision define message, received ";
	  cout << dec <<  Interface->System.databaseRevision << endl;
	  
          break;
      
     // >>>>>>>>>>>>>>> Update a database definition <<<<<<<<<<<<<<<<<<<<
     case UPDATE_DB_DIGI:
     case UPDATE_DB_ANAL:
	 parseDBLoadMessages(PHmsg);
	 needToWriteDB = TRUE;
	 break;

     // >>>>>>>>>>>>>>>>>>>>>>>>>>>> LOAD DATABASE <<<<<<<<<<<<<<<<<<<<<<<<<<<< 
     case DEFINE_DIGITAL:
     case DEFINE_ANALOG:
     {
	 cout << hex << PHmsg.getOpcode() << " Message follows:" << endl;
         cout << "[00]:" << hex << Local.totalMessage[0] << " "
              << "[04]:" << Local.totalMessage[1] << " "
              << "[08]:" << Local.totalMessage[2] << " "
              << "[12]:" << Local.totalMessage[3] << " "
              << "[16]:" << Local.totalMessage[4] << " "
              << "[20]:" << Local.totalMessage[5] << " " << endl;
         cout << "[24]:" << Local.totalMessage[6] << " "
              << "[28]:" << Local.totalMessage[7] << " "
              << "[32]:" << Local.totalMessage[8] << " "
              << "[36]:" << Local.totalMessage[9] << " " << endl;

         cout << getname() <<  ">>>>>>>>>>>>>>>>>>>>: Receiving database from host... " << dec << db_buffer << endl;
         flush(cout);

	 j = db_buffer % 10;
	 if(j == 0)
         {
	    //cout << getname() <<  ": Receiving database from host...................." << endl;
	    //flush(cout);
	 }  

	 db_buffer++;

	 needToSignalDatabaseLoaded = TRUE;
	 IPO.setReceivedDBdownload(FALSE);
	 Interface->System.wsState &= ~(DB_DEF_DONE_WS_STATE);
	 
	 // process the message based upon the opcode
	 parseDBLoadMessages(PHmsg);

         cout << getname() <<  "<<<<<<<<<<<<<<<<<<<<: Receiving database from host... " << dec << db_buffer << endl;
         //flush(cout);

         break;
      }

     // >>>>>>>>>>>>>>>>>>>>>>>>>>> UPDATE DATABASE <<<<<<<<<<<<<<<<<<<<<<<<<<<< 
     case UPDATE_DATABASE:
     {
        // Only the Primary CommLink should receive database updates.
        if (PRIMARY_COMMLINK == IPO.getprimaryStatus(getnum()))
        {
	   // Don't process database update messages on the primary
	   //if the database has not been defined.
	   // true when getting heartbeat on primary, 
	   // false when received database definition
	   if (FALSE == IPO.getReceivedDBdownload()) break;  // out of if statement
	   if (FALSE == newDBupdatesStarted)
	   {
	      cout << getname() 
		   << "CommLinkObject::parseHostMessages: Started to receive "
		   << "database updates." << endl;
              newDBupdatesStarted = TRUE;
	      Interface->System.wsState |= DB_DEF_DONE_WS_STATE;
	   }//endif new DB updates have not started

	   // process the message
           parseDBUpdateMessages(PHmsg);

	   if (TRUE == needToWriteDB)
           {
              int ipn;
              //ipn = find_pid("CXDBREV");
              ipn = find_pid("CFYK3N1");

	      //if (ipn < 0) break; /// ujson add

              Interface->System.databaseRevision = (int)DBO.getvalue(ipn);
              DBO.setDBrev(Interface->System.databaseRevision);
              DBO.writeDatabase();
              needToWriteDB = FALSE;
           }
	   // if we got a data base update, we can't be in the 
	   //primary/backup changed state, so reset the bit
           Interface->System.wsState &= ~(PB_CHANGE_WS_STATE);
#ifdef DEBUG
	   // Tell how many DB updates were received from host and other
	   //useful info
	   if (0 == (DBupdateCounter % DEBUG_DISPLAY_INTERVAL))
	   {
              cout << getname() << ": Received Database Update message " 
		   << dec << DBupdateCounter 
                   << ".  CPU, M/S, FS = " << dec << PHmsg.getcpuNumber() 
                   << " " << PHmsg.getmspbStatus() << " " 
	           << PHmsg.getfailoverState() << endl;
              cout << "         Primary/Backup status for " << getname() 
	           << " is " << pb_status << endl;
              cout << "         Host1 CPU M/S, CT, DI, P/B = " 
		   << IPO.getcpuNum(1) << " " << IPO.getmasterStatus(1) 
                   << " " << IPO.getconnected(1) << " " 
	           << IPO.getfailoverState(1) << " " << IPO.getprimaryStatus(1) 
	           << endl;
              cout << "         Host2 CPU M/S, CT, DI, P/B = " 
		   << IPO.getcpuNum(2) << " " << IPO.getmasterStatus(2) 
                   << " " << IPO.getconnected(2) << " " 
	           << IPO.getfailoverState(2) << " " << IPO.getprimaryStatus(2) 
	           << endl;
           }
	   needToSignalDatabaseLoaded = FALSE;
	   DBupdateCounter++;
#endif
        }//endif Primary CommLink
        else // Backup CommLink
        {
           cout << endl << "********************" << endl;
           cout << getname() << ": Backup CommLink received Database Update "
                << "message." << endl;
           cout << "   UPDATE_DATABASE message not processed by Backup "
                << "CommLink." << endl;
           cout << endl << "********************" << endl;
        }
        break;
     } // endcase update datebase

     case HEARTBEAT:
	// keep alive message, just update packet time
	
	if (PRIMARY_COMMLINK == pb_status) 
	{
           cout << getname() << ": Received Heartbeat message from CPU " 
	        << dec << PHmsg.getcpuNumber() << " M/S, FS = " 
		<< PHmsg.getmspbStatus() << " " 
		<< PHmsg.getfailoverState() << endl;

	   Interface->System.wsState |= DB_DEF_DONE_WS_STATE;
	   newDBupdatesStarted = FALSE;

	   // If we need to let it be known that the database is loaded.
	   if (TRUE == needToSignalDatabaseLoaded)
	   {
              int i;

              // after the database is loaded, update the PID strings for 
              //the variable alarm setpoints
              for(i = 0; i < NUM_REC; i++)
              {
                 if(DBO.isanalog(i))
                 {
                    if(0 != DBO.getlIPN(i))
                    {
                       DBO.setmessage1(i,DBO.getpid(DBO.getlIPN(i)));
#ifdef DEBUG
                       cout << getname() << ": For PID " << DBO.getpid(i)
                            << ", setting variable low alarm setpoint to "
                            << DBO.getpid(DBO.getlIPN(i)) << endl;
#endif
                    } // endif got a variable high alarm setpoint
                    if(0 != DBO.gethIPN(i))
                    {
                       DBO.setmessage2(i,DBO.getpid(DBO.gethIPN(i)));
#ifdef DEBUG
                       cout << getname() << ": For PID " << DBO.getpid(i)
                            << ", setting variable high alarm setpoint to "
                            << DBO.getpid(DBO.gethIPN(i)) << endl;
#endif
                    } // endif got a variable low alarm setpoint
                 } // endif isanalog
              } // endfor all ipns

	      pid_t ppid;

	      // IF CommLink was not spawned as a child task, 
	      // tell the user that the database has been downloaded,
	      // and to Manually start InterfaceToGUI
	      ppid = getppid();
	      cout << endl << getname() << ": WSPCS ppid, pid = " << dec 
		   << ppid << " " << dec << IPO.getPidWSPCS() << endl;

              cout << getname() << ": setting" 
                   << " IPO.setReceivedDBdownload to TRUE " << endl;

              IPO.setReceivedDBdownload(TRUE);
	      needToSignalDatabaseLoaded = FALSE;
	      
	      if (IPO.getPidWSPCS() != ppid)
	      {
	         cout << "*** Manually start InterfaceToGUI - "
		      << " The data base download from the server is "
		      << "complete" << endl;
	      }
	   }//endif needToSignal == TRUE
	}//endif primary CommLink
	else 
	{
	   if (BACKUP_COMMLINK == pb_status)
	   {
	      needToSignalDatabaseLoaded = FALSE;
	   }
	}

#ifdef DEBUG
	heartbeatCounter++;
	if (heartbeatCounter % DEBUG_DISPLAY_INTERVAL == 0)
	{
           if (PRIMARY_COMMLINK != pb_status) 
	   {
              cout << getname() << ": Received Heartbeat message " 
		   << dec << heartbeatCounter << " from CPU " 
		   << PHmsg.getcpuNumber()
	           << ".  M/S = " << dec << PHmsg.getmspbStatus() << " FS = " 
		   << PHmsg.getfailoverState() << endl;
           }
           cout << "         Primary/Backup status for " << getname() 
	        << " is " << pb_status << endl;
           cout << "         Host1 CPU M/S, CT, DI, P/B = " 
		<< IPO.getcpuNum(1) << " " << IPO.getmasterStatus(1) 
                << " " << IPO.getconnected(1) << " " 
	        << IPO.getfailoverState(1) << " " << IPO.getprimaryStatus(1) 
	        << endl;
           cout << "         Host2 CPU M/S, CT, DI, P/B = " 
		<< IPO.getcpuNum(2) << " " << IPO.getmasterStatus(2) 
                << " " << IPO.getconnected(2) << " " 
	        << IPO.getfailoverState(2) << " " << IPO.getprimaryStatus(2) 
	        << endl;
	}//endif debug_display_interval
#endif

	break;

     case SET_TIME:
     {
        if (PRIMARY_COMMLINK == IPO.getprimaryStatus(getnum()))
        {
           int i;

	   // set time message, print times 
           cout << getname() << " CommLinkObject::parseHostMessages: "
	        << "Received SET_TIME message at: " 
	        << ctime(&thisMessageTime) << endl;
	
           hostTime = PHmsg.getHeaderTime();
           wsLocalTime = getLocalTime();

	   cout << "     CCUR Time is " << ctime(&hostTime) << endl;
	   cout << "     WS   Time is " << ctime(&wsLocalTime)  << endl;
           cout << "     (host-ws) time difference is "
	        << dec << (hostTime - wsLocalTime) << " second(s)" << endl;	

	   //cout << "     CCUR Time is " << ctime(&hostTime)
	   //     << "     WS   Time is " << ctime(&wsLocalTime) 
           //     << "     (host-ws) time difference is "
	   //     << dec << (hostTime - wsLocalTime) << " second(s)" << endl;	
	   // if wsState indicates a PB state change (a Failover),
	   // then if host time goes backward by more that a 
	   // timeout value, then restart tasks.
	
	   if ( ((hostTime - wsLocalTime) < FAILOVER_TIMEOUT) &&
	        (Interface->System.wsState & PB_CHANGE_WS_STATE))    
	   {
	      cout << "            CCUR Time is " << ctime(&hostTime) << endl;
	      cout << "            WS   Time is " << ctime(&wsLocalTime) << endl;

	      //cout << "            CCUR Time is " << ctime(&hostTime)
	      //     << "            WS   Time is " << ctime(&wsLocalTime)
              //     << endl; 

	      cout << "  Killing " << getname() 
	           << " because (host-ws) time difference is "
	           << dec << (hostTime - wsLocalTime) << " second(s)" 
                   << endl << endl;	 	   
	      flush(cout);
	   
	      // kill CommLink, so that WSPCS will restart tasks. 
	      kill(getpid(),SIGINT);	   
	   }
	  
           sprintf(cmdstring,"settime %d",hostTime);	
           i = system(cmdstring);
		
           switch (i) // process based upon return code
	   {
              case -1: 
	         perror("Could not process SET TIME message");     
	         cout << getname() << "     SET_TIME: "
		      << "The system() call returned with " << hex << i << endl;
	         break;

              case 0:
                 IPO.settimeOfLastRead(1, hostTime);
                 IPO.settimeOfLastRead(2, hostTime);
	         break;

	      default:
                 IPO.settimeOfLastRead(1, hostTime);
                 IPO.settimeOfLastRead(2, hostTime);
	   
	         perror("SET TIME message return code");     
	         cout << getname() << "     SET_TIME: "
	              << "The system() call returned with " << hex << i << endl;
	         break;
	   }	
	   cout << "     CCUR Time is " << ctime(&hostTime) << endl;
	   cout << "     WS   Time is " << ctime(&wsLocalTime)  << endl;
           cout << "     (host-ws) time difference is "
	        << dec << (hostTime - wsLocalTime) << " second(s)" << endl;	

	   //cout << "     CCUR Time is " << ctime(&hostTime)
	   //     << "     WS   Time is " << ctime(&wsLocalTime) 
           //     << "     (host-ws) time difference is "
	   //     << dec << (hostTime - wsLocalTime) << " second(s)" << endl;	
         } // endif primary
         break;
      } // end case 

      case CONFIRM_PRINT_SINGLE_POINT:
      {
#ifdef DEBUG
	 cout << "Received Print Confirmation Message" << endl;
#endif
     	 parseSinglePointMessages(PHmsg);
         break;
      }
     
      case CONFIRM_CHGSCN_PROCESSING:
      case CONFIRM_CHGALM_PROCESSING:
      case CONFIRM_CHGLIM_PROCESSING:
      case CONFIRM_INSVAL_PROCESSING:
      case CONFIRM_SENCAL_PROCESSING:
      case CONFIRM_LSF_PROCESSING:
      case RETURN_LSF_SCOUNT:
      {
#ifdef DEBUG
	 cout << "Received DATABASE CHANGE OPCODE = " << hex;
	 cout << PHmsg.getOpcode() << endl;
#endif
	 parseDBC_Messages(PHmsg);
	 break;
      }
	 
      case SEND_PEN_DATA:
      case CONFIRM_PEN_PROCESSING:
      {
#ifdef DEBUG
         cout << "Received PEN_RECORDER_RESPONSE" << endl;
#endif	 
	 // process the message based upon the opcode
   	 parsePenMessages(PHmsg);
         break;
      }

      case SEND_CAL_DATA:
      {
#ifdef DEBUG
         cout << "Received CAL_RESPONSE" << endl;
#endif	 
	 // process the message based upon the opcode
   	 parseCALMessages(PHmsg);
         break;
      }
      
      case SEND_BDL_DATA:
      {
#ifdef DEBUG
         cout << "Received BDL_RESPONSE" << endl;
#endif	 
	 // process the message based upon the opcode
   	 parseBDLMessages(PHmsg);
         break;
      }
      
      case SEND_ORL_DATA:
      {
#ifdef DEBUG
         cout << "Received ORL_RESPONSE" << endl;
#endif	 
	 // process the message based upon the opcode
   	 parseORLMessages(PHmsg);
         break;
      }
      
     case SEND_LINK_STATUS:
      {
	 //#ifdef DEBUG
         cout << "Received LINK_STATUS_RESPONSE" << endl;
	 //#endif	 
	 // process the message based upon the opcode
   	 parseLinkMessages(PHmsg);
         break;
      }

      case CONFIRM_ACK_PROCESSING:
      case CONFIRM_RESET_PROCESSING:
      {
#ifdef DEBUG	 
         cout << "parseHostMessages: Alarm Ack-Reset Message, opcode= " << hex << PHmsg.getOpcode()  << endl;
#endif	 
	 // process the message based upon the opcode 
	 parseAckResetMessages(PHmsg);
         break;
      }

      case CONFIRM_COLSS_PROCESSING:
         {
#ifdef DEBUG
            cout << "Received COLSS_RESPONSE" << endl;
#endif
            // process the message based upon the opcode
            parseColssMessages(PHmsg);
            break;
         }

      case SEND_DTREND_DATA:
         {
#ifdef DEBUG
            cout << "Received SEND_DTREND_DATA message, opcode= " << hex << PHmsg.getOpcode() << endl;
#endif
            // process the message based upon the opcode
            parseDTrendMessages(PHmsg);
         }
         break;

      case CONFIRM_DTREND_PROCESSING:
      case INUSE_DTREND_RESPONSE:
      case COMPLETED_DTREND_PRINT:
      case INTERRUPTED_DTREND_PRINT:
         {
#ifdef DEBUG
            cout << "Received DTREND Response message, opcode= " << hex << PHmsg.getOpcode() << endl;
#endif

            // process the message based upon the opcode
            parseDTrendMessages(PHmsg);
         }
          break;


      case SEND_BAR_DATA:
      case CONFIRM_BAR_PROCESSING:
         {
#ifdef DEBUG
            cout << "Received BAR CHART_RESPONSE" << endl;
#endif	 

            // process the message based upon the opcode
            parseBarMessages(PHmsg);
         }
         break;

      case REQ_SHOPPING_LISTS:
      {
         cout << "Received REQ_SHOPPING_LIST" << endl;

	 // Send the message with the opcode to InterfaceToGUI
	 // InterfaceToGUI will process the new message 
	 status = IGUI_MsgQue.sendMessage(PHmsg.pMsg, PHmsg.getMsgSize(),
				          PHmsg.getType());

	 if (status == 0)
	    cout << "Sent shopping list request to InterfaceToGUI " << endl;
	 else
	    cout << "Error sending shopping list request to InterfaceToGUI " << endl;

         // set all the points to BAD data while the shopping lists are being built.
         cout << getname() << " parseHostMessages() Setting points invalid\n";
         flush(cout);

//201028_jhp
/*
         //for (int i = 0; i <= NUM_REC; i++)
         //{
         //   DBO.setnewsta(i,BDACK);
         //}
*/
         break;
      }

      case PRINT_SUMMARY_CONFIRM:
         {
#ifdef DEBUG
            cout << "Received PRTSUM_CONFIRM_MESSAGES" << endl;
#endif	    

            // process the message based upon the opcode
            parsePRTSUMMessages(PHmsg);
          }
          break;
         
      case GROUP_INFO_DATA:
         {
#ifdef DEBUG
            cout << "Received GROUP_INFO_DATA" << endl;
#endif	    

            // process the message based upon the opcode
            parseGroupMessages(PHmsg);
          }
          break;
         
      case GROUP_CONFIRM:
         {
#ifdef DEBUG
            cout << "Received GROUP_CONFIRM_MESSAGES" << endl;
#endif	    

            // process the message based upon the opcode
            parseGroupMessages(PHmsg);
          }
          break;

      case DISPLAY_GROUP_FLAG:
         {
#ifdef DEBUG
            cout << "Received GROUP_FALG_STATUS_MESSAGES" << endl;
#endif

            // process the message based upon the opcode
            parseGroupMessages(PHmsg);
          }
          break;

      case HDSR_TREND1_DATA:
      case HDSR_TREND2_DATA:
      case HDSR_TREND3_DATA:
      case HDSR_TREND4_DATA:
      {
#ifdef DEBUG	 
	 cout << "Received HDSR MESSAGE" << endl;
#endif	    
	 // process the message based upon the opcode 
   	 // parseHDSRMessages(PHmsg);	
 	 break;
      }

      case XRBP_READY:
      case XRBP_DATA:
      case XRBP_MSG:

#ifdef DEBUG	 
         cout << "Received XRBP MESSAGE" << endl;
#endif	    
         // process the message based upon the opcode
         parseXRBPMessages(PHmsg);
         break;


      case XRBPA_DISPLAY:
#ifdef DEBUG	 
         cout << "Received XRBP AUTO DISPLAY MESSAGE" << endl;
#endif	    
         parseXRBPAMessages(PHmsg);
         break;

// ******************************************************
// ******************************************************

      case CEA_ALM_LIMIT_REQUEST:
#ifdef DEBUG
         cout << "(CEA):: CEA alarm limit request done " << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case ONE_CEA_POSITION_REPT:
#ifdef DEBUG
         cout << "(CEA):: one cea position report done" << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case ONE_CEA_GROUP_POSITION_REPT:
#ifdef DEBUG
         cout << "(CEA):: one cea group position report done " << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case ALL_CEAGROUP_POSITION_REPT:
#ifdef DEBUG
         cout << "(CEA):: all cea group position report done" << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case CEA_DROPNO_REQUEST:
#ifdef DEBUG
         cout << "(CEA):: CEA drop tesing request done" << endl;
#endif     
         parseRPCCEAMessages(PHmsg);
         break;

      case CEDMCS_DATAXFER_MODE:
#ifdef DEBUG
         cout << "(CEA):: CEDMCS data transfer request done"  << endl;
#endif     
         parseRPCCEAMessages(PHmsg);
         break;

      case CEA_ALM_LIMIT_PROCESS:
#ifdef DEBUG
         cout << "(CEA):: received CEA alarm limits from ccur" << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case CEA_MESSAGE_IN_BODY:
#ifdef DEBUG
         cout << "(CEA):: CEA processing confirmed" << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

      case CEA_GENERIC_PROCESS_CONFIRMED:
#ifdef DEBUG
         cout << "(CEA):: Generic process confirmed" << endl;
#endif
         parseRPCCEAMessages(PHmsg);
         break;

// ******************************************************
// ******************************************************
//
      case CONFIRM_CECOR_REQUEST:
         parseCPCSMessages(PHmsg);
         break;
      case CONFIRM_CPCEAC_REQUEST:
         parseCPCSMessages(PHmsg);
         break;
      case CONFIRM_CPCS_REQUEST:
         parseCPCSMessages(PHmsg);
         break;
      case CONFIRM_CEAC_REQUEST:
         parseCPCSMessages(PHmsg);
         break;
      case REJECT_CPCS_REQUEST:
         parseCPCSMessages(PHmsg);
         break;

// ******************************************************
// ******************************************************
//
      case RPCCEA_CURRENT_MODE:
         parseRPCCEAMessages(PHmsg);
         break;

      case RPCCEA_PROCESS_CONFIRM:
         parseRPCCEAMessages(PHmsg);
         break;

      case INCORE_CHARGE_RESPONSE:
      case INCORE_PROCESS_CONFIRM:
         parseIncoreMessages(PHmsg);
         break;

      case CONFIRM_SECURE_PROCESSING:
         parseSecureMessages(PHmsg);
         break;

      case REQUEST_SECURE_SIGNOFF:
#ifdef DEBUG
         cout <<"Received REQUEST_SECURE_SIGNOFF"<<endl;	// TEMP. CODE 2013
#endif
         parseSecureMessages(PHmsg);
         break;

      case CONFIRM_BOP_PRINT:
           parseBOPDMDMessages(PHmsg);
           break;

      case DEMANDLOG_PRINT_CONFIRMED:
      case SEND_POSTRIP_DATA:
      case POSTRIP_PRINT_CONFIRMED:
      case SEND_SOE_DATA:
      case SOE_PRINT_CONFIRMED:
      case LASTPOSTRIP_PRINT_CONFIRMED:
      case LASTSOE_PRINT_CONFIRMED:
        {
            cout <<"Received Print reports response"<<endl;
            parsePrreptMessages(PHmsg);
        }
        break;

      case CONFIRM_CECOR_DISKFILE:
      case CONFIRM_CECOR_TAPE:
      case ERROR_QUEUE_IOCPCS:
      case ERROR_CECOR_FILEOPEN:
      case ERROR_CECOR_MAG1OPEN:
      case ERROR_CECOR_DISKREAD:
      case ERROR_TAPE_NOT_MOUNT:
      case ERROR_CECOR_MAG1WRITE:
        {
#ifdef DEBUG
            cout <<"Received cecor response"<<endl;
#endif
            parseCecorMessages(PHmsg);
        }
        break;

      case PROCESS_UTDV_RECEIVE:
      case PROCESS_UTDV_RECEIVE_DATA:
#ifdef DEBUG
         cout << "****RECEIVED UTDV MESSAGE FROM CCUR ****" << endl;
#endif
         parseUTDVMessages(PHmsg);
         break;

      case ERROR_QUEUE_TASK:
      case CONFIRM_CPC_REPORT:
      case CONFIRM_CEAC_REPORT:
      case CONFIRM_PPS_REPORT:
      case CONFIRM_CEAC_TRP_RPT:
      case CONFIRM_CPC_TRP_RPT:
      case ERROR_OPEN_PRINTER:
      case CONFIRM_QUEUE_DSPMRPT:
        {
#ifdef DEBUG
            cout <<"Received DSPM response"<<endl;
#endif
            parseDspmMessages(PHmsg);
        }
        break;

         // process the message based upon the opcode
      case SEND_CSL_DISPLAY:
      case REQUEST_CSL_DISPLAY:
      case EXIT_CSL_DISPLAY:
        {
#ifdef DEBUG
            cout <<"Received CSL display"<<endl;
#endif
            parseCslMessages(PHmsg);
        }
        break;

      case RECIEVE_VTREND_DATA:
      case CONFIRM_VTREND_PROCESS:
      case CONFIRM_VTREND_FLAG:
         {
#ifdef DEBUG
            cout << "Received VTREND RESPONSE" << endl;
#endif
            parseVTRENDMessages(PHmsg);
         }
         break;

      case HDSR_INITr:

      case HDSR_PAGE_710r:
      case HDSR_PAGE_720r:
      case HDSR_PAGE_730r:
      case HDSR_PAGE_740r:
      case HDSR_PAGE_750r:

      case GRAPH_SETUP_74Xr:
      case GRAPH_VIEW_74Xr:
      case GRAPH_VIEW_LEFTr:
      case GRAPH_VIEW_RIGHTr:
      case GRAPH_VIEW_UPr:
      case GRAPH_VIEW_DOWNr:
      case GRAPH_VIEW_DONEr:

      case TAPE_HEADER_74Xr:
      case TAPE_HEADER_750r:

      case HDSR_DISK_PROTECTr:
      case HDSR_BACKUPr:
      case HDSR_CANCEL_LOGr:
      case HDSR_TAPE_HEADERr:
      case HDSR_LOGr:
      case HDSR_SECTORr:
      case HDSR_GRAPH_SETUPr:
      case HDSR_GRAPH_VIEWr:

      case DYN_MESSAGEr:
      case HDSR_FOOTER_MESSAGEr:

//#ifdef DEBUG
          cout << " *** Recieved HDSR Message" << endl;
          cout << hex << PHmsg.getOpcode() << " Size  "  <<  PHmsg.getMsgSize() << endl;
//#endif
//        parseHDSRMessages(PHmsg);

           int status;
           status = IGUI_MsgQue.sendMessage(PHmsg.pMsg,PHmsg.getMsgSize(),PHmsg.getType());
           if( status == -1 )
               cout << " ***** CC Message Sending ERROR " << endl;
           break;
           
           
// Process all Rcs Leak messages from ccur here.(by stc)----------------------------------
// CORENET ujson add 2017.0221 : case CONFIRM_MASS_CALC: 
      case SEND_RCS_LEAK_DATA:
      case SEND_LEAK_OFF_DATA:
      case CONFIRM_LEAKPRN:
      case CONFIRM_MASS_CALC:
      case CONFIRM_MASS_PRN:
      case CONFIRM_MASS_MSG:
         {
#ifdef DEBUG	 
         cout << " Received Rcs Leak RESPONSE from ccur " << endl;
#endif   
         parseRcsLeakMessages(PHmsg);
         }
         break;        

// CORENET ujson add 2017.0223 
     case CONFIRM_PIDTEST:
	 parsePidForTestMessages(PHmsg);
	 break;

     case CONFIRM_EQUIPVALCHG:
     case CONFIRM_EQUIPSTATUS:
     case CONFIRM_EQUIPPRN:
	 parseEquipmentMessages(PHmsg);
	 break;
// CORENET ujson add End

// 20061116 for CEATRACK
     case CONFIRM_CEA_TRACK_MONITOR1:
     case CONFIRM_CEA_TRACK_MONITOR2:
     case CONFIRM_CEA_TRACK_MONITOR3:
     case CONFIRM_CEA_TRACK_MONITOR4:
     case CONFIRM_CEA_TRACK_MONITOR5:
     case CONFIRM_CEA_TRACK_MONITOR6:
     case CONFIRM_CEA_TRACK_MONITOR9:
          parseCEATrackMonitorMessages(PHmsg);
          break;

//------------------------------------------------------------------------------------------
      default:
         cout << "CommLinkObject::parseHostMessages - Unknown message number: ";
	 cout << hex << PHmsg.getOpcode() << " Message follows:" << endl;
         cout << "[00]:" << hex << Local.totalMessage[0] << " "
              << "[04]:" << Local.totalMessage[1] << " "
              << "[08]:" << Local.totalMessage[2] << " "
              << "[12]:" << Local.totalMessage[3] << " "
              << "[16]:" << Local.totalMessage[4] << " "
              << "[20]:" << Local.totalMessage[5] << " " << endl;
         cout << "[24]:" << Local.totalMessage[6] << " "
              << "[28]:" << Local.totalMessage[7] << " "
              << "[32]:" << Local.totalMessage[8] << " "
              << "[36]:" << Local.totalMessage[9] << " " << endl;
         flush(cout);

         break;

   } // end of Opcode switch statement

   // check if the primary/backup status this time is different from the
   //previous time.
   if (pb_status != last_pb_status)
   {
      // create the message object
      messageObject PB_Data(TO_HOST_MSG_ID, "PB_Data");
      int writeStatus = 0;


      // Don't send the primary/backup status twice.  Above, when we process 
      //the send_master_slave_status message, we send the primary/backup
      //status to the host.  There is no need to send it twice, since it is 
      //possible that the pb_status could change on that opcode.
      if (SEND_MASTER_SLAVE_STATUS != PHmsg.getOpcode())
      {
         // Set the opcode in the header
         //to send Primary/Backup status to host
         PB_Data.setHeader(SEND_PRIMARY_BACKUP_STATUS, 1);

         // Set the master/slave primary/backup status of the
         //workstation in the message.
         PB_Data.setmspbStatus(pb_status);

         writeStatus = writeMessageToHost(PB_Data);
      }

      if (writeStatus < 0)
      {
         cout << getname()
              << "parseHostMessages: On pb_status change: Error sending "
              << "Primary/Backup status back to host\n";
      }
      else
      {
         cout << endl << endl << endl;
         cout << "*************************************" << endl;
         cout << "*************************************" << endl;
         cout << "*************************************" << endl;
         cout << getname() << "::parseHostMessages "
              << "     PB status changed from " << dec 
              << last_pb_status << " to " << pb_status << endl;

         // if we changed from backup to primary commlink, set state
         if (PRIMARY_COMMLINK == pb_status  &&  
                PRIMARY_COMMLINK != last_pb_status &&
	           TRUE == IPO.getReceivedDBdownload())
         {
            cout << "     Indicating to wsState a change from Backup " 
                 << "Commlink = " << dec << last_pb_status 
                 << " to Primary Commlink = "  << pb_status << endl;
	    Interface->System.wsState |= PB_CHANGE_WS_STATE;

	    // If the primary is not locked
	    if(!IPO.getprimaryLocked())
	    {
	       // Create message object
               messageObject FOmsg(FROM_HOST_MSG_ID,"FOmsg");

	       // set the opcode
	       FOmsg.setOpcode(PB_STATUS_CHANGE_ON);

	       // send the message to Interface2GUI
               status = IGUI_MsgQue.sendMessage(FOmsg.pMsg,FOmsg.getMsgSize(),
					    FOmsg.getType());
            }
         }

	 cout << "*************************************" << endl;
	 cout << "*************************************" << endl;
	 cout << "*************************************" << endl;
	 cout << endl << endl << endl;

         cout << getname() << " CommLinkObject::parseHostMessages: "
              << " On pb_status change: Sent Primary/Backup "
              << "status = " << dec << pb_status << " back to host\n";
      }//endelse writeMessageToHost() OK.
   } //endif change in pb_status
   flush(cout); // ensure all messages appear at the terminal.

}// end parseHostMessages

void CommLinkObject::closeAndReconnect()
{  
       time_t          wsLocalTime;

       close(ReadSocket);	      // close socket with error
       close(WriteSocket);
       total_bytes_read = 0;          // reset read counts
       bytes_read = 0;
       bytesToRead = sizeof(Local.header);
       msgSize = sizeof(Local.char_buf);
       gotMsgSize = FALSE;

       wsLocalTime = getLocalTime(); 
       cerr << " Attempt reconnect to host at: " 
   	    << ctime(&wsLocalTime) << endl;
   	  
       Connect();   // re-open connection
}

