//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// messageObject.C      
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
#include <stdlib.h>
#include <iostream.h>
#include <time.h>
//
// User Defined Include Files:
//
#include "messageObject.h"
#include "messages.h"
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
// Function Prototypes: NONE
//
#include "prototypes.h"
//
// ***************************************************************************
//
messageObject::messageObject()
{
   createMessage(TO_HOST_MSG_ID, "message:DefaultToHostMessage");
#ifdef DEBUG
   printDebugText();
#endif
}

messageObject::messageObject(char * n)
{
   createMessage(TO_HOST_MSG_ID, n);
#ifdef DEBUG
   printDebugText();
#endif
}


messageObject::messageObject(long id)
{
   createMessage(id, "message:DefaultName");
#ifdef DEBUG
   printDebugText();
#endif   
}

messageObject::messageObject(long id, char * n)
{
   createMessage(id, n);
#ifdef DEBUG
   printDebugText();
#endif
}


messageObject::~messageObject()
{
#ifdef DEBUG
   cout << "messageObject destructor for name = " << name << endl;
#endif
}

void messageObject::createMessage(long id, char * n)
{
   setType(id);
   strncpy(name, n, sizeof(name));
   name[sizeof(name)-1] = '\0';	// by Lee.M.W

   // allocate the memory
   switch(type)
   {
      case TO_HOST_MSG_ID:
	 maxDataSize = TO_HOST_DATA_LEN;
	 pMsg = (char *)malloc(TO_HOST_MSG_LEN);
	 break;
      case FROM_HOST_MSG_ID:
	 maxDataSize = FROM_HOST_DATA_LEN;
	 pMsg = (char *)malloc(FROM_HOST_MSG_LEN);
	 break;
      default:
	 pMsg = NULL;
	 break;
   }
   if(NULL == pMsg)
   {
      cout << getName() << ": ERROR - Can Not Allocate Memory\n";
      exit(1);
   }
   pHeader = (WSmessageHeader *)pMsg;
   pData = pMsg + MSG_HEADER_LEN;
   setHeader(0,0);	// init header
   setMsgSize(MSG_HEADER_LEN);
}

#ifdef DEBUG
void messageObject::printDebugText()
{
   cout << "messageObject constructor for name = " << name << endl;
   cout << " Address of message = " << hex << (void *)pMsg << endl;
   cout << " Address of Header = " << hex << (void *)pHeader << endl;
   cout << " Address of Data = " << hex << (void *)pData << endl;
   cout << " Max Size of Data = " << dec << maxDataSize << endl;
   cout << " Message Size = " << dec << getMsgSize() << endl;
}
#endif   

void messageObject::setHeader(int theOpcode, int numberOfSomething)
{
   struct tm *	current;		// current date and time structure
   time_t	seconds;		// seconds since 1/1/70

   // get the current time for the message header
   time(&seconds);
   current = localtime(&seconds);

   cout << "theOpcode = " << hex << theOpcode << endl;
   
   pHeader->opcode = theOpcode;
   pHeader->numberOfSomething = numberOfSomething;

   pHeader->dateAndTime[0] = (char) current->tm_year;

   // if year greater than 2000, convert to two digit year for host
   if(pHeader->dateAndTime[0] >= 100) pHeader->dateAndTime[0] -= 100;

   pHeader->dateAndTime[1] = (char) current->tm_mon+1;
   pHeader->dateAndTime[2] = (char) current->tm_mday;
   pHeader->dateAndTime[3] = (char) current->tm_hour;
   pHeader->dateAndTime[4] = (char) current->tm_min;
   pHeader->dateAndTime[5] = (char) current->tm_sec;

} //end setmessageHeader

void messageObject::setData(const void * data, int size)
{
#ifdef DEBUG
   cout << name << ".setData: Request to copying " << dec << size 
        << " bytes of data from " << hex << (void*)data << " to " << hex 
        << (void *)pData << endl;
#endif
   if (size <= maxDataSize)
   {
      memcpy(pData, data, size);
      setMsgSize(MSG_HEADER_LEN + size);
   }
   else
   {
      cout << "messageObject::setmessageData - WARNING: size of data larger "
	   << "than size of data portion of message structure\n";
      cout << " Only " << maxDataSize << " bytes will be copied into the "
	   << "data portion of the message\n";
      memcpy(pData, data, maxDataSize);
      setMsgSize(MSG_HEADER_LEN + maxDataSize);
   }
#ifdef DEBUG
   cout << name << ".setData: Message size set to " << dec << getMsgSize() 
        << endl;
#endif
}

void messageObject::setMessage(const void * data, int msgSize)
{
//#ifdef DEBUG
// The next two messages will appear every second when uncommented 
//and DEBUG is defined.
//   cout << name << ".setMessage(): Copying " << dec << msgSize 
//	<< " byte message from " << hex << (void*)data << " to " << hex 
//	<< (void *)pMsg << endl;
//#endif
   // if the input message can fit into this object
   if (msgSize <= (maxDataSize + MSG_HEADER_LEN))
   {
      memcpy(pMsg, data, msgSize);
      setMsgSize(msgSize);
   }
   else // input message too large
   {
      cout << "messageObject::setMessage - ERROR: size of input message "
	   << "larger than size of message\n";
      cout << " Only " << dec << MSG_HEADER_LEN+maxDataSize << " bytes will be "
	   << "copied into message\n";
      memcpy(pMsg, data, (maxDataSize + MSG_HEADER_LEN));
      setMsgSize(MSG_HEADER_LEN + maxDataSize);
   }
}


void messageObject::getMessage(void * destination, int destSize)
{
#ifdef DEBUG
// The next statement will execute every second when uncommented 
//and DEBUG is defined.
 if(getMsgSize() != destSize)
 {
   cout << name << ".getMessage(): Copying " << dec << getMsgSize() 
	<< " byte message from " << hex << (void*)pMsg << " to " 
	<< dec << destSize << " byte destination " << hex << destination 
	<< endl;
 }
#endif
   // if the destination buffer is large enough to hold the message
   if (destSize >= getMsgSize()) 
   {
      // if the destination buffer is bigger than the max size of the message
      if (destSize > (MSG_HEADER_LEN + maxDataSize))
      {
        cout << "messageObject::getMessage - WARNING: number of bytes "
	     << "requested is larger than MAX size of the message\n";
        cout << " Only " << dec << getMsgSize() << " bytes "
	     << "will be copied into destination\n";
      }
      memcpy(destination, pMsg, getMsgSize());
   }
   else // the destination buffer is too small for the message.
   {
      cout << "messageObject::getMessage - ERROR: size of message (" << dec
	   << getMsgSize() << ") is larger than size of destination ("
	   << destSize << ")" << endl;
      cout << " Only " << dec << destSize << " bytes "
	   << "will be copied into destination\n";
      memcpy(destination, pMsg, destSize);
   }
}


void messageObject::getData(void * destination, int destSize)
{
#ifdef DEBUG
   cout << name << ".getData(): Copying " << dec << getDataSize() 
	<< " bytes of data from " << hex << (void*)pData << " to " 
	<< dec << destSize << " byte buffer " << hex << destination << endl;
#endif
   // if the destination buffer is large enough to hold the data portion 
   //of the message
   if (destSize >= getDataSize())
   {
      if (destSize > maxDataSize)
      {
         cout << "messageObject::getData - WARNING: size of destination larger "
	      << "than MAX size of message data\n";
         cout << " Only = " << dec << getDataSize()
	      << " bytes will be copied into destination\n";
      }
      memcpy(destination, pData, getDataSize());
   }
   else // destination buffer is too small for the data portion of the message
   {
      cout << "messageObject::getData - ERROR: size of destination (" << dec
	   << destSize << ") is too small to hold the data portion of the " 
	   << "message.(" <<  getDataSize() << ")" << endl;
      cout << " Only = " << dec << destSize
	   << " bytes will be copied into destination\n";
      memcpy(destination, pData, destSize);
   }
}


time_t messageObject::getHeaderTime()
{
   struct tm    timeFields;             // to convert to seconds

   timeFields.tm_year = (int) pHeader->dateAndTime[0];

   // if two digit year from host is less than 70, i.e., 1970, 
   //convert to years since 1900. If 2 digit year is less than 70, 
   //the year is actually greater than 1970. Only worrying about 2000 
   //thru 2069.
   if (timeFields.tm_year < 70) timeFields.tm_year += 100;
   
   timeFields.tm_mon  = (int) pHeader->dateAndTime[1]-1;
   timeFields.tm_mday = (int) pHeader->dateAndTime[2];

   timeFields.tm_hour = (int) pHeader->dateAndTime[3];
   timeFields.tm_min  = (int) pHeader->dateAndTime[4];
   timeFields.tm_sec  = (int) pHeader->dateAndTime[5];

   // This field is for Daylight Savings time.  Check the time zone in Asia.
   // If the CCUR time versus the workstation time differs by 1 hour, then
   // look at this code.

   // get the current time zone
//   tzset();
   // if Daylight savings, set to 1, otherwise 0
/*   if (tzname[1])
      timeFields.tm_isdst = 1;
   else
*/
      timeFields.tm_isdst = 0;

   // return the time in seconds since the EPOCH
   return (mktime(&timeFields) );
}

