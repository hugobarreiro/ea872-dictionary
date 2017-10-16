#ifndef RESPONSE
#define RESPONSE

#include "header.h"

typedef struct response{
    char* httpVersion;
    unsigned int httpCode;
    char* codeText;
    Header* headers;
    char* payload;
} Response;

Response* createResponse(char* httpVersion, unsigned int httpCode, char* codeText);
Response* addHeaders2Response(Response* response, Header* headersList);
Response* addPayload2Response(Response* response, char* payload);
void freeResponse(Response* response);
char* printResponse(Response* response);
char* getContentLength(Response* response);

#endif
