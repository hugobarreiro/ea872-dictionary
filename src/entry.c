#include "entry.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Definition* newDef(char* id, char* def){
    Definition* definition = (Definition*) malloc(sizeof(Definition));
    definition->id = strdup(id);
    definition->def = strdup(def);
    definition->next = NULL;
    return definition;
}

Entry* createEntry(char* word){
    Entry* entry = (Entry*) malloc(sizeof(Entry));
    entry->word = strdup(word);
    entry->defs = newDef("", "");
    return entry;
}

Entry* addDefinition(Entry* entry, char* id, char* def){
    Definition* prev = entry->defs;
    Definition* next = prev->next;
    while(next){
        prev = next;
        next = prev->next;
    }
    prev->next = newDef(id, def);
    return entry;
}

void freeDef(Definition* def){
    if(def){
        freeDef(def->next);
        free(def->id);
        free(def->def);
        free(def);
    }
}

void freeEntry(Entry* entry){
    if(entry){
        free(entry->word);
        freeDef(entry->defs);
        free(entry);
    }
}

char* def2HTML(Definition* def, unsigned int num){
    char buffer[1000];
    sprintf(buffer, "<def id=\"%s\">\n<p>[%u] %s\n<button>Remover</button></p>\n</def>\n", def->id, num, def->def);
    return strdup(buffer);
}

char* makeHTML(Entry* entry){
    char buffer[10000];
    sprintf(buffer, "<h4>%s</h4>\n", entry->word);
    Definition* def = entry->defs->next;
    unsigned int num = 1;
    while(def){
        strcat(buffer, def2HTML(def, num));
        def = def->next;
        num++;
    }
    strcat(buffer, "<p><input type=\"button\" value=\"Incluir nova definição\" id=\"new\"></p>\n");
    strcat(buffer, "<form id=\"newDef\">\n<textarea></textarea><input type=\"submit\" id=\"submitNew\" value=\"Incluir\">\n</form>\n");
    return strdup(buffer);
}
