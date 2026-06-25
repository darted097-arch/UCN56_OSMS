#ifndef _COLSS_PARAMETERS_H
#define _COLSS_PARAMETERS_H
//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// colssParameters.h    
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//
// **************************************************************************************
//  Definitions for all COLSS messageParameters
// ***************************************************************************	      		
//
// ***************************************************************************	      		
//
//
const int	  REQUEST_COLSS_CURRENT_MODE		 =   0x0782;      	      
const int	  CONFIRM_COLSS_CURRENT_MODE		 =   0x0783;      	      
const int	  REQUEST_COLSS_MODE_CHANGE		 =   0x0784;      		      
const int	  CONFIRM_COLSS_MODE_CHANGE		 =   0x0785;      		      
const int	  REQUEST_COLSS_INITIALIZATION		 =   0x0786;      		      
const int	  CONFIRM_COLSS_INITIALIZATION		 =   0x0787;      		      
const int	  REQUEST_COLSS_REPORTS			 =   0x0788;      		      
const int	  CONFIRM_COLSS_REPORTS			 =   0x0789;      		      
const int	  REQUEST_COLSS_SURV_TEST		 =   0x078A;      		      
const int	  CONFIRM_COLSS_SURV_TEST		 =   0x078B;      		          		      
const int	  REQUEST_COLSS_POWER_DP		 =   0x078C;      		      
const int	  CONFIRM_COLSS_POWER_DP		 =   0x078D;      		      
const int	  REQUEST_COLSS_CPU_COMPARISON		 =   0x078E;       		      
const int	  CONFIRM_COLSS_CPU_COMPARISON		 =   0x078F;      	      
const int	  REQUEST_COLSS_FAILED_SENSOR 		 =   0x0790;      	      
const int	  CONFIRM_COLSS_FAILED_SENSOR 		 =   0x0791;
const int	  REQUEST_COLSS_DETAILED_PRINT		 =   0x0792;
const int	  CONFIRM_COLSS_DETAILED_PRINT  	 =   0x0793;  
const int	  REQUEST_COLSS_TEST_CASE		 =   0x0794;         	      
const int	  CONFIRM_COLSS_TEST_CASE		 =   0x0795;   
const int 	  REQUEST_COLSS_DISP_CHANGE		 =   0x0796;       	      
const int 	  CONFIRM_COLSS_DISP_CHANGE		 =   0x0797;    
const int 	  REQUEST_COLSS_CPUC_PRINT		 =   0x0798;       	      
const int 	  CONFIRM_COLSS_CPUC_PRINT		 =   0x0799; 
//
// ***************************************************************************
//	
// The following are not opcodes and therefore do not need to be unique
// vis-a-vis the opcodes.
//
const int 	  COLSS_INIT_MODE			=	0x0000;
const int 	  COLSS_SCHEDULED_MODE			=	0x0001;
const int 	  COLSS_UNSCHEDULED_MODE		=	0x0002;
const int 	  COLSS_TEST_MODE			=	0x0003;

#endif 
