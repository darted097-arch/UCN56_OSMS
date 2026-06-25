#ifndef _MESSAGE_OBJECT_H
#define _MESSAGE_OBJECT_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// messageObject.h      
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the shared memory object that handles shared memory 
//  operations.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
//
// User Defined Include Files:
//
#include "messageParameters.h"
#include "messages.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
// Standard Workstation Message Header Length
// All messages on the workstation have the same header.
const int MSG_HEADER_LEN	= 24;	// number bytes reserved for header info

// HOST MESSAGE - a message from the host
// HOST_MESSAGE_LENGTH must match WSOBUFSIZ in WSCOMM.INC on the host
const int FROM_HOST_MSG_LEN	= 4096;	
const int FROM_HOST_DATA_LEN	= (FROM_HOST_MSG_LEN - MSG_HEADER_LEN);
const int TO_HOST_MSG_LEN	= 1024;
const int TO_HOST_DATA_LEN	= (TO_HOST_MSG_LEN - MSG_HEADER_LEN);

const long TO_HOST_MSG_ID	= 10;
const long FROM_HOST_MSG_ID	= 11;

//

class messageObject
{
  public:
     		messageObject();
     		messageObject(char * objectName);
		messageObject(long id);
		messageObject(long id, char * Name);
    	       ~messageObject();
// set methods
     void	setMessage(const void * data, int msgSize);
     void	setData(const void * data, int size);
     void	setHeader(int opcode, int numberOfSomething);
// set attribute access methods
     void	setOpcode(int code){pHeader->opcode = code;};
     void	setmspbStatus(short pb){pHeader->mspbStatus = pb;};
     void	setfailoverState(int s){pHeader->disabilityIndex = s;};
     void	setMsgSize(int s){pHeader->messageSize = s;};

// get methods
     void	getMessage(void * destination, int destSize);
     void	getData(void * destination, int destSize);
     char *	getName(){return name;};
     long	getType(){return type;};
     int	getmaxMsgSize(){return(MSG_HEADER_LEN + maxDataSize);}; 
// get attribute access methods
     int	getOpcode(){return pHeader->opcode;};
     int	getNumberOfSomething(){return pHeader->numberOfSomething;};
     time_t	getHeaderTime();
     short	getmspbStatus(){return((pHeader->mspbStatus) & 0x00ff);};
     short	getcpuNumber(){return((pHeader->mspbStatus) >> 8);};
     int	getfailoverState(){return pHeader->disabilityIndex;};
     int	getMsgSize(){return pHeader->messageSize;};
     int	getDataSize(){return pHeader->messageSize - MSG_HEADER_LEN;};

#ifdef LINUX_APP
     char *	getDataPtr() { return pData;};	// CORENET Co, 2012.07.09
#endif

     char *	pMsg;

  private:
     long		type;
     void		setType(long t){type = t;};
     void		createMessage(long id, char * n);
     void		printDebugText();
     WSmessageHeader *	pHeader;
     char *		pData;
     unsigned int	maxDataSize;
     char		name[32];
     
     //WSDBmessageHeader * pDBHeader;  //210118_jhp
};
#endif

