//
// **********************************************
// HBNPP 3&4 PMS DISPLAY SYSTEM
// 
// FDDIparse.C          
// ver 1.0 -- pms01 2016.06.30.
// **********************************************
//

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "sharedMemoryObject.h"

//
// Global Variables:
//
INTERFACE	*Interface;		// shared memory to Picasso variables

// Create Global shared memory objects
sharedMemoryObject InterfaceSMO(INTERFACE_KEY,sizeof(INTERFACE),0);

int main()
{
   char c;
   int state = 1;
   int count = 0;
   int count1 = 0;
   int index;
   ssize_t ret_status;
   char in_buff[11];
   char test_str[11];
   char status_str[20];
   char escseq1[5];
   char escseq2[6];
   char escseq3[6];
   char write_buff[100000];

// map Interface shared memory
   Interface = (INTERFACE *) InterfaceSMO.getAddr();

// enable non-blocking I/O
   fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);

   sprintf(escseq1,"%cY!P",27);
   sprintf(escseq2,"%cY+78",27);
   sprintf(escseq3,"%cY,78",27);
   sprintf(write_buff,"Status: ");

   for (index=0; index<10; )
   {
      ret_status = read(STDIN_FILENO,&in_buff[index],1);
      if ((int)ret_status >= 0) index += 1;
   }
   in_buff[10]='\0';

   while (1)
   {
   ret_status = read(STDIN_FILENO,&c,1);
      
   if ((int)ret_status < 0)
   {
      if (count1 == 0) system("echo 1 >>fddi.in");
      count1 += 1;
      count1 %= 10;
      sleep(1);
   }
   else
   {
/*
      printf("state = %d, char = %d\n",state,in_buff[0]);
*/
      switch (state)
      {
         case 1:
            strcpy(test_str,in_buff);
            test_str[4] = 0;
            switch (test_str[0])
            {
               case 27:
               case ' ':
                    if ( strcmp(test_str,escseq1) == 0 )
                    {
                       count = 0;
                       state = 7;
                    }
                    break;

               default:
                    break;
            }
            break;

         case 2:
            strcpy(test_str,in_buff);
            switch (test_str[0])
            {

               case 27:
                    sprintf(write_buff,"%s           , Channel A: ",write_buff);
                    state = 3;  
                    break;
               default:
                    sprintf(write_buff,"%s%c",write_buff,test_str[0]);
                    break;
            }
            break;

         case 3:
            strcpy(test_str,in_buff);
            test_str[5] = 0;
            switch (test_str[0])
            {
               case 27:
                    if ( strcmp(test_str,escseq2) == 0 )
                    {
                       count = 0;
                       status_str[0]=0;
                       state = 8;
                    }
                    break;

               default:
                    break;
            }
            break;

         case 4:
            strcpy(test_str,in_buff);
            switch (test_str[0])
            {
               case 27:
                    status_str[9]=0;
                    if ( strcmp(status_str,"   ACTIVE") == 0)
                    {
                       Interface->System.FDDIChanA = 1;
                    }
                    else
                    {
                       Interface->System.FDDIChanA = 0;
                    }
                    sprintf(write_buff,"%s , Channel B: ",write_buff);
                    state = 5;  
                    break;
               default:
                    sprintf(status_str,"%s%c",status_str,test_str[0]);
                    sprintf(write_buff,"%s%c",write_buff,test_str[0]);
                    break;
            }
            break;

         case 5:
            strcpy(test_str,in_buff);
            test_str[5] = 0;
            switch (test_str[0])
            {
               case 27:
                    if ( strcmp(test_str,escseq3) == 0 )
                    {
                       count = 0;
                       status_str[0]=0;
                       state = 9;
                    }
                    break;

               default:
                    break;
            }
            break;

         case 6:
            strcpy(test_str,in_buff);
            switch (test_str[0])
            {
               case 27:
                    status_str[9]=0;
                    if ( strcmp(status_str,"   ACTIVE") == 0)
                    {
                       Interface->System.FDDIChanB = 1;
                    }
                    else
                    {
                       Interface->System.FDDIChanB = 0;
                    }
                    printf("%s\n",write_buff);
                    fflush(stdout);
                    sprintf(write_buff,"Status: ");
                    state = 1;
                    break;
               default:
                    sprintf(status_str,"%s%c",status_str,test_str[0]);
                    sprintf(write_buff,"%s%c",write_buff,test_str[0]);
                    break;
            }
            break;

         case 7:
            if (count < 2)
            {
               count += 1;
            }
            else
            {
               state = 2;
            }
            break;

         case 8:
            if (count < 3)
            {
               count += 1;
            }
            else
            {
               state = 4;
            }
            break;

         case 9:
            if (count < 3)
            {
               count += 1;
            }
            else
            {
               state = 6;
            }
            break;

      }
      for (index=0; index<9; index++)
      {
          in_buff[index]=in_buff[index+1];
      }
      in_buff[9]=c;
      in_buff[10]='\0';
   }
   }
   fflush(stdout);
}
