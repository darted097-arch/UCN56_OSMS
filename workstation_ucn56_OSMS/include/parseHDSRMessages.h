#ifndef _PARSE_HDSRMESSAGES_H
#define _PARSE_HDSRMESSAGES_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseHDSRMessages.h  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  Prototype definitions for HDSR.
// **************************************************************************************
//
// **************************************************************************************
//
#include <api/api.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>		// by thlee
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messageObject.h"

int receiveMessage(int msqid, void *message, int mode, long type);

   struct DATE_BIT {
      unsigned yearBit: 16;
      unsigned monthBit: 8;
      unsigned dayBit: 8;
   };

void parseHDSRMessage_send(messageObject & msg);
int parseHDSRMessage_receive( messageObject &Msg);
int tConvertIntToDate( int iDate, char *p_cDate );
int tGetIntRtm( char *rtmStruct, char* rtmVar);
int tGetCharRtm( char *rtmStruct, char* rtmVar, char *string );
int tPutIntRtm( char *rtmStruct, char* rtmVar, int val);
int tPutFloatRtm( char *rtmStruct, char* rtmVar, float val);
int tPutCharRtm( char *rtmStruct, char* rtmVar, char *string);
int tPutFArrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, float *val);
int tPutFarrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, float val);
int tPutIArrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, int *val);
int tPutIarrayRtm( char *rtmStruct, char* rtmVar, int arrayNo, int val);
int tGetDate( char *dString );
int tGetTime( char *dString );
//int tValidatePoint( char *point);
int tValidateDiskDate( char *iString, char *sString, char *eString );
int tConvertToCcString( char *ccString, char *string, int num );
int tConvertIntToTime( int iTime, char *p_cTime );
int tStrncpy( char *string, char *CCstring, int num );

#endif
