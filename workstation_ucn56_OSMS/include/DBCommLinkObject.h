#ifndef _DBCOMMLINKOBJECT_H
#define _DBCOMMLINKOBJECT_H

//
// *****************************************************************
// DBCommLinkObject.h
// ver 1.0 - 2021.1.8
// Author : Ji Hoon Park
// *****************************************************************

// ****************************************************************
// This file defines the DBCommLinkObject class.
// *****************************************************************

#include <netinet/in.h>
#include <iostream.h>
#include <map>

#include "Typedefs.h"
#include "systemParameters.h"
#include "messageObject.h"

const int        DB_NUM_HOSTS       = 2;

const time_t     DB_PACKET_TIMEOUT  = 30;          // seconds before datalink timeout
const int        DB_SLEEPTIME       = 20000;       // microseconds to wait between hosts
const int        DB_CNCT_TRY_LIMIT  = 10;          // # of tries to connect()
const int        DB_CNCT_RETRY_WAIT = 3;           // seconds to wait between connect tries

const int        MAX_DB_COMMLINK_NAME_LEN = 12;    // Len of DBCommLink Process Name

const int        MAX_DB_PID_SIZE = 12;

#define HOST_NAME_A "ygn6c1"
#define HOST_NAME_B "ygn6c2"

class DBCommLinkObject
{
public:
	             DBCommLinkObject();
	             ~DBCommLinkObject();

    static void      Connect();
    static void      mainLoop();
    static void      setnum(int v) {num = v;};
    static void      setname(char *n) {strncpy(name, n, MAX_DB_COMMLINK_NAME_LEN);};
    static void      sethostName(char *n) {strncpy(hostName, n, MAX_HOSTNAME_LEN);};
    static void      setportNo(char *n) {strncpy(portNo, n, MAX_PORTNO_LEN);};
    static void      setsendUse(char *n) {strncpy(sendUse, n, 5);};

    static int       getnum() {return num;};
    static char      *getname() {return name;};
    static char      *gethostName() {return hostName;};
    static char      *getportNo() {return portNo;};         
    static char      *getsendUse() {return sendUse;};
    static bool      parseEOF();  
    static void      parseTagID(unsigned char *buf, int buflen);
    static int       isconnected() {return connected;}; 
    static void      setconnected() {connected = TRUE;};
    static void      resetconnected() {connected = FALSE;};
    static ofstream  eventFile;
    static ofstream  tagIDFile;

private:
  
    // connect to host, low level
    static int       u_connect(u_port_t port, char * hostName);
    
    // enhanced read function 
    static ssize_t   u_read(int fd, unsigned char *buf, size_t size);  
    static int       u_ignore_sigpipe();                       

    static void      initDBCommLink();
    static int       connectEOFToHost();         // connect to host
    static void      parseDBDownMessages();  
    
    static int       bytes_read;
    static int       total_bytes_read;

    static int       msgSize;
    static int       gotMsgSize;
    static int       bytesToRead;

    static int       ReadSocket;    // UDP socket for reading
    static int       WriteSocket;   // UDP socket for writing

    static int       num;                // Which CommLink this is, 1 or 2
    static char      name[MAX_DB_COMMLINK_NAME_LEN];  // Process name of this CommLink
    static char      hostName[MAX_HOSTNAME_LEN]; // Name of host connected to   
    static int       connected;
    static char      portNo[MAX_PORTNO_LEN]; // portNo of host connected to 
    static char      sendUse[5];
};
#endif
