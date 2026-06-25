#ifndef _MESSAGE_QUEUE_OBJECT_H
#define _MESSAGE_QUEUE_OBJECT_H
//
// **********************************************
// hbnpp 3&4 pms display 
// 
// messageQueueObject.h 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object is the message queue which passes messages from 
//  CommLink to InterfaceToGUI and processes the messages in that queue.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
#include <sys/msg.h>
//
// User Defined Include Files:
//
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
// message queue for messages coming to CommLink
#define	WS_MESSAGE_KEY	(key_t) 0x1badface      // key for message queue


// message queue for messages coming to Interface2GUI.
#define COMM2GUI_KEY	(key_t) 0x1c2eface	// key for message queue

// message queue for messages coming to Interface2GUI.
#define CC_MESSAGE_KEY  (key_t) 0x2c2eface      // by thlee

// miscellaneous constants
#define	MESSAGE_PERMISSIONS	0666	// read/write access for the world
#define	WAIT			0	// for receive, wait for response
#define	NOWAIT		IPC_NOWAIT	// for receive, don't wait for response

const   long    QNEXT_MSG = 0;
const   long    QPICTURE_ACTION_MSG = 1;

//
// Function Prototypes: NONE
//
//
// ***************************************************************************
//


class messageQueueObject
{
  public:
     messageQueueObject(key_t key, int rflag);
    ~messageQueueObject();
     int receiveMessage(void *buffer, int mode, long type);
     int sendMessage(const void *buffer, int length, long type);
     void flush();
     int  getQid() {return(Qid);};
  private:
     int	Qid;		// message queue ID
     int	removeOnExit;	// 
};

#endif
