#ifndef _DBWSPCSOBJECT_H
#define _DBWSPCSOBJECT_H

// **************************************************************
// DBWSPCSObject.h
// ver 1.0 : 2021.1.8
// Author : Ji Hoon Park
// **************************************************************

// **************************************************************
// This file defines the DBCommLinkObject class.
// **************************************************************

#include <netinet/in.h>
#include <iostream.h>
#include <map>

#include "DBCommLinkObject.h"
#include "systemParameters.h"

const int HOST1_NAME_INDEX = 0;
const int HOST1_PORTNO_INDEX = 1;
const int HOST2_NAME_INDEX = 2;
const int HOST2_PORTNO_INDEX = 3;
const int SEND_USE_INDEX = 4;

struct OPTION
{
    char *  str;
    int     used;
};

static OPTION options[] =
{
    {"-HOST1", FALSE},
    {"-PORT1", FALSE},
    {"-HOST2", FALSE},
    {"-PORT2", FALSE},
    {"-SEND", FALSE},
};

struct DBWSPCSinfoStruct
{
    char     hostname[DB_NUM_HOSTS][MAX_HOSTNAME_LEN+1];
    char     hostport[DB_NUM_HOSTS][MAX_PORTNO_LEN+1];   
    char     SEND[5];
};

const static int NUM_OPTIONS = (sizeof(options) / sizeof(struct OPTION));

class DBWSPCSObject
{
public:
			    DBWSPCSObject();
			    ~DBWSPCSObject();

    static      void        startAll();
    static      void        stopAll();
    static      void        mainLoop();

    static      void        setDBWSPCSinfo(char *);
    static      void        parseDBOptions(int, char * *);

    static      void        setHost1Name(const char * s)    { strcpy(DBWSPCSinfo.hostname[0], s); }
    static      void        setHost2Name(const char * s)    { strcpy(DBWSPCSinfo.hostname[1], s); }
   
    static      void        setHost1PortNo(const char * s)  { strcpy(DBWSPCSinfo.hostport[0], s); }
    static      void        setHost2PortNo(const char * s)  { strcpy(DBWSPCSinfo.hostport[1], s); }

    static      void        setSENDOption(const char * s)   { strcpy(DBWSPCSinfo.SEND, s); }


    static      char *      getHost1Name(char * s)
		            {
			        strcpy(s, DBWSPCSinfo.hostname[0]);
			        return s;
	   		    }
    
    static      char *      getHost2Name(char * s)
			    {
			        strcpy(s, DBWSPCSinfo.hostname[1]);
				return s;
			    }

    static      char *      getHost1PortNo(char * s)
			    {
			        strcpy(s, DBWSPCSinfo.hostport[0]);
			        return s;
			    }

    static      char *      getHost2PortNo(char * s)
   	   	            {
                                strcpy(s, DBWSPCSinfo.hostport[1]);
			        return s;
                            }

    static      char *      getSENDOption(char *s)
                            {
                                strcpy(s, DBWSPCSinfo.SEND);
				return s;
                            }

private:
    static      DBWSPCSinfoStruct     DBWSPCSinfo;

    int         pidCommLink1;
    int         pidCommLink2;
    int         pidDBWSPCS;
};

#endif
