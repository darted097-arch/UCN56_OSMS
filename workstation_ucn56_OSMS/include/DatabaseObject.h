#ifndef	_DATABASE_OBJECT_H
#define	_DATABASE_OBJECT_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// DatabaseObject.h     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// This file defines the CommLinkObject class.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
//
#include <iostream.h>
//
// User Defined Include Files:
//
#include "database.h"
#include "databaseParameters.h"
#include "sharedMemoryObject.h"
//
// External Variables:
//
//
// Local Constant Definitions:
//
#define 	DEFAULT_DBREV 	-999
#define		NOT_A_DBREV	-998
#define 	DB_FILENAME 	"dbimage.bin"
const int	DB_FILEKEYS 	= 0440;
//
// Function Prototypes:
//
class DatabaseObject : public sharedMemoryObject
{
public:
	 	DatabaseObject(key_t key, int size, int mode);	// constructor
	       ~DatabaseObject();				// destructor
	       
	void	setnewsta(int ipn, int value){pDB[ipn].newsta = value;};
	void	setpid(int ipn, char * inPid)
		{
		   strncpy(pDB[ipn].pid, inPid, LENGTH_PID);
		   pDB[ipn].pid[LENGTH_PID] = '\0';
		};
	void	setiPicassoId(int ipn, int Id){pDB[ipn].iPicassoId = Id;};
	void	setdesc(int ipn, char * descr)
		{
		   strncpy(pDB[ipn].desc, descr, LENGTH_DESC);
		   pDB[ipn].desc[LENGTH_DESC] = '\0';
		}
	void	settagid(int ipn, char * tagId)
		{
		   strncpy(pDB[ipn].tagid, tagId, LENGTH_TAGID-1);
		   pDB[ipn].tagid[LENGTH_TAGID-1] = '\0';
		}
	void	setpipbitmap(int ipn, int i){pDB[ipn].pipbitmap = i;};
	void	setpisabitmap(int ipn, int i){pDB[ipn].pisabitmap = i;};
	void	setsig_type(int ipn, int i){pDB[ipn].sig_type = i;};
	void	setmessage1(int ipn, char * m)
		{
		   strncpy(pDB[ipn].message1, m, LENGTH_MESS);
		   pDB[ipn].message1[LENGTH_MESS] = '\0';
		};
	void	setmessage2(int ipn, char * m)
		{
		   strncpy(pDB[ipn].message2, m, LENGTH_MESS);
		   pDB[ipn].message2[LENGTH_MESS] = '\0';
		};
	void	seteng_units(int ipn, char * u)
		{
		   strncpy(pDB[ipn].eng_units, u, LENGTH_UNIT);
		   pDB[ipn].eng_units[LENGTH_UNIT] = '\0';
		};
	void	setmet_units(int ipn, char * u)
		{
		   strncpy(pDB[ipn].met_units, u, LENGTH_UNIT);
		   pDB[ipn].met_units[LENGTH_UNIT] = '\0';
		};
	void	setdisfmt(int ipn, int disfmt);

	void	seterange_lo(int ipn, float r){pDB[ipn].erange_lo = r;};
	void	seterange_hi(int ipn, float r){pDB[ipn].erange_hi = r;};
	void	setmfactor(int ipn, float f){pDB[ipn].mfactor = f;};
	void	setmoffset(int ipn, float f){pDB[ipn].moffset = f;};
	void	sethhval(int ipn, float v){pDB[ipn].hhval = v;};
	void	sethval(int ipn, float v){pDB[ipn].hval = v;};
	void	setlval(int ipn, float v){pDB[ipn].lval = v;};
	void	setllval(int ipn, float v){pDB[ipn].llval = v;};
	void	sethhmet(int ipn, float v){pDB[ipn].hhmet = v;};
	void	sethmet(int ipn, float v){pDB[ipn].hmet = v;};
	void	setlmet(int ipn, float v){pDB[ipn].lmet = v;};
	void	setllmet(int ipn, float v){pDB[ipn].llmet = v;};
	void	setlIPN(int ipn, short lIpn){pDB[ipn].lIPN = lIpn;};
	void	sethIPN(int ipn, short hIpn){pDB[ipn].hIPN = hIpn;};
	void	setvalue(int ipn, float v){pDB[ipn].value = v;};
	void	setDBrev(int r){*databaseRevision = r;};
	

	int	getnewsta(int ipn){return(pDB[ipn].newsta);};
	char *	getpid(int ipn){return(pDB[ipn].pid);};
	int	getiPicassoId(int ipn){return(pDB[ipn].iPicassoId);};
	int	getsig_type(int ipn){return(pDB[ipn].sig_type);};
	float   getvalue(int ipn){return(pDB[ipn].value);};
	short	getlIPN(int ipn){return pDB[ipn].lIPN;};
	short	gethIPN(int ipn){return pDB[ipn].hIPN;};
	int	getDBrev(){return *databaseRevision;};

  	void *	getDBrecptr(int ipn){return(&(pDB[ipn]));};
	
	int	isanalog(int ipn){return(getsig_type(ipn) < MINDIGTYPE);};
	void	writeDatabase();

private:
	void		setpDBrev(void * a){databaseRevision = (int *)a;};
	void		readDatabase();
	RECORD *	pDB;
	int *		databaseRevision;

};
#endif
