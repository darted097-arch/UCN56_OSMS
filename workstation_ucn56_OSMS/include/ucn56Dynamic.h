
#ifndef _UCN56_DYNAMIC_H
#define _UCN56_DYNAMIC_H

typedef struct  {
   short        pisabitmap;          //Dynamic logical flags (for xfer)
   char         badata;              //point is bad
   char         dlConnected;         //data link connected

   char         oscan;               //Out of scan flag
   char         ochek;               //Out of alarm check flag
   char         enval;               //Entered value flag
   char         unreli;              //Unreliable flag

   char         pchnge;              //Point Changed (Remove? Cfm)
   char         oldoscan;            //Old out-of-scan   flag
   char         oldochek;            //Old out-of-check  flag
   char         oldenval;            //Old entered-value flag

   char         llim;                //Low alarm limit exists
   char         hlim;                //High alarm limit exists
   char         lllim;               //LoLo  alarm limit exists
   char         hhlim;               //HiHI alarm limit exists

   int          oldsta;              //Old alarm state
   int          newsta;              //New alarm state

   float        oldval;              //Old English value
   int          oldstt;              //Old digital state
   float        curval;              //Current analog value
   int          curstt;              //Current digital state
   float        oldmet;              //Old Metric value
   float        curmet;              //Current Metric value
   float        cursen;              //Current Sensor value

} DYNAMIC_RECORD;

#endif


