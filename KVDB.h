#ifndef KVDB_H
#define KVDB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define KEY_LEN 256
#define VALUE_LEN 1024
#define TABLE_SIZE 256

typedef struct entry {
    char key[KEY_LEN];
    char value[VALUE_LEN];
    struct entry* next;
} entry;

typedef struct KVDb {
    entry* table[TABLE_SIZE];
    size_t count;
} KVDb;

KVDb* db_create(void);
void   db_destroy(KVDb *db);
int    db_set(KVDb *db, const char *key, const char *value);
char* db_get(KVDb *db, const char *key);
int    db_del(KVDb *db, const char *key);
void   db_list(const KVDb *db);

int    db_save(const KVDb *db, const char *path);
int    db_load(KVDb *db, const char *path);


void key_pattern_search(KVDb *db, const char *pattern);
bool exist_key(KVDb *db, const char* key);
void db_list_values(KVDb *db);
void find_value(KVDb *db, const char* value);

int rename_key(KVDb* db, const char *old_key, const char *new_key);
int append_key(KVDb *db, const char *key, const char *string_to_append);

unsigned int hash(const char *key);

#endif