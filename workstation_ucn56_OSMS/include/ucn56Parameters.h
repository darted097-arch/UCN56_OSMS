
#ifndef _UCN56_PARAMS_H
#define _UCN56_PARAMS_H

typedef struct {
// Static Information for Database Points
// int           iPicassoId;          //The Id in the GUI
   char          Ftnpid[8];           //The point id (No pad)
   char          pid[12];             //The point id 8 bytes+pad
   char          desc[36];            //English Description 32 bytes+pad
   char          tagid[12];           //Tag id from DB
   char          dispid[12];          //Tag Id to display
   int           sig_type;            //Point Type

// keep pipbitmap so that the host can run the Picasso displays autonomously
   int           pipbitmap;           //Static logical flags (for xfer)

   char          alstat;              //Digital alarm status
   char          copos2;              //High cutout flag
   char          copos1;              //Low/digital cutout flag
   char          metalm;              //Metric alarm units flag

   char          dlim;                //Drift limit exists

///////////Add For dydata
   char          llim;                //Low alarm limit exists
   char          hlim;                //High alarm limit exists
   char          lllim;               //LoLo  alarm limit exists
   char          hhlim;               //HiHI alarm limit exists
/////////// End

   char          metunit;             //Metric units flag
   char          chgprot;             //Protected from DB changes
   char          pidlvl1;             //Level 1 point id

   char          pipuse;              //Used by PIP
   char          eunit;               //Eng Units from Datalink
   short         dsllvl;              //Dynamic sector list level

   char          recordExist;         //pointid in record exits
   char          PIPSPARE2;           //alignment
   char          PIPSPARE3;           //alignment
   char          PIPSPARE4;           //alignment

   // Digital Only Only Fields:
   char          message1[12];        //Open Contact
   char          message2[12];        //Closed Contact

   // Analog Only Fields:
   char          disfmt[8];           //Display format in C/C++

   char          eng_units[16];       //English Units
   char          met_units[16];       //Metric Units

   // Engineering Range
   float         erange_lo;           //Low engineering range
   float         erange_hi;           //High engineering range
   float         adfactor;
   float         adoffset;

   // Decimal Alarm Information
   float         hhval;               //HiHi alarm limit
   float         hval;                //High alarm limit
   float         lval;                //Low  alarm limit
   float         llval;               //LoLo alarm limit

   // Metric Alarm Information
   float         hhmet;               //HiHi alarm limit
   float         hmet;                //High alarm limit
   float         lmet;                //Low  alarm limit
   float         llmet;               //LoLo alarm limit

   // Engineering Units Conversion
   float         mfactor;
   float         moffset;

   // Polynomial Constants
   float         acon;                //A constant
   float         bcon;                //B constant

   // Polynomial Coefficients
   float         coeff0;              //Coefficient 0
   float         coeff1;              //Coefficient 1
   float         coeff2;              //Coefficient 2
   float         coeff3;              //Coefficient 3
   float         coeff4;              //Coefficient 4
   float         coeff5;              //Coefficient 5

   // Decimal deadband values
   float         hidbval;             //High decimal deadband
   float         lodbval;             //Low decimal deadband
   // Metric Deadband Values
   float         hidbmet;             //High metric deadband
   float         lodbmet;             //Low metric deadband

   float         alrdl;               //Analog link values for non-BO

   short         ctout1;              //Low/Digital Cutout IPN
   short         ctout2;              //High Cutout IPN
   short         convno;              //Conversion type index
   short         icl;                 //Incremental change limit
   short         degree;              //Degree of Polynomial
   short         PIPSPARE5;           //alignment
   float         drftlm;              //Drift limit
   float         hislim;              //High sensor limit
   float         loslim;              //Low sensor limit 
   short         hivsp;               //Variable hi alarm point index
   short         lovsp;               //Variable lo alarm point index
   short         almdly;              //Alarm delay
   short         stat1;               //Digital Status pairs
   short         altype;              //Digital Alarm type
   short         chass;               //I/O chassis
   short         kirtp;               //Channel/Card/Chassis(In Packed Data)
   short         rangeindex;          //index to the selected pair, for use by RTP driver

   int           senunit;             //Sensor Units for inputs
   float         resistor;            //Resistor value for milli-amp input
   float         attenuator;          //Attenuation factor for input

   //These variables were added to support the new PDAS and BOP Datalink design.
   //Review existing variables and delete unused vars
   float         hi_OOR;              //high out of range 
   float         lo_OOR;              //low out of range
   char          cabinet[4];          //A,B,C,D,N1,N2,N3 + null
   char          datalinkId[12];      //CPC,CEDMCS,ICCM, etc
   char          datalinkChannel[4];  //ICCMS datalink channel A|B
   char          sysAcr[8];           //ICCMS, etc
   char          bopPointName[16];    //bop DAS point name
   char          bopControllerName[4];//bop DAS controller name
   int           bufferIndex;         //for serial datalink buffers
   short         pdasIndex;           //index for local pdas node
   short         iccmsIndex;          //index for local iccms node
   short         chassisAddress;      //converted to hex
   short         cardAddress;         //0-15
   short         channelNumber;       //0-15

   // Common Dynamic Variables
   // keep pisabitmap so that the host can run the Picasso displays autonomously
   short        pisabitmap;          //Dynamic logical flags (for xfer)

   char         oscan;               //Out of scan flag
   char         ochek;               //Out of alarm check flag

   char         enval;               //Entered value flag
   char         unreli;              //Unreliable flag
	       
   char         ques;                //Questionable flag
   char         alscan;              //Alarmed last scan 
   char         flscan;              //Failed last scan 
   char         blscan;              //Bad last scan
			      
   char         pchnge;              //Point Changed (Remove? Cfm)
   char         oldoscan;            //Old out-of-scan   flag
   char         oldochek;            //Old out-of-check  flag
   char         oldenval;            //Old entered-value flag

   char         badata;              //point is bad
   char         dlConnected;         //data link connected
   char         PISASPARE2;          //alignment
   char         PISASPARE3;          //alignment
	       
   short        PISASPARE4;          //alignment
   short        atdptr;              //Next PID on Alarm Time Delay thread
   int          atdlay;              //Alarm time delay (seconds)

   int          oldsta;              //Old alarm state
   int          newsta;              //New alarm state

   // Initial Values and State
   float        initval;             //Initial value (decimal)
   int          initstat;            //Initial status (digital)
   float        initmet;             //Initial value (metric)
	       
   // Current analog/digital value
   float        oldval;              //Old English value
   int          oldstt;              //Old digital state

   float        curval;              //Current analog value
   int          curstt;              //Current digital state

   float        oldmet;              //Old Metric value
   float        curmet;              //Current Metric value
				       
   float        cursen;              //Current Sensor value

} U56_RECORD;

#endif


