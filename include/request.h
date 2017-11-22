#ifndef REQUEST
#define REQUEST

#include "header.h"

typedef enum requestType {REQ_GET, REQ_HEAD, REQ_TRACE, REQ_OPTIONS, REQ_PUT, REQ_POST, REQ_DELETE} requestType;

typedef struct request{
    requestType type;
    char *resource;
    char *httpVersion;
    Header *headers;
    char *payload;
} Request;

Request* createRequest(char* type, char* resource, char *httpVersion);
Request* addHeaders2Request(Request* request, Header* headersList);
char* findConnection(Request* request);
void freeRequest(Request* request);
char* printRequest(Request* request);
char* getRequestType(Request* request);
Request* addPayload2Request(Request* request, char* payload);

#endif
