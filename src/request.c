#include "request.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Request* createRequest(char* type, char* resource, char *httpVersion){
    Request* request = (Request*) malloc(sizeof(Request));
    if(!strcmp(type,"GET")) request->type = REQ_GET;
    if(!strcmp(type,"HEAD")) request->type = REQ_HEAD;
    if(!strcmp(type,"PUT")) request->type = REQ_PUT;
    if(!strcmp(type,"POST")) request->type = REQ_POST;
    if(!strcmp(type,"DELETE")) request->type = REQ_DELETE;
    if(!strcmp(type,"TRACE")) request->type = REQ_TRACE;
    if(!strcmp(type,"OPTIONS")) request->type = REQ_OPTIONS;

    request->resource = strdup(resource);
    request->httpVersion = strdup(httpVersion);
    request->headers = createHeadersList();
    return request;
}

Request* addHeaders2Request(Request* request, Header* headersList){
    if(request->headers) free(request->headers);
    request->headers = headersList;
    return request;
}

void freeRequest(Request* request){
    if(request){
        free(request->resource);
        free(request->httpVersion);
        freeHeaders(request->headers);
        free(request);
    }
}
char* getRequestType(Request* request){
    char* type;
    switch(request->type){
        case REQ_GET:       type=strdup("GET");
                            break;
        case REQ_HEAD:      type=strdup("HEAD");
                            break;
        case REQ_PUT:       type=strdup("PUT");
                            break;
        case REQ_POST:      type=strdup("POST");
                            break;
        case REQ_DELETE:    type=strdup("DELETE");
                            break;
        case REQ_TRACE:     type=strdup("TRACE");
                            break;
        case REQ_OPTIONS:   type=strdup("OPTIONS");
                            break;
        default:            type=strdup("?");
    }
    return type;
}

char* printRequest(Request* request){
    char buffer[1000];
    buffer[0]='\0';
    char* type = getRequestType(request);
    sprintf(buffer,"%s %s %s\r\n", type, request->resource, request->httpVersion);
    free(type);
    strcat(buffer, printHeader(request->headers->next));
    strcat(buffer, "\r\n");
    return strdup(buffer);
}

char* findConnection(Request *request){
    for(Header* header = request->headers; header; header=header->next)
        if(!strcmp(header->head,"Connection"))
            return header->values->next->val;
    return strdup("");
}
