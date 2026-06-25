//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// parseColssMessages.C 
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// ***************************************************************************
//  This module processes the CSL messages.  
//  This includes only messages from the host to the workstation.
// ***************************************************************************
//
// ***************************************************************************
//
// System Include Files:
#include <iostream.h>
#include <fstream.h>
#include <unistd.h>
#include <stdio.h>

#ifdef LINUX_APP
#include <netdb.h>      // CORENET 2012.07.09
#include <netinet/in.h> // CORENET 2012.07.09
#endif

//
// User Defined Include Files:
#include "systemParameters.h"		// generic system parameters
#include "databaseParameters.h"		// database parameters
#include "messageParameters.h"		// message parameters
#include "messages.h"			// message variables
#include "interface.h"			// interface variables
#include "messageQueueObject.h"		//
#include "messageObject.h"
//
// External Variables:
extern	INTERFACE	*Interface;	// shm/CCUR to GUI interface variables
extern	messageQueueObject	IGUI_MsgQue;	// CommLink to InterfaceToGUI message queue
//
// Local Constant Definitions: NONE
//
// Function Prototypes: NONE
//      
//
// ***************************************************************************
//
void parseCslMessages(messageObject & msg)
{
//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
   WSrequestMessageData *grm_data_ptr; 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   // >>>>>>>>>>>>>>>> Process Message According to Opcode <<<<<<<<<<<<<<<<<<<<<<<<<
   switch (msg.getOpcode())
   {

    case REQUEST_CSL_DISPLAY:

	 // put the Interface data into the message
         msg.setData(&(Interface->GenericRequestMessage.data), 
	     (sizeof(Interface->GenericRequestMessage.data.requestOption) +
	      sizeof(Interface->GenericRequestMessage.data.theWorkstation)) );
         cout << hex << msg.getOpcode() << endl;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grm_data_ptr = (WSrequestMessageData *)msg.getDataPtr(); 
         grm_data_ptr->requestOption = htonl(grm_data_ptr->requestOption); 
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;


    case EXIT_CSL_DISPLAY:

	 // put the Interface data into the message
	 msg.setData(&(Interface->GenericRequestMessage.data), 
	     (sizeof(Interface->GenericRequestMessage.data.requestOption) +
	      sizeof(Interface->GenericRequestMessage.data.theWorkstation)) );
         cout << hex << msg.getOpcode() << endl;

//////////////////////////////////////////////////////////////
// CORENET 2012.07.09
//////////////////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LINUX_APP
         grm_data_ptr = (WSrequestMessageData *)msg.getDataPtr(); // CORENET
         grm_data_ptr->requestOption = htonl(grm_data_ptr->requestOption); // CORENET
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	 break;


   case SEND_CSL_DISPLAY:
#ifdef DEBUG
         cout <<"csllog task has started sending 33 messages!"<<endl;
#endif
         // get the data from the message into the shared memory area
         msg.getData(&(Interface->CSL_Template.Csl_cache), sizeof(Interface->CSL_Template.Csl_cache));
/*0000000000000000000 deleted 2013.09 CORENET
         strncpy(Interface->CSL_Template.Csl_datim.Datim1,Interface->CSL_Template.Csl_cache.Cache1,18);
         Interface->CSL_Template.Csl_datim.Datim1[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid1,Interface->CSL_Template.Csl_cache.Cache1+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid1[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg1,Interface->CSL_Template.Csl_cache.Cache1+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg1[63]='\0';
//         
         strncpy(Interface->CSL_Template.Csl_datim.Datim2,Interface->CSL_Template.Csl_cache.Cache2,18);
         Interface->CSL_Template.Csl_datim.Datim2[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid2,Interface->CSL_Template.Csl_cache.Cache2+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid2[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg2,Interface->CSL_Template.Csl_cache.Cache2+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg2[63]='\0';
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim3,Interface->CSL_Template.Csl_cache.Cache3,18);
         Interface->CSL_Template.Csl_datim.Datim3[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid3,Interface->CSL_Template.Csl_cache.Cache3+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid3[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg3,Interface->CSL_Template.Csl_cache.Cache3+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg3[63]='\0';         
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim4,Interface->CSL_Template.Csl_cache.Cache4,18);
         Interface->CSL_Template.Csl_datim.Datim4[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid4,Interface->CSL_Template.Csl_cache.Cache4+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid4[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg4,Interface->CSL_Template.Csl_cache.Cache4+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg4[63]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim5,Interface->CSL_Template.Csl_cache.Cache5,18);
         Interface->CSL_Template.Csl_datim.Datim5[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid5,Interface->CSL_Template.Csl_cache.Cache5+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid5[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg5,Interface->CSL_Template.Csl_cache.Cache5+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg5[63]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim6,Interface->CSL_Template.Csl_cache.Cache6,18);
         Interface->CSL_Template.Csl_datim.Datim6[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid6,Interface->CSL_Template.Csl_cache.Cache6+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid6[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg6,Interface->CSL_Template.Csl_cache.Cache6+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg6[63]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim7,Interface->CSL_Template.Csl_cache.Cache7,18);
         Interface->CSL_Template.Csl_datim.Datim7[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid7,Interface->CSL_Template.Csl_cache.Cache7+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid7[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg7,Interface->CSL_Template.Csl_cache.Cache7+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg7[63]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim8,Interface->CSL_Template.Csl_cache.Cache8,18);
         Interface->CSL_Template.Csl_datim.Datim8[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid8,Interface->CSL_Template.Csl_cache.Cache8+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid8[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg8,Interface->CSL_Template.Csl_cache.Cache8+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg8[63]='\0';            
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim9,Interface->CSL_Template.Csl_cache.Cache9,18);
         Interface->CSL_Template.Csl_datim.Datim9[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid9,Interface->CSL_Template.Csl_cache.Cache9+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid9[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg9,Interface->CSL_Template.Csl_cache.Cache9+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg9[63]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim10,Interface->CSL_Template.Csl_cache.Cache10,18);
         Interface->CSL_Template.Csl_datim.Datim10[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid10,Interface->CSL_Template.Csl_cache.Cache10+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid10[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg10,Interface->CSL_Template.Csl_cache.Cache10+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg10[63]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim11,Interface->CSL_Template.Csl_cache.Cache11,18);
         Interface->CSL_Template.Csl_datim.Datim11[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid11,Interface->CSL_Template.Csl_cache.Cache11+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid11[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg11,Interface->CSL_Template.Csl_cache.Cache11+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg11[63]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim12,Interface->CSL_Template.Csl_cache.Cache12,18);
         Interface->CSL_Template.Csl_datim.Datim12[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid12,Interface->CSL_Template.Csl_cache.Cache12+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid12[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg12,Interface->CSL_Template.Csl_cache.Cache12+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg12[63]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim13,Interface->CSL_Template.Csl_cache.Cache13,18);
         Interface->CSL_Template.Csl_datim.Datim13[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid13,Interface->CSL_Template.Csl_cache.Cache13+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid13[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg13,Interface->CSL_Template.Csl_cache.Cache13+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg13[63]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim14,Interface->CSL_Template.Csl_cache.Cache14,18);
         Interface->CSL_Template.Csl_datim.Datim14[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid14,Interface->CSL_Template.Csl_cache.Cache14+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid14[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg14,Interface->CSL_Template.Csl_cache.Cache14+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg14[63]='\0';   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim15,Interface->CSL_Template.Csl_cache.Cache15,18);
         Interface->CSL_Template.Csl_datim.Datim15[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid15,Interface->CSL_Template.Csl_cache.Cache15+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid15[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg15,Interface->CSL_Template.Csl_cache.Cache15+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg15[63]='\0';            
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim16,Interface->CSL_Template.Csl_cache.Cache16,18);
         Interface->CSL_Template.Csl_datim.Datim16[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid16,Interface->CSL_Template.Csl_cache.Cache16+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid16[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg16,Interface->CSL_Template.Csl_cache.Cache16+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg16[63]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim17,Interface->CSL_Template.Csl_cache.Cache17,18);
         Interface->CSL_Template.Csl_datim.Datim17[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid17,Interface->CSL_Template.Csl_cache.Cache17+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid17[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg17,Interface->CSL_Template.Csl_cache.Cache17+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg17[63]='\0';    
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim18,Interface->CSL_Template.Csl_cache.Cache18,18);
         Interface->CSL_Template.Csl_datim.Datim18[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid18,Interface->CSL_Template.Csl_cache.Cache18+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid18[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg18,Interface->CSL_Template.Csl_cache.Cache18+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg18[63]='\0';             
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim19,Interface->CSL_Template.Csl_cache.Cache19,18);
         Interface->CSL_Template.Csl_datim.Datim19[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid19,Interface->CSL_Template.Csl_cache.Cache19+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid19[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg19,Interface->CSL_Template.Csl_cache.Cache19+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg19[63]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim20,Interface->CSL_Template.Csl_cache.Cache20,18);
         Interface->CSL_Template.Csl_datim.Datim20[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid20,Interface->CSL_Template.Csl_cache.Cache20+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid20[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg20,Interface->CSL_Template.Csl_cache.Cache20+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg20[63]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim21,Interface->CSL_Template.Csl_cache.Cache21,18);
         Interface->CSL_Template.Csl_datim.Datim21[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid21,Interface->CSL_Template.Csl_cache.Cache21+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid21[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg21,Interface->CSL_Template.Csl_cache.Cache21+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg21[63]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim22,Interface->CSL_Template.Csl_cache.Cache22,18);
         Interface->CSL_Template.Csl_datim.Datim22[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid22,Interface->CSL_Template.Csl_cache.Cache22+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid22[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg22,Interface->CSL_Template.Csl_cache.Cache22+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg22[63]='\0';                                      
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim23,Interface->CSL_Template.Csl_cache.Cache23,18);
         Interface->CSL_Template.Csl_datim.Datim23[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid23,Interface->CSL_Template.Csl_cache.Cache23+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid23[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg23,Interface->CSL_Template.Csl_cache.Cache23+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg23[63]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim24,Interface->CSL_Template.Csl_cache.Cache24,18);
         Interface->CSL_Template.Csl_datim.Datim24[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid24,Interface->CSL_Template.Csl_cache.Cache24+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid24[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg24,Interface->CSL_Template.Csl_cache.Cache24+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg24[63]='\0';                                             
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim25,Interface->CSL_Template.Csl_cache.Cache25,18);
         Interface->CSL_Template.Csl_datim.Datim25[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid25,Interface->CSL_Template.Csl_cache.Cache25+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid25[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg25,Interface->CSL_Template.Csl_cache.Cache25+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg25[63]='\0';                                        
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim26,Interface->CSL_Template.Csl_cache.Cache26,18);
         Interface->CSL_Template.Csl_datim.Datim26[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid26,Interface->CSL_Template.Csl_cache.Cache26+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid26[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg26,Interface->CSL_Template.Csl_cache.Cache26+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg26[63]='\0';                                                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim27,Interface->CSL_Template.Csl_cache.Cache27,18);
         Interface->CSL_Template.Csl_datim.Datim27[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid27,Interface->CSL_Template.Csl_cache.Cache27+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid27[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg27,Interface->CSL_Template.Csl_cache.Cache27+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg27[63]='\0';                      
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim28,Interface->CSL_Template.Csl_cache.Cache28,18);
         Interface->CSL_Template.Csl_datim.Datim28[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid28,Interface->CSL_Template.Csl_cache.Cache28+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid28[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg28,Interface->CSL_Template.Csl_cache.Cache28+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg28[63]='\0';                     
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim29,Interface->CSL_Template.Csl_cache.Cache29,18);
         Interface->CSL_Template.Csl_datim.Datim29[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid29,Interface->CSL_Template.Csl_cache.Cache29+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid29[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg29,Interface->CSL_Template.Csl_cache.Cache29+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg29[63]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim30,Interface->CSL_Template.Csl_cache.Cache30,18);
         Interface->CSL_Template.Csl_datim.Datim30[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid30,Interface->CSL_Template.Csl_cache.Cache30+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid30[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg30,Interface->CSL_Template.Csl_cache.Cache30+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg30[63]='\0';                         
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim31,Interface->CSL_Template.Csl_cache.Cache31,18);
         Interface->CSL_Template.Csl_datim.Datim31[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid31,Interface->CSL_Template.Csl_cache.Cache31+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid31[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg31,Interface->CSL_Template.Csl_cache.Cache31+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg31[63]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim32,Interface->CSL_Template.Csl_cache.Cache32,18);
         Interface->CSL_Template.Csl_datim.Datim32[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid32,Interface->CSL_Template.Csl_cache.Cache32+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid32[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg32,Interface->CSL_Template.Csl_cache.Cache32+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg32[63]='\0';                          
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim33,Interface->CSL_Template.Csl_cache.Cache33,18);
         Interface->CSL_Template.Csl_datim.Datim33[18]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid33,Interface->CSL_Template.Csl_cache.Cache33+18,8);
         Interface->CSL_Template.Csl_tskid.Tskid33[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg33,Interface->CSL_Template.Csl_cache.Cache33+26,63);
         Interface->CSL_Template.Csl_msg.CslMsg33[63]='\0';                               
//        
0000000000000000000 deleted 2013.09 CORENET */
//0000000000000000000 added 2013.09 CORENET
         strncpy(Interface->CSL_Template.Csl_datim.Datim1,Interface->CSL_Template.Csl_cache.Cache1,20);
         Interface->CSL_Template.Csl_datim.Datim1[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid1,Interface->CSL_Template.Csl_cache.Cache1+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid1[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg1,Interface->CSL_Template.Csl_cache.Cache1+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg1[61]='\0';
//         
         strncpy(Interface->CSL_Template.Csl_datim.Datim2,Interface->CSL_Template.Csl_cache.Cache2,20);
         Interface->CSL_Template.Csl_datim.Datim2[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid2,Interface->CSL_Template.Csl_cache.Cache2+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid2[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg2,Interface->CSL_Template.Csl_cache.Cache2+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg2[61]='\0';
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim3,Interface->CSL_Template.Csl_cache.Cache3,20);
         Interface->CSL_Template.Csl_datim.Datim3[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid3,Interface->CSL_Template.Csl_cache.Cache3+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid3[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg3,Interface->CSL_Template.Csl_cache.Cache3+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg3[61]='\0';         
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim4,Interface->CSL_Template.Csl_cache.Cache4,20);
         Interface->CSL_Template.Csl_datim.Datim4[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid4,Interface->CSL_Template.Csl_cache.Cache4+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid4[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg4,Interface->CSL_Template.Csl_cache.Cache4+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg4[61]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim5,Interface->CSL_Template.Csl_cache.Cache5,20);
         Interface->CSL_Template.Csl_datim.Datim5[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid5,Interface->CSL_Template.Csl_cache.Cache5+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid5[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg5,Interface->CSL_Template.Csl_cache.Cache5+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg5[61]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim6,Interface->CSL_Template.Csl_cache.Cache6,20);
         Interface->CSL_Template.Csl_datim.Datim6[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid6,Interface->CSL_Template.Csl_cache.Cache6+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid6[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg6,Interface->CSL_Template.Csl_cache.Cache6+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg6[61]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim7,Interface->CSL_Template.Csl_cache.Cache7,20);
         Interface->CSL_Template.Csl_datim.Datim7[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid7,Interface->CSL_Template.Csl_cache.Cache7+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid7[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg7,Interface->CSL_Template.Csl_cache.Cache7+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg7[61]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim8,Interface->CSL_Template.Csl_cache.Cache8,20);
         Interface->CSL_Template.Csl_datim.Datim8[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid8,Interface->CSL_Template.Csl_cache.Cache8+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid8[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg8,Interface->CSL_Template.Csl_cache.Cache8+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg8[61]='\0';            
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim9,Interface->CSL_Template.Csl_cache.Cache9,20);
         Interface->CSL_Template.Csl_datim.Datim9[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid9,Interface->CSL_Template.Csl_cache.Cache9+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid9[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg9,Interface->CSL_Template.Csl_cache.Cache9+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg9[61]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim10,Interface->CSL_Template.Csl_cache.Cache10,20);
         Interface->CSL_Template.Csl_datim.Datim10[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid10,Interface->CSL_Template.Csl_cache.Cache10+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid10[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg10,Interface->CSL_Template.Csl_cache.Cache10+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg10[61]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim11,Interface->CSL_Template.Csl_cache.Cache11,20);
         Interface->CSL_Template.Csl_datim.Datim11[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid11,Interface->CSL_Template.Csl_cache.Cache11+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid11[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg11,Interface->CSL_Template.Csl_cache.Cache11+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg11[61]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim12,Interface->CSL_Template.Csl_cache.Cache12,20);
         Interface->CSL_Template.Csl_datim.Datim12[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid12,Interface->CSL_Template.Csl_cache.Cache12+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid12[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg12,Interface->CSL_Template.Csl_cache.Cache12+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg12[61]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim13,Interface->CSL_Template.Csl_cache.Cache13,20);
         Interface->CSL_Template.Csl_datim.Datim13[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid13,Interface->CSL_Template.Csl_cache.Cache13+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid13[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg13,Interface->CSL_Template.Csl_cache.Cache13+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg13[61]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim14,Interface->CSL_Template.Csl_cache.Cache14,20);
         Interface->CSL_Template.Csl_datim.Datim14[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid14,Interface->CSL_Template.Csl_cache.Cache14+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid14[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg14,Interface->CSL_Template.Csl_cache.Cache14+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg14[61]='\0';   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim15,Interface->CSL_Template.Csl_cache.Cache15,20);
         Interface->CSL_Template.Csl_datim.Datim15[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid15,Interface->CSL_Template.Csl_cache.Cache15+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid15[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg15,Interface->CSL_Template.Csl_cache.Cache15+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg15[61]='\0';            
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim16,Interface->CSL_Template.Csl_cache.Cache16,20);
         Interface->CSL_Template.Csl_datim.Datim16[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid16,Interface->CSL_Template.Csl_cache.Cache16+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid16[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg16,Interface->CSL_Template.Csl_cache.Cache16+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg16[61]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim17,Interface->CSL_Template.Csl_cache.Cache17,20);
         Interface->CSL_Template.Csl_datim.Datim17[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid17,Interface->CSL_Template.Csl_cache.Cache17+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid17[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg17,Interface->CSL_Template.Csl_cache.Cache17+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg17[61]='\0';    
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim18,Interface->CSL_Template.Csl_cache.Cache18,20);
         Interface->CSL_Template.Csl_datim.Datim18[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid18,Interface->CSL_Template.Csl_cache.Cache18+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid18[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg18,Interface->CSL_Template.Csl_cache.Cache18+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg18[61]='\0';             
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim19,Interface->CSL_Template.Csl_cache.Cache19,20);
         Interface->CSL_Template.Csl_datim.Datim19[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid19,Interface->CSL_Template.Csl_cache.Cache19+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid19[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg19,Interface->CSL_Template.Csl_cache.Cache19+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg19[61]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim20,Interface->CSL_Template.Csl_cache.Cache20,20);
         Interface->CSL_Template.Csl_datim.Datim20[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid20,Interface->CSL_Template.Csl_cache.Cache20+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid20[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg20,Interface->CSL_Template.Csl_cache.Cache20+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg20[61]='\0';                
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim21,Interface->CSL_Template.Csl_cache.Cache21,20);
         Interface->CSL_Template.Csl_datim.Datim21[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid21,Interface->CSL_Template.Csl_cache.Cache21+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid21[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg21,Interface->CSL_Template.Csl_cache.Cache21+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg21[61]='\0';                   
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim22,Interface->CSL_Template.Csl_cache.Cache22,20);
         Interface->CSL_Template.Csl_datim.Datim22[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid22,Interface->CSL_Template.Csl_cache.Cache22+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid22[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg22,Interface->CSL_Template.Csl_cache.Cache22+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg22[61]='\0';                                      
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim23,Interface->CSL_Template.Csl_cache.Cache23,20);
         Interface->CSL_Template.Csl_datim.Datim23[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid23,Interface->CSL_Template.Csl_cache.Cache23+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid23[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg23,Interface->CSL_Template.Csl_cache.Cache23+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg23[61]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim24,Interface->CSL_Template.Csl_cache.Cache24,20);
         Interface->CSL_Template.Csl_datim.Datim24[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid24,Interface->CSL_Template.Csl_cache.Cache24+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid24[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg24,Interface->CSL_Template.Csl_cache.Cache24+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg24[61]='\0';                                             
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim25,Interface->CSL_Template.Csl_cache.Cache25,20);
         Interface->CSL_Template.Csl_datim.Datim25[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid25,Interface->CSL_Template.Csl_cache.Cache25+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid25[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg25,Interface->CSL_Template.Csl_cache.Cache25+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg25[61]='\0';                                        
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim26,Interface->CSL_Template.Csl_cache.Cache26,20);
         Interface->CSL_Template.Csl_datim.Datim26[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid26,Interface->CSL_Template.Csl_cache.Cache26+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid26[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg26,Interface->CSL_Template.Csl_cache.Cache26+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg26[61]='\0';                                                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim27,Interface->CSL_Template.Csl_cache.Cache27,20);
         Interface->CSL_Template.Csl_datim.Datim27[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid27,Interface->CSL_Template.Csl_cache.Cache27+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid27[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg27,Interface->CSL_Template.Csl_cache.Cache27+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg27[61]='\0';                      
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim28,Interface->CSL_Template.Csl_cache.Cache28,20);
         Interface->CSL_Template.Csl_datim.Datim28[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid28,Interface->CSL_Template.Csl_cache.Cache28+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid28[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg28,Interface->CSL_Template.Csl_cache.Cache28+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg28[61]='\0';                     
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim29,Interface->CSL_Template.Csl_cache.Cache29,20);
         Interface->CSL_Template.Csl_datim.Datim29[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid29,Interface->CSL_Template.Csl_cache.Cache29+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid29[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg29,Interface->CSL_Template.Csl_cache.Cache29+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg29[61]='\0';                  
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim30,Interface->CSL_Template.Csl_cache.Cache30,20);
         Interface->CSL_Template.Csl_datim.Datim30[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid30,Interface->CSL_Template.Csl_cache.Cache30+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid30[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg30,Interface->CSL_Template.Csl_cache.Cache30+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg30[61]='\0';                         
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim31,Interface->CSL_Template.Csl_cache.Cache31,20);
         Interface->CSL_Template.Csl_datim.Datim31[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid31,Interface->CSL_Template.Csl_cache.Cache31+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid31[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg31,Interface->CSL_Template.Csl_cache.Cache31+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg31[61]='\0';                 
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim32,Interface->CSL_Template.Csl_cache.Cache32,20);
         Interface->CSL_Template.Csl_datim.Datim32[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid32,Interface->CSL_Template.Csl_cache.Cache32+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid32[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg32,Interface->CSL_Template.Csl_cache.Cache32+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg32[61]='\0';                          
//                  
         strncpy(Interface->CSL_Template.Csl_datim.Datim33,Interface->CSL_Template.Csl_cache.Cache33,20);
         Interface->CSL_Template.Csl_datim.Datim33[20]='\0';
         strncpy(Interface->CSL_Template.Csl_tskid.Tskid33,Interface->CSL_Template.Csl_cache.Cache33+20,8);
         Interface->CSL_Template.Csl_tskid.Tskid33[8]='\0';
         strncpy(Interface->CSL_Template.Csl_msg.CslMsg33,Interface->CSL_Template.Csl_cache.Cache33+28,61);
         Interface->CSL_Template.Csl_msg.CslMsg33[61]='\0';                               
//        
//0000000000000000000 added 2013.09 CORENET
//        
         IGUI_MsgQue.sendMessage(msg.pMsg, sizeof(WSmessageHeader), msg.getType());
         break;
    default:
        cout << "parseCslMessages: Received Unknown Message, opcode: "
                   << hex << msg.getOpcode() << endl;
   } // end of opcode switch statement
}
