#include "KVDB.h"
#include <stdio.h>
#include <string.h>

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

    // === TEST append_key ===
    printf("\\n=== TEST append_key ===\\n");
    
    // Test successful append
    db_set(db, "append_test", "hello");
    printf("Before append: get('append_test') = %s\\n", db_get(db, "append_test"));
    append_key(db, "append_test", " world");
    printf("After append: get('append_test') = %s (should be 'hello world')\\n", db_get(db, "append_test"));
    
    // Test append to non-existing
    int res = append_key(db, "nonexistent", " foo");
    printf("Append to non-existing returned %d (should be -2)\\n", res);
    
    // Test overflow (VALUE_LEN is 1024, so truncate for test)
    db_set(db, "overflow_test", "a");
    char long_str[VALUE_LEN];
    memset(long_str, 'x', VALUE_LEN - 1);
    long_str[VALUE_LEN - 1] = '\\0';  // Full length
    res = append_key(db, "overflow_test", long_str);  // 1 + 1023 = 1024 < 1024? No, >= VALUE_LEN
    printf("Overflow append returned %d (should be -1)\\n", res);
    printf("overflow_test value after: %s (should still 'a')\\n", db_get(db, "overflow_test"));
    
    // === TEST increase_decrease_value ===
    printf("\\n=== TEST increase_decrease_value ===\\n");
    
    // Test setup numeric
    db_set(db, "counter", "42");
    printf("Before increase: get('counter') = %s\\n", db_get(db, "counter"));
    
    // Test increase
    increase_decrease_value(db, "counter", true);
    printf("After increase: get('counter') = %s (should be 43)\\n", db_get(db, "counter"));
    
    // Test decrease
    increase_decrease_value(db, "counter", false);
    printf("After decrease: get('counter') = %s (should be 42)\\n", db_get(db, "counter"));
    
    // Test non-numeric
    db_set(db, "text", "abc");
    increase_decrease_value(db, "text", true);
    printf("Non-numeric after increase attempt: %s (should still abc)\\n", db_get(db, "text"));
    
    // Test zero
    db_set(db, "zero", "0");
    increase_decrease_value(db, "zero", false);
    printf("Zero decrease: %s (should be -1)\\n", db_get(db, "zero"));
    
    // Test missing key
    increase_decrease_value(db, "missing", true);
    printf("Missing key handled (no crash)\\n");
    
    printf("\\n=== FINAL LIST ===\\n");
    db_list(db);
    
    db_destroy(db);
    return 0;
}

