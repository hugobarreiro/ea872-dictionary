#ifndef ENTRY
#define ENTRY

typedef struct definition{
    char* id;
    char* def;
    struct definition* next;
} Definition;

typedef struct entry{
    char* word;
    struct definition* defs;
} Entry;

Entry* createEntry(char* word);
Entry* addDefinition(Entry* entry, char* id, char* def);
void freeEntry(Entry* entry);
char* makeHTML(Entry* entry);

#endif
