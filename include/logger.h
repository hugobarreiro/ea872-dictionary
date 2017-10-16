#ifndef LOGGER
#define LOGGER

#include <stdio.h>
#include "request.h"
#include "response.h"

typedef FILE* Logger;

Logger loggerInit();
void logMessage(Logger logger, char* message);
void logRequest(Logger logger, Request* request);
void logResponse(Logger logger, Response* response);
void logError(Logger logger, char* error);
void loggerClose(Logger logger);

#endif
