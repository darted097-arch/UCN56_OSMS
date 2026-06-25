#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

struct dataTestHeader
{
    char szTT[2];
    int nTotCnt;
    int nCurCnt;
};

struct dataTestBody
{
    struct dataTestHeader    header;
    char                     char_buf[4096]; 
};

int main(int argc, char **argv)
{
    int  sockfd;
    int  clilen;
    int  state;

    struct sockaddr_in serveraddr;

    clilen = sizeof(serveraddr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0)
    {
	perror("Socket error : ");
	exit(0);
    }
    //bzero(&serveraddr, sizeof(serveraddr));
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(3333);

    struct dataTestBody db;
    memset(db.char_buf, 0x00, 4096);
    
    char szTest[24] = {0,};
    sprintf(szTest, "%s", "PARK JI HOON UDP TEST" );      

    sendto(sockfd, (void *)&szTest, sizeof(szTest), 0, (struct sockaddr *)&serveraddr, clilen);

/*
    for(int i=0; i<27; i++)
    {
        sprintf( db.header.szTT, "%s", "TT" );  
        db.header.nTotCnt = 27;
	db.header.nCurCnt = i;
    
        if(i%2==0) // Analog
	{


	}
	else // Digital
	{



	}
    }
*/
    /*
    sendto(sockfd, (void *)&add_data, sizeof(add_data), 0, (struct sockaddr *)&serveraddr, clilen);
    sleep(100);

    sendto(sockfd, (void *)&add_data, sizeof(add_data), 0, (struct sockaddr *)&serveraddr, clilen);
    sleep(100);
    
    sendto(sockfd, (void *)&add_data, sizeof(add_data), 0, (struct sockaddr *)&serveraddr, clilen);
    sleep(100);
    */
    
    close(sockfd);

    return 0;
}
