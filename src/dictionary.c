#include "dictionary.h"
#include <mongoc.h>
#include <bson.h>

mongoc_client_t *client;
mongoc_database_t *dict;
mongoc_collection_t *defs;

void initDictionary(){
    mongoc_init();

    char* uriStr = "mongodb://localhost:27017";
    client = mongoc_client_new(uriStr);
    dict = mongoc_client_get_database(client, "dict");
    defs = mongoc_client_get_collection(client, "dict", "defs");
}

void includeDefinition(char* word, char* def){
    bson_t *document = BCON_NEW(
        "word", BCON_UTF8(word),
        "def", BCON_UTF8(def)
    );
    bson_oid_t oid;
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(document, "_id", &oid);

    bson_error_t error;
    if(!mongoc_collection_insert(defs, MONGOC_INSERT_NONE, document, NULL, &error))
        fprintf(stderr, "%s\n", error.message);

    bson_destroy(document);
}

void removeDefinition(char* id){
    bson_oid_t oid;
    bson_oid_init_from_string(&oid, id);
    bson_t *document = bson_new();
    BSON_APPEND_OID(document, "_id", &oid);

    bson_error_t error;
    if(!mongoc_collection_remove(defs, MONGOC_REMOVE_SINGLE_REMOVE, document, NULL, &error))
        fprintf(stderr, "Remove failed: %s\n", error.message);
    bson_destroy(document);
}

Entry* getDefinition(char* word){
    Entry* entry = createEntry(word);
    bson_t *query = bson_new();
    BSON_APPEND_UTF8 (query, "word", word);

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(defs, query, NULL, NULL);
    const bson_t *document;
    while(mongoc_cursor_next(cursor, &document)){
        bson_iter_t iter;
        bson_iter_t def, id;
        if(bson_iter_init(&iter, document) &&
           bson_iter_find_descendant(&iter, "def", &def) &&
           bson_iter_find_descendant(&iter, "_id", &id) &&
           BSON_ITER_HOLDS_OID(&id) &&
           BSON_ITER_HOLDS_UTF8(&def)){
               char oid[25];
               bson_oid_to_string(bson_iter_oid(&id), oid);
               addDefinition(entry, oid, bson_iter_utf8(&def, NULL));
        }
        bson_destroy(document);
    }
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    return entry;
}

void endDictionary(){
    mongoc_collection_destroy(defs);
    mongoc_database_destroy(dict);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
