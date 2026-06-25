#ifndef _PID_LIST_ITEM_OBJECT
#define _PID_LIST_ITEM_OBJECT
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// PidListItemObject.h  
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This object contains defines the PID List Item 
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files
//
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
//
// Local Structure Definitions

class PidListItemObject
{
    public:
	PidListItemObject(const char * Pid, const int Ipn, 
			  const int PicassoId);
	~PidListItemObject();

	void 	setNext(PidListItemObject * p){Next = p;};

	char *			getPidName(){return PidName;};
	int			getIpn(){return Ipn;};
	int			getPicassoId(){return PicassoId;};
	PidListItemObject * 	getNext(){return Next;};

    private:
	void 	setPidName(const char * n)
	{
	    strncpy(PidName, n, sizeof(PidName)-1);
	};
	void 	setIpn(int i){Ipn = i;};
	void	setPicassoId(const int id){PicassoId = id;};


    	char			PidName[12];	
	int			Ipn;
	int			PicassoId;
    	PidListItemObject *	Next;	// pointer to next PicListItem
};
#endif

