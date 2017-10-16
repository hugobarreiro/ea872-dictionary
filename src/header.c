#include "header.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Header* createHeadersList(){
    return createHeader("\0");
}

Value* createValuesList(){
    return createValue("\0");
}

Header* createHeader(char* header){
    Header* headerNode = (Header*) malloc(sizeof(Header));
    headerNode->head = strdup(header);
    headerNode->values = createValuesList();
    headerNode->next = NULL;
    return headerNode;
}

Value* createValue(char* value){
    Value* valueNode = (Value*) malloc(sizeof(Value));
    valueNode->val = strdup(value);
    while(valueNode->val[0]==' ')
        valueNode->val=(valueNode->val)+1;
    valueNode->next = NULL;
    return valueNode;
}

Value* addValue(Value* valuesList, Value* value){
    Value* prev = valuesList;
    Value* next = valuesList->next;
    while(next){
        prev = next;
        next = prev->next;
    }
    prev->next = value;
    return valuesList;
}

Header* addHeader(Header* headersList, Header* header){
    Header* prev = headersList;
    Header* next = headersList->next;
    while(next){
        prev = next;
        next = prev->next;
    }
    prev->next = header;
    return headersList;
}

Header* addValues2Header(Header* header, Value* valuesList){
    if(header->values) free(header->values);
    header->values = valuesList;
    return header;
}

void freeHeaders(Header* headersList){
    if(headersList){
        freeHeaders(headersList->next);
        free(headersList->head);
        free(headersList->values);
        free(headersList);
    }
}

void freeValues(Value* valuesList){
    if(valuesList){
        freeValues(valuesList->next);
        free(valuesList->val);
        free(valuesList);
    }
}

char* printHeader(Header* header){
    char buffer[500];
    buffer[0] = '\0';
    if(header){
        sprintf(buffer, "%s : ", header->head);
        strcat(buffer, printValue(header->values->next));
        strcat(buffer, printHeader(header->next));
    }
    else strcat(buffer, "\r\n");
    return strdup(buffer);
}

char* printValue(Value* value){
    char buffer[500];
    buffer[0]='\0';
    if(value){
        sprintf(buffer,"%s", value->val);
        if(value->next)
            strcat(buffer,",");
        strcat(buffer, printValue(value->next));
    }
    else strcat(buffer, "\r\n");
    return strdup(buffer);
}
