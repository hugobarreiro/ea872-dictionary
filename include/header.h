#ifndef HEADER_TAD
#define HEADER_TAD

typedef struct value{
    char *val;
    struct value *next;
} Value;

typedef struct header{
    char *head;
    struct header *next;
    struct value *values;
} Header;

Header*  createHeadersList();
Value*   createValuesList();

Header*  createHeader(char* header);
Value*   createValue(char* value);

Value*   addValue(Value* valuesList, Value* value);
Header*  addHeader(Header* headersList, Header* header);

Header*  addValues2Header(Header* header, Value* valuesList);

void     freeHeaders(Header* headersList);
void     freeValues(Value* valuesList);

char*     printHeader(Header* header);
char*     printValue(Value* value);

#endif
