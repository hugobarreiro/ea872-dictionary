#ifndef DICTIONARY
#define DICTIONARY

#include "entry.h"

void initDictionary();
void includeDefinition(char* word, char* def);
void removeDefinition(char* id);
Entry* getDefinition(char* word);
void endDictionary();

#endif
