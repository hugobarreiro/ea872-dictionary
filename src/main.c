#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "request.h"
#include "response.h"
#include "process.h"
#include "parser.h"
#include "logger.h"
#include "dictionary.h"

int socketDesc;
int yydebug=0;
void initServer(unsigned int port){
      initDictionary();
      socketDesc = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in server;
      server.sin_family = AF_INET;
      server.sin_port = htons(port);
      server.sin_addr.s_addr = INADDR_ANY;
      bind(socketDesc, (struct sockaddr*)&server, sizeof(server));
      listen(socketDesc, 5);
      char message[32];
      sprintf(message, "Server opened at port %u", port);
}

void finishServer(int param){
    endDictionary();
    close(socketDesc);
    exit(0);
}

Request* internalServerErrorRequest(){
    Request* request = createRequest("GET", "/errors/500.html", "HTTP/1.1");
    Header* headerList = createHeadersList();
    Header* headerBuffer = createHeader("Connection");
    Value* valueBufferList = createValuesList();
    Value* valueBuffer = createValue("keep-alive");
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);
    addHeaders2Request(request, headerList);
    return request;
}

int main(int argc, char** argv)
{
    initServer(5555);
    signal(SIGINT, finishServer);

    for(;;){
        int message;
        struct sockaddr_in client;
        int addrSize = sizeof(client);
        message = accept(socketDesc, (struct sockaddr*)&client, &addrSize);

        char request_message[1024];
        read(message, request_message, 1024);
        fprintf(stderr, "%s", request_message);

        Request* request = parseRequest(request_message);
        fprintf(stderr, "%s\n", printRequest(request));
        Response* response = processRequest(request);

        char* responseString = printResponse(response);
        fprintf(stderr, "%s\n", responseString);
        write(message, responseString, strlen(responseString+1));
        free(responseString);
        close(message);
    }
}
