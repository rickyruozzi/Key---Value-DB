#include "KVDB.h"
#include <stdio.h>

unsigned int hash(const char *key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % TABLE_SIZE;
}

int main() {
    KVDb* db = db_create();
    
    db_set(db, "test1", "value1");
    db_set(db, "best", "value2");
    db_set(db, "keytest", "value3");
    
    printf("\\n=== LIST ALL ===\\n");
    db_list(db);
    
    printf("\\n=== PATTERN 'test' ===\\n");
    key_pattern_search(db, "test");
    
    printf("\\n=== PATTERN 'x' ===\\n");
    key_pattern_search(db, "x");
    
    char* val = db_get(db, "test1");
    printf("\\nGet 'test1': %s\\n", val ? val : "NULL");
    
    db_destroy(db);
    return 0;
}
