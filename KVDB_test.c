#include "KVDB.h"
#include <stdio.h>

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
    
    // Test exist_key (optimized)
    printf("\\n=== TEST exist_key ===\\n");
    printf("exist_key('test1'): %d\\n", exist_key(db, "test1"));
    printf("exist_key('nonexistent'): %d\\n", exist_key(db, "nonexistent"));
    
    // Test rename_key
    printf("\\n=== TEST rename_key ===\\n");
    printf("Before rename: get('test1') = %s\\n", db_get(db, "test1"));
    printf("get('newtest1') before = %s\\n", db_get(db, "newtest1") ? db_get(db, "newtest1") : "NULL");
    rename_key(db, "test1", "newtest1");
    printf("After rename: get('test1') = %s\\n", db_get(db, "test1") ? db_get(db, "test1") : "NULL");
    printf("get('newtest1') = %s (should be value1)\\n", db_get(db, "newtest1"));
    
    // Test rename to existing
    printf("\\n=== TEST rename to existing ===\\n");
    rename_key(db, "newtest1", "best");  // Should print "already exists" and not overwrite
    printf("get('best') after attempt = %s (should still be value2)\\n", db_get(db, "best"));
    
    printf("\\n=== FINAL LIST ===\\n");
    db_list(db);
    
    db_destroy(db);
    return 0;
}
