#include "KVDB.h"

KVDb* db_create(void){
    KVDb* db = (KVDb*)malloc(sizeof(KVDb));
    if(db == NULL){
        pritnf("Impossibile allocare spazio per il database");
        exit(0);
    } 
    return db;
}

void db_destroy(KVDb* db){ //distrugge un database creato
    if (!db){
        printf("Il puntatore al database non è valido");
        return;
    }
    for(int i=0; i<TABLE_SIZE; i++){
        entry *e = db->table[i]; //ogni elemento dell'array di puntatori table, punta alla testa di una lista concatenata di coppie key - value
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
    if(strlen(key) > KEY_LEN || strlen(value) > VALUE_LEN){
        printf("Chiave o valore troppo lungo");
        return -1;
    }
    unsigned int index = hash(key);
    entry* e = db->table[index];

    while(e){
        //aggiorniamo se la chiave è già presente
        if(strcmp(e->key, key) == 0){
            strncpy(e->value, value, sizeof(VALUE_LEN)-1);
            e->value[VALUE_LEN-1] = "\0";
            return 0; //il valore è modificato e non ci interessa la procedura di aggiunta di un nuovo nodo
        }
    }
    entry *ne = (entry*)malloc(sizeof(entry));
    if(!ne){
        printf("Impossibile allocare una nuova entry");
        return -1;
    }
    strncpy(ne->key, key, sizeof(KEY_LEN)-1);
    ne->key[KEY_LEN-1] = "\0";
    strncpy(ne->value, value, sizeof(VALUE_LEN)-1);
    ne->key[VALUE_LEN-1] = "\0";  
    ne->next = db->table[index];
    db->table[index] = ne;
    db->count++; 
    return 0;
}

char* db_get(KVDb *db,const char *key){
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
                return 0;
            }
            else{
                db->table[index] = e->next;
                free(e);
                return 0;
            }
        }
        prev = e;
        e = e->next;
    }
    return -2;
}