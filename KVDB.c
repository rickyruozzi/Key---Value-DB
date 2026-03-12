#include "KVDB.h"

unsigned int hash(const char *key) {
    unsigned long h = 5381;
    int c;
    while ((c = *key++))
        h = ((h << 5) + h) + c;
    return h % TABLE_SIZE;
}

KVDb* db_create(void){
    KVDb* db = (KVDb*)malloc(sizeof(KVDb));
    if(db == NULL){
        printf("Impossibile allocare spazio per il database");
        exit(0);
    } 
    db->count = 0;
    for(int i = 0; i < TABLE_SIZE; i++) db->table[i] = NULL;
    return db;
}

void db_destroy(KVDb* db){
    if (!db){
        printf("Il puntatore al database non è valido");
        return;
    }
    for(int i=0; i<TABLE_SIZE; i++){
        entry *e = db->table[i];
        while(e!=NULL){
            entry* next = e->next;
            free(e);
            e=next;
        }
    }
    free(db);
}

int db_set(KVDb *db, const char *key, const char *value){
    if (!db || !key || !value) return -1;
    if(strlen(key) > KEY_LEN - 1 || strlen(value) > VALUE_LEN - 1){
        printf("Chiave o valore troppo lungo");
        return -1;
    }
    unsigned int index = hash(key);
    entry* e = db->table[index];

    while(e){
        if(strcmp(e->key, key) == 0){
            strncpy(e->value, value, VALUE_LEN-1);
            e->value[VALUE_LEN-1] = '\0';
            return 0;
        }
        e = e->next;
    }
    entry *ne = (entry*)malloc(sizeof(entry));
    if(!ne){
        printf("Impossibile allocare una nuova entry");
        return -1;
    }
    strncpy(ne->key, key, KEY_LEN-1);
    ne->key[KEY_LEN-1] = '\0';
    strncpy(ne->value, value, VALUE_LEN-1);
    ne->value[VALUE_LEN-1] = '\0';
    ne->next = db->table[index];
    db->table[index] = ne;
    db->count++; 
    return 0;
}

char* db_get(KVDb *db, const char *key){
    if (!db || !key){
        return NULL;
    }
    unsigned int index = hash(key);
    entry *e = db->table[index];
    while(e!=NULL){
        if(strcmp(e->key, key)==0){
            return e->value;
        }
        e = e->next; 
    }
    return NULL;
}

int db_del(KVDb* db, const char* key){
    if(!db || !key) return -1;
    unsigned int index = hash(key);
    entry *e = db->table[index];
    entry *prev = NULL;
    while(e){
        if(strcmp(e->key, key) == 0){
            if(prev){
                prev->next = e->next; 
                free(e);
                db->count--;
                return 0;
            }
            else{
                db->table[index] = e->next;
                free(e);
                db->count--;
                return 0;
            }
        }
        prev = e;
        e = e->next;
    }
    return -2;
}

void db_list(const KVDb* db){
    if(!db) return;
    printf("Chiave - Valore \n");
    for(int i=0; i<TABLE_SIZE; i++){
        entry *e = db->table[i];
        while(e){
            printf("%s - %s\n", e->key, e->value);
            e = e->next;
        }
    }
    printf("entry count: %zu\n", db->count);
}

int db_save(const KVDb *db, const char *path){
    if(!db || !path) return -1;
    FILE *F = fopen(path, "w");
    if(!F){
        printf("impossibile aprire il file");
        return -1;
    }
    for(int i=0; i<TABLE_SIZE; i++){
        entry *e = db->table[i];
        while(e){
            fprintf(F, "%s\t%s\n", e->key, e->value);
            e = e->next;
        }
    }
    fclose(F);
    return 0;
}

int db_load(KVDb *db, const char *path){
    if (!db || !path) return -1;
    FILE *f = fopen(path, "r");
    if (!f) return -1;   
    char line[KEY_LEN + VALUE_LEN + 4];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        char *tab = strchr(line, '\t');
        if (!tab) continue;
        *tab = '\0';
        db_set(db, line, tab + 1);
    }
    fclose(f);
    return 0;
}

void key_pattern_search(KVDb *db, const char *pattern){
    if(!db) return;
    if(!pattern) return;
    size_t count = 0;
    printf("Risultati pattern '%s':\n", pattern);
    for(int i=0; i<TABLE_SIZE; i++){
        entry *e = db->table[i];
        while(e){
            if(strstr(e->key, pattern) != NULL){ 
                printf("%s - %s\n", e->key, e->value);
                count++;
            }
            e = e->next;
        }
    }
    printf("Trovate %zu entries\n\n", count);
}

int main() {
    printf("Test KVDB - key_pattern_search\n\n");
    
    KVDb* db = db_create();
    
    db_set(db, "test1", "valore test 1");
    db_set(db, "best", "migliore valore");
    db_set(db, "keytest", "test chiave 3");
    db_set(db, "apple", "frutta");
    
    printf("=== Lista completa ===\n");
    db_list(db);
    
    printf("=== key_pattern_search 'test' ===\n");
    key_pattern_search(db, "test");
    
    printf("=== key_pattern_search 'e' ===\n");
    key_pattern_search(db, "e");
    
    printf("=== key_pattern_search 'xyz' ===\n");
    key_pattern_search(db, "xyz");
    
    printf("Get 'test1': %s\n", db_get(db, "test1"));
    
    db_destroy(db);
    
    printf("Test completati!\n");
    return 0;
}

