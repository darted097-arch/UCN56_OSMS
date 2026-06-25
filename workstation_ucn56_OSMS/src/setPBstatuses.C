//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// sendPBstatuses.C     
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
// Determines the Primary and Backup statuses of the two CommLinks from 
// shared memory input.  Sets the shared memory Primary and backup statuses 
// of the two CommLinks in shared memory.
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
#include "InterProcSMObject.h"
#include "interface.h"
//
// Global Variables:
//
extern INTERFACE * Interface;
//
// Local Constant Definitions:
//
//
// Function Prototypes:
//
#include "prototypes.h"
//
// ***************************************************************************
//
// for comments: C = CPU, 0 = unconnected, M = master, S = slave, 
// U = connected but undetermined M/S, ? = invalid or questionable value.
//
static int PBstatus[64][3] = {
//  INDEX            CPU 1                CPU 2
    0x0000,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=0
    0x0001,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=U
    0x0002,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=?
    0x0003,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=0,C2=M
    0x0004,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=?
    0x0005,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=S
    0x0006,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=?
    0x0007,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=0,C2=?
    0x0008,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=0
    0x0009,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=U
    0x000A,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=?
    0x000B,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=U,C2=M
    0x000C,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=?
    0x000D,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=S
    0x000E,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=?
    0x000F,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=U,C2=?
    0x0010,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=0
    0x0011,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=U
    0x0012,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0013,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=?,C2=M
    0x0014,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0015,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=S
    0x0016,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0017,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0018,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=0
    0x0019,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=U
    0x001A,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=?
    0x001B,     PRIMARY_COMMLINK,    BACKUP_COMMLINK, //C1=M,C2=M
    0x001C,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=?
    0x001D,     PRIMARY_COMMLINK,    BACKUP_COMMLINK, //C1=M,C2=S
    0x001E,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=?
    0x001F,     PRIMARY_COMMLINK,    UNKNOWN_COMMLINK,//C1=M,C2=?
    0x0020,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=0
    0x0021,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=U
    0x0022,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0023,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=?,C2=M
    0x0024,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0025,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=S
    0x0026,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0027,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0028,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=0
    0x0029,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=U
    0x002A,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=?
    0x002B,     BACKUP_COMMLINK,     PRIMARY_COMMLINK,//C1=S,C2=M
    0x002C,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=?
    0x002D,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=S
    0x002E,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=?
    0x002F,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=S,C2=?
    0x0030,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=0
    0x0031,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=U
    0x0032,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0033,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=?,C2=M
    0x0034,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0035,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=S
    0x0036,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0037,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x0038,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=0
    0x0039,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=U
    0x003A,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x003B,     UNKNOWN_COMMLINK,    PRIMARY_COMMLINK,//C1=?,C2=M
    0x003C,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x003D,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=S
    0x003E,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK,//C1=?,C2=?
    0x003F,     UNKNOWN_COMMLINK,    UNKNOWN_COMMLINK //C1=?,C2=?
};
// Index to above table based upon the following.
//   05   |   04   |       03       |   02   |   01   |       00
// CPU1=S | CPU1=M | CPU1=connected | CPU2=S | CPU2=M | CPU2=connected 
//
const int       CPU2_CONNECTED_BIT      = 0x0001;
const int       CPU2_MASTER_BIT         = 0x0002;
const int	CPU2_SLAVE_BIT		= 0x0004;
const int       CPU1_CONNECTED_BIT      = 0x0008;
const int       CPU1_MASTER_BIT         = 0x0010;
const int	CPU1_SLAVE_BIT		= 0x0020;

static int	prevIndex = 0;
// Determines the Primary and Backup CommLinks 
void setPBstatuses()
{
   extern InterProcSMObject     IPO;

   int  index;

   index = 0;
   if (FALSE == IPO.getprimaryLocked())
   {
#ifdef DEBUG
#endif
      if (1 == IPO.getconnected(1)) index |= CPU1_CONNECTED_BIT;
      if (1 == IPO.getconnected(2)) index |= CPU2_CONNECTED_BIT;

      if (1 == IPO.getmasterStatus(1)) index |= CPU1_MASTER_BIT;
      if (255 == IPO.getmasterStatus(1)) index |= CPU1_SLAVE_BIT;

      if (1 == IPO.getmasterStatus(2)) index |= CPU2_MASTER_BIT;
      if (255 == IPO.getmasterStatus(2)) index |= CPU2_SLAVE_BIT;

      IPO.setprimaryStatus(1,PBstatus[index][1]);
      IPO.setprimaryStatus(2,PBstatus[index][2]);
   }

   if (PRIMARY_COMMLINK == IPO.getprimaryStatus(1)  &&
       TRUE == IPO.getconnected(1) )
   {
      Interface->System.wsState |= PB_DETERMINED_WS_STATE;
   }
   else
   {
      if (PRIMARY_COMMLINK == IPO.getprimaryStatus(2)  &&
          TRUE == IPO.getconnected(2) )
      {
         Interface->System.wsState |= PB_DETERMINED_WS_STATE;
      }
      else
      {
         Interface->System.wsState &= ~(PB_DETERMINED_WS_STATE);
      }
   }
/*--------------------------------
//----------------- CORENET 2013 --------------
	cout << "*********************************************************************************" << endl;
	cout << "IPO.getconnected(1) " << IPO.getconnected(1) << endl;
	cout << "IPO.getconnected(2) " << IPO.getconnected(2) << endl;
	cout << "IPO.getmasterStatus(1) " << IPO.getmasterStatus(1) << endl;
	cout << "IPO.getmasterStatus(2) " << IPO.getmasterStatus(2) << endl;
	cout << "IPO.getprimaryStatus(1) " << IPO.getprimaryStatus(1) << endl;
	cout << "IPO.getprimaryStatus(2) " << IPO.getprimaryStatus(2) << endl;

	if (1 == IPO.getconnected(1) && PRIMARY_COMMLINK == IPO.getprimaryStatus(1)) {
      		index |= CPU1_CONNECTED_BIT;
      		index |= CPU1_MASTER_BIT;
      		IPO.setprimaryStatus(1,PBstatus[index][1]);
      		Interface->System.wsState |= PB_DETERMINED_WS_STATE;
	cout << "************** HOST1 connected !!!!!!!!!!!!!!!!!!!!!" << endl;
	}
	else if (1 == IPO.getconnected(2) && PRIMARY_COMMLINK == IPO.getprimaryStatus(2)) {
      		index |= CPU2_CONNECTED_BIT;
      		index |= CPU2_MASTER_BIT;
      		IPO.setprimaryStatus(2,PBstatus[index][2]);
      		Interface->System.wsState |= PB_DETERMINED_WS_STATE;
	cout << "************** HOST2 connected !!!!!!!!!!!!!!!!!!!!!" << endl;
	}
	cout << "*********************************************************************************" << endl;
//----------------- CORENET 2013 --------------
--------------------------*/

   if (index != prevIndex)
   {
      cout << "***** setPBstatuses(): masterStatus for CommLink 1, 2 = " 
	   << dec << IPO.getmasterStatus(1) << " " << IPO.getmasterStatus(2)
	   << endl;
      cout << " setPBstatuses(): For index = " << hex << index 
           << " CommLink1= " << dec << IPO.getprimaryStatus(1)
	   << " CommLink2 = " << IPO.getprimaryStatus(2) << endl
	   << "   PrimaryLocked = " << IPO.getprimaryLocked() << endl;
   }
   prevIndex = index;
}


