/*****************************************************************************
​ ​*​ ​Copyright​ ​(C)​ ​2018 ​by​ Zach Farmer
​ ​*
​ ​*​ ​Redistribution,​ ​modification​ ​or​ ​use​ ​of​ ​this​ ​software​ ​in​ ​source​ ​or​ ​binary
​ ​*​ ​forms​ is permitted under the Zach Literally Could Not Care Less If You 
 * Paid Him To License and GNU GPL.
 *
 * ​Zach Farmer ​is not liable for any misuse of this material.
​ ​*
*****************************************************************************/
/**
​ ​*​ ​@file​ ​main.c
​ ​*​ ​@brief​ ​Sockets Client Project 1
​ ​*
​ ​*​ ​This​ ​is the project 1 socket client
​ ​*
​ ​*​ ​@author​ ​Zach Farmer
​ ​*​ ​@date​ ​Mar 16 2018
​ ​*
​ ​*/

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "msgstruct.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
  

#define PORT 55171
#define HOSTNAME "beaglebone"

/**
​ ​*​ ​@brief​ ​prompt, prints options for request from server
​ ​*
​ ​*​ ​prints options
​ ​*
​ ​*/
void prompt()
{
    printf("\n0 - Light\n");
    printf("1 - Temp\n");
    printf("2 - Day/Night\n");
    printf("Enter a choice:");
}

/**
​ ​*​ ​@brief​ ​client-side socket process function
​ ​*
​ ​*​ ​sends a sample message struct to a server at a given location
​ ​*
​ ​*​ ​@param​ ​portno port number
 * @param hostname the hostname of the server
​ *
​ ​*​ ​@return​ 0 if successful
​ ​*/

int client(int portno, char* hostname)
{
    int sock_ret, err_ret;
    struct hostent *server;
    struct sockaddr_in server_addr;

    printf("Client Thread: Active\n");

    sock_ret = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ret < 0) 
        printf("Client: ERROR opening socket");

    server = gethostbyname(hostname);

    if (server == NULL) {
        printf("Client: ERROR, host does not seem to exist\n");
        return 1;
    }
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);
    server_addr.sin_port = htons(portno);
    if (connect(sock_ret,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
        printf("Client: ERROR connecting");

    char sensor = 1;
    double sensorval = 0;

    prompt();
    while(1)
    {
        if((sensor=getchar()) != '\n')
        {
            //char send = sensor %2;
            printf("Sending %c", sensor);    
            err_ret = write(sock_ret,&sensor, sizeof(char));
            if (err_ret < 0) 
                 printf("Client: ERROR writing to socket");

            err_ret = read(sock_ret,&sensorval,sizeof(double));
            if (err_ret < 0)
                 printf("Client: ERROR reading from socket");

            if(sensor == '2')
            {
                if(sensorval<2)
                {
                    printf("\nIt is night");      
                }
                else
                {
                    printf("\nIt is day");  
                }

            }
            else
            {
                printf("\nClient: Message received from server: %lf\n", sensorval);

            }

            prompt();
        }
    }
    printf("Client: Exiting\n");

    return 0;
}

int main(void)
{
    printf("ECEN 5013 Project1 Socket Client\n\n");

    client(PORT, HOSTNAME);
    return 0;
}
