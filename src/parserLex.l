%{
    #include "request.h"
    #include "parserYacc.tab.h"

    int yywrap(void){
        BEGIN 0;
        return 1;
    }
%}

%x REQ RES VERSION HEADER VAL PAYLOAD

func        ^(GET|HEAD|OPTIONS|POST|PUT|DELETE|TRACE)
resource    "/"[^ \r]*
field       ^[^ :\r]*
value       [^,\r]*
%%


<*>[ \t]   ;

<*>"#".*   ;

<INITIAL>\r\n   ;

<REQ>\r\n    {
            BEGIN HEADER;
            return END;
            }

<REQ>":"     {
        BEGIN VAL;
        return COLON;
        }

<REQ>","     {
        BEGIN VAL;
        return COMMA;
        }

{func}  {
        yylval.str = strdup(yytext);
        BEGIN RES;
        return FUNCTION;
        }

<RES>{resource}     {
                    yylval.str = strdup(yytext);
                    BEGIN VERSION;
                    return RESOURCE;
                    }

<VERSION>[^ \r]*    {
                    yylval.str = strdup(yytext);
                    BEGIN REQ;
                    return HTTP;
                    }

<HEADER>{field} {
                yylval.str = strdup(yytext);
                BEGIN REQ;
                return HEAD;
                }

<HEADER>\r\n  {
              BEGIN PAYLOAD;
              return REQEND;
              }

<VAL>{value} {
                yylval.str = strdup(yytext);
                BEGIN REQ;
                return VALUE;
                }

<PAYLOAD>.*        {
                yylval.str = strdup(yytext);
                return BODY;
                }
