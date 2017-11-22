#include "process.h"

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "entry.h"
#include "dictionary.h"

char basePath[] = "bin/data";

Header* responseHeader(Request* request, char* lastModified, unsigned int length){
    Header* headerList = createHeadersList();
    Header* headerBuffer;
    Value* valueBufferList;
    Value* valueBuffer;


    headerBuffer = createHeader("Date");
    valueBufferList = createValuesList();
    char* timeBuffer = getHTTPDate();
    valueBuffer = createValue(timeBuffer);
    addValue(valueBufferList, valueBuffer);
    free(timeBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);


    headerBuffer = createHeader("Server");
    valueBufferList = createValuesList();
    valueBuffer = createValue("HTTP Server v.0.3 by Felipe Pratali and Hugo Barreiro");
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);


    headerBuffer = createHeader("Connection");
    valueBufferList = createValuesList();
    valueBuffer = createValue(findConnection(request));
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);


    if(strcmp(lastModified,"")){
        headerBuffer = createHeader("Last-Modified");
        valueBufferList = createValuesList();
        valueBuffer = createValue(lastModified);
        addValue(valueBufferList, valueBuffer);
        addValues2Header(headerBuffer, valueBufferList);
        addHeader(headerList, headerBuffer);
    }


    headerBuffer = createHeader("Content-Length");
    valueBufferList = createValuesList();
    char lengthBuffer[10];
    sprintf(lengthBuffer, "%u", length);
    valueBuffer = createValue(lengthBuffer);
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);

    headerBuffer = createHeader("Content-Type");
    valueBufferList = createValuesList();
    valueBuffer = createValue("text/html");
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(headerList, headerBuffer);

    return headerList;
}

Response* responseError(Request* request, unsigned int errorCode){
    char* error;
    char* payload;

    switch(errorCode){
        case 400:   error = strdup("Bad Request");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  400 Bad Request\n</body>\n</html>");
                    break;
        case 403:   error = strdup("Forbidden");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  403 Forbidden\n</body>\n</html>");
                    break;
        case 404:   error = strdup("Not Found");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  404 Not Found\n</body>\n</html>");
                    break;
        case 405:   error = strdup("Method Not Allowed");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  405 Method Not Allowed\n</body>\n</html>");
                    break;
        case 501:   error = strdup("Not Implemented");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  501 Not Implemented\n</body>\n</html>");
                    break;
        default:    error = strdup("Internal Server Error");
                    payload = strdup("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\"/>\n<title>Document</title>\n</head>\n<body>\n  500 Internal Server Error\n</body>\n</html>");
    }

    Response* response = createResponse(request->httpVersion, errorCode, error);
    addHeaders2Response(response, responseHeader(request, "", strlen(payload)));
    addPayload2Response(response, payload);
    free(error);
    free(payload);
    return response;
}

Response* traceRequest(Request* request){
    Response* response = createResponse(request->httpVersion, 200, "OK");
    char* payload = printRequest(request);
    addHeaders2Response(response, responseHeader(request, "", strlen(payload)));
    fprintf(stderr, "boo");
    addPayload2Response(response, payload);
    free(payload);
    return response;
}

Response* optionsRequest(Request* request){
    Response* response = createResponse(request->httpVersion, 200, "OK");
    Header* header = responseHeader(request, "", 0);
    Header* headerBuffer;
    Value* valueBufferList;
    Value* valueBuffer;
    headerBuffer = createHeader("Allows");
    valueBufferList = createValuesList();
    valueBuffer = createValue("GET");
    addValue(valueBufferList, valueBuffer);
    valueBuffer = createValue("HEAD");
    addValue(valueBufferList, valueBuffer);
    valueBuffer = createValue("TRACE");
    addValue(valueBufferList, valueBuffer);
    valueBuffer = createValue("OPTIONS");
    addValue(valueBufferList, valueBuffer);
    addValues2Header(headerBuffer, valueBufferList);
    addHeader(header, headerBuffer);
    addHeaders2Response(response, header);
    return response;
}

Response* getRequest(char* path, Request* request, struct stat resourceStat){
    fprintf(stderr, "%s", printRequest(request));
    Response* response = NULL;

    int fileDescr;
    if( (fileDescr = open(path, O_RDONLY, 0600)) == -1)
        response = responseError(request, 500);
    else{
        char* payload = (char*) malloc(resourceStat.st_size+1);
        if( (read(fileDescr, payload, resourceStat.st_size+1)) == -1)
            response = responseError(request, 500);
        else{
            response = createResponse(request->httpVersion, 200, "OK");
            char* lastRaw = strdup(ctime(&resourceStat.st_mtime));
            char* lastBuffer = (char*) malloc((strlen(lastRaw)+1)*sizeof(char));
            memcpy(lastBuffer, lastRaw, strlen(lastRaw)-1);
            lastBuffer[strlen(lastRaw)-1]='\0';
            addHeaders2Response(response, responseHeader(request, lastBuffer, strlen(payload)));
            free(lastRaw);
            free(lastBuffer);
            addPayload2Response(response, payload);
        }
        free(payload);
    }
    return response;
}

Response* headRequest(char* path, Request* request, struct stat resourceStat){
    Response* response = NULL;

    int fileDescr;
    if( (fileDescr = open(path, O_RDONLY, 0600)) == -1)
        response = responseError(request, 500);
    else{
        response = createResponse(request->httpVersion, 200, "OK");
        char* lastRaw = strdup(ctime(&resourceStat.st_mtime));
        char* lastBuffer = (char*) malloc((strlen(lastRaw)+1)*sizeof(char));
        memcpy(lastBuffer, lastRaw, strlen(lastRaw)-1);
        lastBuffer[strlen(lastRaw)-1]='\0';
        addHeaders2Response(response, responseHeader(request, lastBuffer, resourceStat.st_size));
        free(lastRaw);
        free(lastBuffer);
    }
    return response;
}

Response* processRequest(Request* request){

    Response* response;
    char* path;
    if(request->type == REQ_TRACE)
        response = traceRequest(request);
    else if(request->type == REQ_OPTIONS)
        response = optionsRequest(request);
    else if(request->type == REQ_GET || request->type == REQ_HEAD){
        char* tok = strtok(request->resource, "/");
        if(tok == NULL){
            char* path = "bin/data/index.html";
            struct stat index;
            if(stat(path, &index)==-1)
                response = responseError(request, 404);
            else{
                switch(request->type){
                    case REQ_GET:
                                response = getRequest(path, request, index);
                                break;
                    case REQ_HEAD:
                                response = headRequest(path, request, index);
                                break;
                    default:    response = responseError(request, 501);
                }
            }
        }
        else if(!strcmp(tok, "word")){
            char* word = strtok(NULL, "");
            Entry* entry = getDefinition(word);
            char* payload = makeHTML(entry);
            freeEntry(entry);
            response = createResponse(request->httpVersion, 200, "OK");
            addHeaders2Response(response, responseHeader(request, "", strlen(payload)));
            //fprintf(stderr, "Adicionei os headers!\n");
            if(request->type == REQ_GET)
                addPayload2Response(response, payload);
            //fprintf(stderr, "Adicionei os payloads!\n");
            free(payload);
        }
        else
            response = responseError(request, 404);
    }
    else if(request->type == REQ_POST || request->type == REQ_PUT){
        char* tok = strtok(request->resource, "/");
        if(!strcmp(tok, "word")){
            char* word = strtok(NULL, "");
            includeDefinition(word, request->payload);
            response = createResponse(request->httpVersion, 200, "OK");
            addHeaders2Response(response, responseHeader(request, "", 0));
        }
        else
            response = responseError(request, 404);
    }
    else if(request->type == REQ_DELETE){
        char* tok = strtok(request->resource, "/");
        if(!strcmp(tok, "word")){
            tok = strtok(NULL, "/");
            char* id = strtok(NULL, "");
            removeDefinition(id);
            response = createResponse(request->httpVersion, 200, "OK");
            addHeaders2Response(response, responseHeader(request, NULL, 0));
        }
        else
            response = responseError(request, 404);
    }
    else
        response = responseError(request, 501);
    // else{
    //     path = (char*) malloc((strlen(basePath)+strlen(request->resource))*sizeof(char));
    //
    //     strcpy(path, basePath);
    //     strcat(path, request->resource);
    //
    //     //se nao foi encontrado o recurso retornar Not Found
    //     struct stat resourceStat;
    //     if(stat(path, &resourceStat)==-1)
    //         response = responseError(request, 404);
    //
    //     /* verifica se o recurso tem permissao de leitura.
    //     * se nao tiver retorna Forbidden
    //     */
    //     else{
    //         if(access(path, R_OK))
    //             response = responseError(request, 403);
    //
    //     /* verifica se o recurso eh um arquivo regular.
    //     * se for, abre o arquivo e retorna
    //     */
    //         else{
    //             if((resourceStat.st_mode & S_IFMT) == S_IFREG){
    //                 switch(request->type){
    //                     case REQ_GET:   response = getRequest(path, request, resourceStat);
    //                                 break;
    //                     case REQ_HEAD:  response = headRequest(path, request, resourceStat);
    //                                 break;
    //                     default:    response = responseError(request, 501);
    //                 }
    //             }
    //
    //     /* verifica se o diretorio tem permissao de execucao.
    //     * se nao, retorna Forbidden
    //     */
    //             else{
    //                 if(access(path, X_OK))
    //                     response = responseError(request, 403);
    //
    //                 // verifica se existe um arquivo regular dentro do diretorio de nome index.html
    //                 else{
    //                     char* index = (char*) malloc(strlen(path)+11);
    //                     strcpy(index, path);
    //                     strcat(index, "index.html");
    //                     struct stat indexStat;
    //                     if(stat(index, &indexStat)==-1){
    //         // se nao tiver, verifica se existe um arquivo regular de nome welcome.html
    //                         char* welcome = (char*) malloc(strlen(path)+13);
    //                         strcpy(welcome, path);
    //                         strcat(welcome, "welcome.html");
    //                         struct stat welcomeStat;
    //                         if(stat(welcome, &welcomeStat)==-1)
    //             // se nao tiver, retorna Not Found
    //                             response = responseError(request, 404);
    //
    //         /* verifica se o arquivo tem permissao de leitura
    //         * se nao, retorna Forbidden
    //         */
    //                         else{
    //                             if(access(welcome, R_OK))
    //                                 response = responseError(request, 403);
    //
    //         // se tiver, abre o arquivo e retorna
    //                             else{
    //                                 if((welcomeStat.st_mode & S_IFMT) == S_IFREG){
    //                                     switch(request->type){
    //                                         case REQ_GET:   response = getRequest(welcome, request, welcomeStat);
    //                                                     break;
    //                                         case REQ_HEAD:  response = headRequest(welcome, request, welcomeStat);
    //                                                     break;
    //                                         default:    response = responseError(request, 501);
    //                                     }
    //                                 }
    //                                 else response = responseError(request, 403);
    //                             }
    //                         }
    //                         free(welcome);
    //                     }
    //                     else{
    //                         if(access(index, R_OK))
    //                             response = responseError(request, 403);
    //
    //                         else{
    //                             if((indexStat.st_mode & S_IFMT) == S_IFREG){
    //                                 switch(request->type){
    //                                     case REQ_GET:   response = getRequest(index, request, indexStat);
    //                                                 break;
    //                                     case REQ_HEAD:  response = headRequest(index, request, indexStat);
    //                                                 break;
    //                                     default:    response = responseError(request, 501);
    //                                 }
    //                             }
    //                             else response = responseError(request, 403);
    //                         }
    //                     }
    //                     free(index);
    //                 }
    //             }
    //         }
    //     }
    //
    //     free(path);
    // }
    return response;
}
