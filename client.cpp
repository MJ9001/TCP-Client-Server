#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_MSG_LEN 256
#define DEFAULT_PORT 23220
#define DO_LOOP true

int main(int argc, char **args)
{
    std::string hostname = "127.0.0.1";
    hostent *connHost;
    sockaddr_in serveraddr;
    int socketServer, recvLen, connPort = DEFAULT_PORT;
    char cBuffer[MAX_MSG_LEN+1];
    char recvBuffer[MAX_MSG_LEN];

    if(argc >= 2)
        connPort = atoi(args[1]);
    if(argc >= 3)
        hostname = std::string(args[2]);

    //Get server IP
    connHost = gethostbyname(hostname.c_str());

    serveraddr.sin_family = AF_INET; //IPv4
    memcpy((char*) &serveraddr.sin_addr, connHost->h_addr_list[0], connHost->h_length); //Set address
    serveraddr.sin_port = htons(connPort);//Set port to listen on

    //Setup socket
    if(!(socketServer = socket(PF_INET, SOCK_STREAM, 0)))
    {
        perror("Error: could not create socket.\n");
        //printf("Error code: %d\n", errno);
        exit(1);
    }
    //Connect to server
    if(connect(socketServer, (sockaddr*)&serveraddr, sizeof(serveraddr)))
    {
        perror("Error: could not connect.\n");
        //printf("Error code: %d\n", errno);
        exit(1);
    }
    do {
        //Get server welcome message
        recvLen = recv(socketServer, recvBuffer, sizeof(recvBuffer), 0);
        printf("%s", recvBuffer);
        scanf("%[^\n]", cBuffer);//Read message to send to server
        if(strlen(cBuffer) >= MAX_MSG_LEN)
        {
            printf("\033[1;31mError: Message is longer then %d, string has been cut to that length.\033[0m;\n", MAX_MSG_LEN);
            cBuffer[MAX_MSG_LEN-1] = '\0';
        }
        getchar();//Pop off \n
        //Send message to server
        send(socketServer, cBuffer, strlen(cBuffer), 0);
        if(!strcmp(cBuffer, "exit"))//If message is exit terminate program
            break;
        recvLen = recv(socketServer, recvBuffer, sizeof(recvBuffer), 0);//Recieve response from server
        printf("%s\n", recvBuffer);//Display modified text.
    }
    while(DO_LOOP);
    close(socketServer);

}