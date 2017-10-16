#include "parser.h"
#include "parserYacc.tab.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
Request* pickRequest();

Request* parseRequest(char* message){
    YY_BUFFER_STATE buffer = yy_scan_string(message);
    yyparse();
    Request* request = pickRequest();
    yy_delete_buffer(buffer);
    return request;
}
