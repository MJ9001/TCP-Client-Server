#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_MSG_LEN 256
#define DEFAULT_PORT 23220
#define DO_LOOP true

int main(int argc, char **args)
{
    int socketServer = 0, socketClient = 0, respons, connPort = DEFAULT_PORT;
    int8_t i;
    socklen_t clientBufferLen;
    sockaddr_in serveraddr, clientaddr;
    char cBuffer[256];
    std::string prompt = "Please enter in message to +1 ASCII.\nEnter 'exit' to quit at any time.\n";

    if(argc >= 2)
        connPort = atoi(args[1]);

    //Setup socket
    if(!(socketServer = socket(AF_INET, SOCK_STREAM, 0)))
    {
        perror("Error: could not create socket.\n");
        printf("Error code: %d\n", errno);
        exit(1);
    }

    //Setup server address information
    //serveraddr = (sockaddr_in*)calloc(sizeof(sockaddr_in), 1);
    serveraddr.sin_family = AF_INET; //IPv4
    serveraddr.sin_addr.s_addr = INADDR_ANY; //Accept any address
    serveraddr.sin_port = htons(connPort);//Set port to listen on


    //Bind socket to address
    if(bind(socketServer, (sockaddr*)&serveraddr, sizeof(serveraddr)))
    {
        perror("Error: could not bind to port.\nIs it in use?\n");
        printf("Error code: %d\n", errno);
        exit(1);
    }

    //Listen for a client
    listen(socketServer, 0);
    printf("Waiting for client to connect...\n");

    //Setup client buffer**
    clientBufferLen = sizeof(clientBufferLen);

    //Accept client
    if(!(socketClient = accept(socketServer, (sockaddr *)&clientaddr, &clientBufferLen)))
    {
        perror("Error: could not connect to client.\n");
        printf("Error code: %d\n", errno);
        exit(1);
    }

    printf("Client connected.\n");

    do {
        //Send request message promt to client
        send(socketClient, prompt.c_str(), prompt.length()+1, 0);

        //Recieve from client
        if(recv(socketClient, cBuffer, MAX_MSG_LEN, 0) < 0)
        {
            perror("Error: could not recieve packet from client.\n");
            printf("Error code: %d\n", errno);
            exit(1);
        }

        i = 0;
        while(cBuffer[i] != 0)
            cBuffer[i++]++; 
            if(!strcmp(cBuffer, "exit"))
                break;
        cBuffer[i] = '\0';
        //Send to client
        send(socketClient, cBuffer, MAX_MSG_LEN, 0);

    } while(DO_LOOP);

    //Close connection
    close(socketClient);
}