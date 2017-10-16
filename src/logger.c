#include "logger.h"
#include "utils.h"

#include <stdlib.h>

Logger loggerInit(){
    char* date = getLoggerFileDate();
    char path[17];
    sprintf(path, "bin/log/%s.log", date);
    free(date);
    Logger logger = fopen(path, "a");
    if(!logger)
        exit(1);
    return logger;
}

void logMessage(Logger logger, char* message){
    char* date = getLoggerDate();
    fprintf(logger, "%s %s\n", date, message);
    free(date);
    fflush(logger);
}

void logRequest(Logger logger, Request* request){
    char logString[128];
    sprintf(logString, "Request: \"%s %s %s\"", getRequestType(request), request->resource, request->httpVersion);
    logMessage(logger, logString);
}

void logResponse(Logger logger, Response* response){
    char logString[128];
    sprintf(logString, "Response: \"%u %s %s bytes\"", response->httpCode, response->codeText, getContentLength(response));
    logMessage(logger, logString);
}
void logError(Logger logger, char* error){
    logMessage(logger, error);
}

void loggerClose(Logger logger){
    fclose(logger);
}
