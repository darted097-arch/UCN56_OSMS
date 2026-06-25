#ifndef	_INTER_PROC_SMOBJECT_H
#define	_INTER_PROC_SMOBJECT_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// InterProcSMObject.h  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// *****************************************************************************
// This file defines the InterProcSMObject class.
// *****************************************************************************
//
// *****************************************************************************
//
// System Include Files:
//
#include <iostream.h>
//
// User Defined Include Files:
//
#include "sharedMemoryObject.h"
#include "interProcSM.h"
//
// External Variables:
//
// None
//
// Local Constant Definitions:
//
// None
//
// Function Prototypes:
//
// None
//
class InterProcSMObject : public sharedMemoryObject
{
public:
 	InterProcSMObject(key_t key, int size, int mode);
       ~InterProcSMObject();

// "set" methods
   static void setPidCommLink1(int v){pSM->pidCommLink1 = v;};
   static void setPidCommLink2(int v){pSM->pidCommLink2 = v;};
   static void setPidRTM(int v){pSM->pidRTM = v;};
   static void setPidControl(int v){pSM->pidControl = v;};
   static void setPidInterfaceToGUI(int v){pSM->pidInterfaceToGUI = v;};
   static void setPidFDDIstart(int v){pSM->pidFDDIstart = v;};
   static void setPidTicker(int v){pSM->pidTicker = v;};
   static void setPidTimer(int v){pSM->pidTimer = v;};
   static void setPidWSPCS(int v){pSM->pidWSPCS = v;};
   static void setPidDBWSPCS(int v){pSM->pidDBWSPCS = v;};
   static void setReceivedDBdownload(int v){pSM->receivedDBdownload = v;};   
   static void setprimaryLocked(int i){pSM->primaryLocked = i;};
   static void setNumTicks(int i){pSM->numTicks = i;};
   static void incNumTicks(){pSM->numTicks++;};
   static void setNumTicksOfLastRead(int hostNum, int t)
   {
      pSM->hosts[hostNum-1].numTicksOfLastRead = t;
   };
   static void setMODEMOption(const char * s);
   static void setSENDOption(const char * s)
   {
      strncpy(pSM->SEND, s, 5);
   };
// host connection information
   static void setHostName(int hostNum, char * n)
   {
      strncpy(pSM->hosts[hostNum-1].hostName,n, MAX_HOSTNAME_LEN);
   };
   static void setPortNo(int hostNum, char * n)
   {
      strncpy(pSM->hosts[hostNum].portNo, n, MAX_PORTNO_LEN); 
      //strncpy(pSM->hosts[hostNum-1].portNo, n, MAX_PORTNO_LEN); 
   };
   static void setHostNum(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].hostNum = n;
   };
   static void setcpuNum(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].cpuNum = n;
   };
   static void setconnected(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].connected = n;
   };
   static void setfailoverState(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].failoverState = n;
   };
   static void setmasterStatus(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].masterStatus = n;
   };
   static void setprimaryStatus(int hostNum, int n)
   {
      pSM->hosts[hostNum-1].primaryStatus = n;
   };
   static void settimeOfLastRead(int hostNum, time_t t)
   {
      pSM->hosts[hostNum-1].timeOfLastRead = t;
   };

   // ujson add 20210302
   static void setPidDBCommLink1(int v){pSM->pidDBCommLink1 = v;};
   static void setPidDBCommLink2(int v){pSM->pidDBCommLink2 = v;};
   static void setDBHostNum(int hostNum, int n)
   {
      pSM->DBhosts[hostNum-1].hostNum = n;
   };
   static void setDBHostName(int hostNum, char * n)
   {
      strncpy(pSM->DBhosts[hostNum-1].hostName,n, MAX_HOSTNAME_LEN);
   };
   static void setDBPortNo(int hostNum, char * n)
   {
      strncpy(pSM->DBhosts[hostNum-1].portNo, n, MAX_PORTNO_LEN); 
   };


// "get" methods
   static int	getPidCommLink1(){return pSM->pidCommLink1;};
   static int	getPidCommLink2(){return pSM->pidCommLink2;};
   static int	getPidRTM(){return pSM->pidRTM;};
   static int	getPidControl(){return pSM->pidControl;};
   static int	getPidInterfaceToGUI(){return pSM->pidInterfaceToGUI;};
   static int	getPidFDDIstart(){return pSM->pidFDDIstart;};
   static int	getPidTicker(){return pSM->pidTicker;};
   static int	getPidTimer(){return pSM->pidTimer;};
   static int	getPidWSPCS(){return pSM->pidWSPCS;};
   static int   getPidDBWSPCS(){return pSM->pidDBWSPCS;};
   static int	getReceivedDBdownload(){return pSM->receivedDBdownload;};
   static int	getprimaryLocked(){return pSM->primaryLocked;};
   static int	getNumTicks(){return pSM->numTicks;};
   static int   getNumTicksOfLastRead(int hostNum)
                {
                   return pSM->hosts[hostNum-1].numTicksOfLastRead;
                };
   static char *getMODEMOption(char * s)
                { 
                   strcpy(s, pSM->MODEM);
                   return s;
                };
   static char *getSENDOption(char * s)
                {
                   strcpy(s, pSM->SEND);
                   return s;
                };
   static char *getHostName(int hostNum)
		{
		   return pSM->hosts[hostNum-1].hostName;
		};
   static int	getHostNum(int hostNum)
		{
		   return pSM->hosts[hostNum-1].hostNum;
		};
   static char *getPortNo(int hostNum)
                {
                   return pSM->hosts[hostNum].portNo;
                   //return pSM->hosts[hostNum-1].portNo;
                };
   static int	getcpuNum(int hostNum)
   		{
		   return pSM->hosts[hostNum-1].cpuNum;
   		};
   static int	getconnected(int hostNum)
		{
		   return pSM->hosts[hostNum-1].connected;
		};
   static int	getfailoverState(int hostNum)
		{
		   return pSM->hosts[hostNum-1].failoverState;
		};
   static int	getmasterStatus(int hostNum)
		{
		   return pSM->hosts[hostNum-1].masterStatus;
		};
   static int	getprimaryStatus(int hostNum)
		{
		   return pSM->hosts[hostNum-1].primaryStatus;
		};
   static time_t gettimeOfLastRead(int hostNum)
		{
		   return pSM->hosts[hostNum-1].timeOfLastRead;
		};

   // ujson add 20210302
   static int	getPidDBCommLink1(){return pSM->pidDBCommLink1;};
   static int	getPidDBCommLink2(){return pSM->pidDBCommLink2;};
   static char *getDBHostName(int hostNum)
		{
		   return pSM->DBhosts[hostNum-1].hostName;
		};
   static char *getDBPortNo(int hostNum)
                {
                   return pSM->DBhosts[hostNum-1].portNo;
                };
   static int	getDBHostNum(int hostNum)
		{
		   return pSM->DBhosts[hostNum-1].hostNum;
		};


private:
   static interProc *  pSM;   
};
#endif
