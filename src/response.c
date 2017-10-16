#include "response.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Response* createResponse(char* httpVersion, unsigned int httpCode, char* codeText){
    Response* response = (Response*) malloc(sizeof(Response));
    response->httpVersion = strdup(httpVersion);
    response->httpCode = httpCode;
    response->codeText = strdup(codeText);
    response->headers = createHeadersList();
    response->payload = "\0";
    return response;
}

Response* addHeaders2Response(Response* response, Header* headersList){
    if(response->headers) free(response->headers);
    response->headers = headersList;
    return response;
}

Response* addPayload2Response(Response* response, char* payload){
    response->payload = strdup(payload);
    return response;
}

void freeResponse(Response* response){
    if(response){
        free(response->httpVersion);
        free(response->codeText);
        freeHeaders(response->headers);
        free(response->payload);
        free(response);
    }
}

char* printResponse(Response* response){
    char buffer[1000];
    buffer[0]='\0';
    if(response){
        sprintf(buffer, "%s %u %s\r\n", response->httpVersion, response->httpCode, response->codeText);
        strcat(buffer, printHeader(response->headers->next));
        strcat(buffer, "\r\n");
        strcat(buffer, response->payload);
        strcat(buffer, "\r\n");
    }
    return strdup(buffer);
}

char* getContentLength(Response* response){
    for(Header* header = response->headers; header; header=header->next)
        if(!strcmp(header->head,"Content-Length"))
            return header->values->next->val;
    return strdup("");
}
