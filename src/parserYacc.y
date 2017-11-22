%{
    #define YYDEBUG 1
    #include "request.h"
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <signal.h>

    void yyerror(const char* str) {
        fprintf(stderr, "Parse error: %s\n", str);
        //raise(SIGINT);
    }
    int yylex();

    Request* request;
    Request* pickRequest(){
        return request;
    }

%}
%debug
%define parse.error verbose

%union{
    char* str;
    Value* value;
    Header* header;
    Request* request;
}

%token COLON COMMA VALUE HEAD FUNCTION RESOURCE HTTP END REQEND BODY ENDOFFILE

%%

export : request                            {
                                            request = $<request>1;
                                            };
request : requestLine headers REQEND BODY   {
                                            $<request>$ = addHeaders2Request($<request>1, $<header>2);
                                            $<request>$ = addPayload2Request($<request>$, $<str>4);
                                            };
request : requestLine headers REQEND        {
                                            $<request>$ = addHeaders2Request($<request>1, $<header>2);
                                            }
requestLine : FUNCTION RESOURCE HTTP END    {
                                            $<request>$ = createRequest($<str>1, $<str>2, $<str>3);
                                            free($<str>1);
                                            free($<str>2);
                                            free($<str>3);
                                            };
headers : headers header                    {
                                            $<header>$ = addHeader($<header>1, $<header>2);
                                            };
headers : header                            {
                                            $<header>$ = createHeadersList();
                                            $<header>$ = addHeader($<header>$, $<header>1);
                                            };
header : HEAD COLON values END              {
                                            $<header>$ = createHeader($<str>1);
                                            free($<str>1);
                                            $<header>$ = addValues2Header($<header>$, $<value>3);
                                            };
values : %empty                             {
                                            $<value>$ = createValuesList();
                                            };
values : values COMMA VALUE                 {
                                            Value* temp = createValue($<str>3);
                                            free($<str>3);
                                            $<value>$ = addValue($<value>1, temp);
                                            };
values : VALUE                              {
                                            $<value>$ = createValuesList();
                                            Value* temp = createValue($<str>1);
                                            free($<str>1);
                                            $<value>$ = addValue($<value>$, temp);
                                            };
%%
