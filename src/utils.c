#include "utils.h"
#include <stdio.h>
#include <string.h>

char* getDate(char* fmt){
    char dateComm[32];
    sprintf(dateComm, "echo -n \"$(date %s)\"", fmt);
    FILE* dateStream = popen(dateComm, "r");
    char date[64];
    fgets(date, 64, dateStream);
    return strdup(date);
}

char* getLoggerFileDate(){
    return getDate("+%Y%m%d");
}

char* getLoggerDate(){
    return getDate("-R");
}

char* getHTTPDate(){
    return getDate("-R");
}
