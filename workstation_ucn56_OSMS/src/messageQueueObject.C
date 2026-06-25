//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// messageQueueObject.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This file implements the message queue object.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>

#include <sys/msg.h>
#include <sys/unistd.h>
#include <errno.h>
//
// User Defined Include Files:
//
#include "messageParameters.h"
#include "databaseParameters.h"
#include "messages.h"
#include "messageQueueObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
//#ifndef DEBUG
//   #define DEBUG
//#endif
//
//
// Function Prototypes: NONE
//
#include "prototypes.h"
//
// ***************************************************************************
//
messageQueueObject::messageQueueObject(key_t key, int rflag)
{
   removeOnExit = rflag;
   Qid = msgget(key, MESSAGE_PERMISSIONS | IPC_CREAT);
   cout << "Message queue has been created for key = " << hex << key;
   cout << " id = " << hex << Qid << endl;

   flush();
}

messageQueueObject::~messageQueueObject()
{
   cout << "Message queue destructor called for id = " << hex << Qid << endl;
   if (removeOnExit)
   {
      msgctl(Qid, IPC_RMID, NULL);
   }
   else
   {
      cout << " Not Removing message queue id = " << hex << Qid << " from the system\n";
   }
}


void messageQueueObject::flush()
{
   int len;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   int buf[1000];
   while (len = (receiveMessage (buf, NOWAIT, QNEXT_MSG)) != -1)
   // flush message queue of erroneous messages
   //while (len = (receiveMessage (NULL, NOWAIT, QNEXT_MSG)) != -1)
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   {
#ifdef DEBUG
      cout << "messageQueueObject::flush() flushing messages in loop" << endl;
      cout << "                            len = " << len << endl;
#endif
   };
}

int messageQueueObject::sendMessage (const void * buffer, int length, long type)
{
   int retval;
   struct msgbuf1
   {
      long	   messagetype;
      unsigned int message[WS_MESSAGE_LENGTH/sizeof(int)];
   } sndbuf, *msgp;
#ifdef DEBUG
   msqid_ds ds;
   msgctl(Qid,IPC_STAT,&ds);
#endif
// Start of Program

#ifdef DEBUG
   cout << "messageQueueObject::sendMessage() attempting to send a message" << endl;
   cout << "Qid = " << hex << Qid << " *buffer = " << hex << (void*)buffer;
   cout << " length = " << length;
   cout << " type = " << type << endl;
#endif
   msgp = &sndbuf;                      // Point to user send structure
   msgp->messagetype = type;
   memcpy (sndbuf.message, buffer, length);

#ifdef DEBUG
   cout << "About to call msgsnd(Qid,msgp,length,IPC_NOWAIT) " << hex << Qid;
   cout << ", " << hex << msgp << ", " << length << ", " << IPC_NOWAIT << endl;
   cout << "Message = " << hex << msgp->message[0] << " " << hex << msgp->message[1] << " ..." << endl;
#endif

   retval = msgsnd(Qid,msgp,length,IPC_NOWAIT);
#ifdef DEBUG
   cout << " #bytes: maxbytes: #msgs: = " << dec << ds.msg_cbytes << " "
        << ds.msg_qbytes << " " << ds.msg_qnum << endl;
#endif
   if (-1 == retval)
   {
      cout << "***** ERROR sending message to message queue. error = " << dec << errno << endl;
   }
  
#ifdef DEBUG
   cout << "Returning result of msgsnd = " << hex << retval << endl;
#endif
   return(retval);
}


int messageQueueObject::receiveMessage (void * buffer, int mode, long type)
{
//   Copies a workstation buffer pointed to in a message queue to the passed buffer
// and returns the length of the message received from the system message
// queue.

   int  length;         // the return code
   struct msgbuf1
   {
      long messagetype;
      unsigned int message[WS_MESSAGE_LENGTH/sizeof(int)];
   } rcvbuf, *msgp;

// Start of Program
   msgp = &rcvbuf;	// Point to user send structure

   length = msgrcv (Qid, msgp, WS_MESSAGE_LENGTH, type, mode);
   if (length != -1)
   {
#ifdef DEBUG
       cout << "Received from msgrcv " << hex << length << endl;
       cout << "Message = " << hex << rcvbuf.message[0] << " " << hex << rcvbuf.message[1];
       cout << " ..." << endl;
#endif
       memcpy (buffer, rcvbuf.message, length);
   }
   return(length);
}

